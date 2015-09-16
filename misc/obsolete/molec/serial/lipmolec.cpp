/**
 * Lipschitzian solver for cluster conf optimization.
 * 
 * @author Mikhail Posypkin, ISA RAS, posypkin@isa.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include <kernel/bnblib/wtraverse.hpp>
#include <problems/optlib/mbfgs.hpp>
#include <problems/cover/common/go.hpp>
#include <problems/cover/common/interdiscarder.hpp>
#include <problems/cover/common/lipzfdiscarder.hpp>
#include <problems/cover/common/lipzgdiscarder.hpp>
#include <problems/cover/common/decomposer.hpp>
#include <problems/cover/common/linconsdiscarder.hpp>
#include <problems/molec/common/lipmol.hpp>
#include <problems/molec/common/mors.hpp>
#include <problems/molec/common/lenjon.hpp>
#include <problems/molec/common/cluster.hpp>
#include <kernel/bnblib/seq.hpp>

template <typename FloatType> class SMolec  {

  public:

    void Main (int argc, char *argv[])
    {
      int n, N, *tuple, side;      
      typedef GOFactory<double> Fact;
      FloatType scale = 1.;
      FloatType * a, *b;
      const FloatType eps1 = 0.1;
      const FloatType hop = 0.8;
      const FloatType dis = 0.7;
      const FloatType rho = 14.;
      const int ns = 4;
      const int poolsize = 1024;
      Decomposer<Fact> decomp;

      Cluster<FloatType> cluster;
      cluster.mNAtoms = atoi (argv[1]);
      cluster.mSpaceD = atoi (argv[2]);
      n = cluster.mSpaceD * cluster.mNAtoms;
      cluster.setDim(n);
      a = new FloatType[n];
      b = new FloatType[n];  
      for(int i = 0; i < n; i ++) {
	a[i] = -1.;
	b[i] = 1.;
      }
      /*
      for(int na = 0; na < cluster.mNAtoms; na ++) {
	int s = na * cluster.mSpaceD;
	if(na == 0) {
	  for(int i = 0; i < cluster.mSpaceD; i ++) {
	    a[s + i] = b[s + i] = 0.;
	  } 
	} else if(na == (cluster.mNAtoms - 1)) {
	  for(int i = 0; i < cluster.mSpaceD; i ++) {
	    if(i == 0) {
	      a[s + i] = 1.;
	      b[s + i] = 2.;
	    } else {
	      a[s + i] = b[s + i] = 0.;
	    }
	  }
	} else if((cluster.mSpaceD == 3) && (na == 1)){
	      a[s] = 0.;
	      b[s] = 1.;
	      a[s + 1] = 0.;
	      b[s + 1] = 1.;
	      a[s + 2] = 0.;
	      b[s + 2] = 0.;
	} else {
	  for(int i = 0; i < cluster.mSpaceD; i ++) {
	    if(i == 0) {
	      if(na <= cluster.mNAtoms / 2) {
		a[s + i] = 0.;
		b[s + i] = 1.;
	      } else {
		a[s + i] = 0.;
		b[s + i] = 2.;
	      }       
	    } else if(i == cluster.mSpaceD - 1){
	      if(na <= cluster.mNAtoms / 2) {
		a[s + i] = 0.;
		b[s + i] = 1.;
	      } else {
		a[s + i] = -1.;
		b[s + i] = 1.;
	      }
	    } else {
	      a[s + i] = -2.;
	      b[s + i] = 2.;
	    }
	  }
	}
      }
      */
      VecUtils::vecPrint(n, a);
      VecUtils::vecPrint(n, b);
      //cluster.mPotential = new Mors<FloatType> (14.);
      cluster.mPotential = new LenJon<FloatType> ();
      LipMolec<FloatType> im(cluster.mNAtoms, cluster.mSpaceD, cluster.mPotential);
      GOInitialData<FloatType> id(a, b, eps1, eps1, &im);
      Fact fact;
      fact.setInitialData(&id);
      
      LipzFuncDiscarder<double> dscc;
      dscc.setEps(eps1);
      dscc.setObjective(&im);

      LipzGradDiscarder<double> dscg;
      dscg.setEps(eps1);
      dscg.setObjective(&im);
      dscg.setInitialData(&id);
      dscg.getOptions() |= LipzGradDiscarder<double>::Options::CHECK_GRAD;
      dscg.getOptions() |= LipzGradDiscarder<double>::Options::CHECK_GRAD_COMP;
      dscg.getOptions() |= LipzGradDiscarder<double>::Options::DO_GRAD_BALL_CUT;
      dscg.getOptions() |= LipzGradDiscarder<double>::Options::DO_PRIMARY_BALL_CUT;
      dscg.getOptions() |= LipzGradDiscarder<double>::Options::DO_UNCONS_BALL_CUT;

      
      fact.addDiscarder(&decomp);
      fact.addDiscarder(&dscg);

#if 1
      for(int i = 0; i < cluster.mNAtoms - 1; i ++) {
	fact.addDiscarder(getLinDiscarder(true, cluster.mNAtoms, cluster.mSpaceD, i));
	fact.addDiscarder(getLinDiscarder(false, cluster.mNAtoms, cluster.mSpaceD, i));
      }
#endif

#if 1
      for(int i = 0; i < cluster.mSpaceD; i ++) {
	fact.addDiscarder(getMassDiscarder(1, cluster.mNAtoms, cluster.mSpaceD, i));
	fact.addDiscarder(getLinDiscarder(-1, cluster.mNAtoms, cluster.mSpaceD, i));
      }
#endif
      
      typedef WTraverse < Fact > Traverse;
      typedef BNBSeqSolver < Traverse > Solver;
      Solver solver(&fact);
      
      solver.setDebugOptions(
	   Solver::Options::PRINT_RESULTING_STAT 
	   //|Solver::Options::PRINT_STATE
	  );
      solver.solve ();

      if(solver.getSolutionContainer()->empty())
	printf("Empty container\n");
      printf("Minimum = %s\n", solver.getSolutionContainer()->top().toString().c_str());

//      printf ("Function minimum is %lf\n", solver.getRecord());    
    }
  private:
    LinConsDiscarder<double>* getLinDiscarder(bool left, int nat, int spaced, int atom)
    {
      int n = nat * spaced;
      int s = 0;
      LinObjective<double> *lo = new LinObjective<double>();
      lo->setDim(n);
      double *coe = (double*)malloc((n + 1) * sizeof(double));
      for(int j = 0; j <= n; j ++) coe[j] = 0.;
      if(left) {
	coe[spaced * atom + s] = 1.;
	coe[spaced * (atom + 1) + s] = -1.;
	coe[n] = 0.;
      } else {
	coe[spaced * atom + s] = -1.;
	coe[spaced * (atom + 1) + s] = 1.;
	coe[n] = -1.;
      }
      lo->setCoeff(coe);
      LinConsDiscarder<double> *lcons = new LinConsDiscarder<double>();
      lcons->setObjective(lo);
      lcons->getOptions() |= LinConsDiscarder<double>::Options::DO_CUT;
      return lcons;
    }

    LinConsDiscarder<double>* getMassDiscarder(int dir, int nat, int spaced, int dim)
    {
      int n = nat * spaced;
      LinObjective<double> *lo = new LinObjective<double>();
      lo->setDim(n);
      double *coe = (double*)malloc((n + 1) * sizeof(double));
      for(int j = 0; j <= n; j ++) coe[j] = 0.;
      for(int i = dim; i < n; i += nat) {
	coe[i] = 1. * dir;
      }
      lo->setCoeff(coe);
      LinConsDiscarder<double> *lcons = new LinConsDiscarder<double>();
      lcons->setObjective(lo);
      lcons->getOptions() |= LinConsDiscarder<double>::Options::DO_CUT;
      return lcons;
    }


    static Potential<FloatType>* mPotential;

};

main(int argc, char* argv[])
{
  SMolec<double> mol;
  mol.Main(argc, argv);
}
