#include "rna.hpp"
#include "rnagen.hpp"
#include <kernel/bnblib/msheur2.hpp>
#include <kernel/bnblib/wtraverse.hpp>
#include <problems/go/go.hpp>
#include <problems/go/gopack.hpp>
#include <problems/optlib/fletchsearch.hpp>
#include <kernel/bnblib/basicheurmaster.hpp>
#include <problems/go/contheur.hpp>
#include <problems/optlib/steepdesc.hpp>
#include "sultan.hpp"
#include <problems/optlib/basinhop.hpp>
#include <problems/go/spheregenerator.hpp>
#include <problems/go/ortgenerator.hpp>
#include <kernel/bnblib/chainsolgen.hpp>
#include <problems/go/gosolutionsurface.hpp>
#include <problems/optlib/distdivers.hpp>

#define BNB_MAXIMUM_COMP_TAG "maxcomp"

#define BNB_MAXIMUM_COMP_VALUE_TAG "val"

#define BNB_MAXIMUM_COMP_POSITION_TAG "pos"

void initID(int n, RNA & rna, GOInitialData<double> & id)
{
  id.mObj = &rna;
  id.mA = (double*) malloc(n * sizeof(double));
  id.mB = (double*) malloc(n * sizeof(double));
  
  /*
  for(int i = 0; i < n; i ++) {
    id.mA[i] = -1.;
    id.mB[i] = 1.;
  }
  id.mA[0] = 0.;
  */
  /*
  for(int i = 0; i < n; i ++) {
    id.mA[i] = -0.001;
    id.mB[i] = 0.001;
  }
  id.mA[0] = .999;
  id.mA[0] = 1.001;
  */
  
  for(int i = 0; i < n; i ++) {
    if(i%2) {
      id.mA[i] = -1.;
      id.mB[i] = 0.;
    } else {
      id.mA[i] = 0.;
      id.mB[i] = 1.;
    }
  }    
}

void initDNAcode(int n, char* dna)
{
  for(int i = 0; i < n; i ++) {
    dna[i] = 'g';
    //dna[i] = (i % 2) ? 'a' : 'c';
    
    /*
    int k = (((double)rand()) / (double)RAND_MAX) * 4;
    switch(k){
      case 0:
        dna[i] = 'a'; 
        break;
      case 1:
        dna[i] = 'c';
        break;
      case 2:
        dna[i] = 't';
        break;
      case 3:
        dna[i] = 'g';
    }
    */
  }
  dna[n] = 0;
}

main(int argc, char* argv[])
{
  typedef GOFactory < double > Fact;
  typedef WTraverse < Fact > Traverse;
  typedef BNBMsHeurSolver < Traverse > Solver;
  RNABase<double> rnabase;
  RNA rna;
  GOInitialData<double> id;
  Fact fact;
  SteepDesc<double> sd;
  FletchSearch<double> ls;
  int n, nis, nls, nbhs;
  double ampl, eps = 0.001;
  double hop = 0.1;
  double T;
  char* dnacode;
  char* ssfname = NULL;
  
  n = 50;
  nis = 10000; 
  nls = 1;
  nbhs = 100;
  ampl = 0.1;
  
  if(argc > 1)
    n = atoi(argv[1]);
  if(argc > 2)
    ssfname = argv[2];    
  
  
  //ampl = 2. / pow(nis, 1./(double) n);
  printf("ampl set to %lf\n", ampl);
  dnacode = (char*) malloc(n + 1);
  initDNAcode(n, dnacode);
  printf("%s\n", dnacode);
  rna.setRadius(1.);
  //RnaGenerator::setScale(1000.);
  RnaGenerator::generate(dnacode, n, rnabase);
  //RnaGenerator::generate("gggggggggg", n, rnabase);
  //initSultan2(n,rna);
  rna.setRNABase(&rnabase);
  initID(n, rna, id);
  printf("Source data:\n %s\n", rna.toString().c_str());
  fact.setInitialData(&id);
  ls.setEps(0.5 * eps);
  ls.setRightEnd(32.);
  ls.setRho(0.05);
  ls.setSigma(0.3);
  sd.setLineSearch(&ls);
  sd.setObjective(&rna);
  sd.setMaxIters(100000);
  sd.setEps(eps);
  
  Basinhop<double> bh;
  bh.setLocalOptimizer(&sd);
  bh.setNumberOfStarts(nbhs);

  
  AbstractCommunicator acomm(&argc, &argv);  
  Solver solver (&acomm, &fact, 0);
  solver.setNumberOfInitialSolutions(nis);
  SphereSolutionGenerator<double> sphere;
  OrtSolutionGenerator<double> ort;
  sphere.setInitialData(&id);
  ort.setInitialData(&id);
  ChainSolutionGenerator< GOFactory <double> > chain;
  chain.add(&ort);
  chain.add(&sphere);  
  solver.setSolutionGenerator(&chain);
  BNBBasicHeurMaster < Fact > hm;
  hm.setSendToAll(false);
  hm.setUpdateMode(BNBBasicHeurMaster < Fact >::UpdateModes::DO_UPDATE_FROM_MASTER);
  hm.setSolutionSurfaceUpdateMode(BNBBasicHeurMaster < Fact >::UpdateModes::DO_UPDATE_FROM_HEUR);
  GOSolutionSurface < double > gos;
  DistDivers < double > div;
  div.setEps(1.);
  gos.setDivers(&div);
  gos.setObjective(&rna);
  //gos.setOptions(GOSolutionSurface<double>::Options::EVAL_EIGEN | GOSolutionSurface<double>::Options::ONLY_MINIMA);
  hm.setSolutionSurface(&gos);
  ContHeur < Fact > hs;
  hs.setInitialData(&id);
  hs.setAmplitude(ampl);
  hs.setLocalOptimizer(&sd);
  //fact.setLocalOptimizer(&sd);
  //hs.setLocalOptimizer(&bh);
  hs.setNumberOfStarts(nls);  
  solver.setHeurSlave(&hs);
  solver.setHeurMaster(&hm);
  T = acomm.time();
  solver.solve ();
  acomm.barrier();
  if(acomm.getMyId() == 0) {
    T = acomm.time() - T;
    printf("OVERALL TIME = %lf\n", T);
    printf ("Function minimum is %lf\n", solver.getRecord());    
    SolutionContainer < Fact > * sc = solver.getSolutionContainer();
    BNB_ASSERT(sc);
    if(!sc->empty())
      printf ("Best solution: %s\n", sc->top().toString().c_str());
    else
      printf("The container is empty!\n");
    SolutionContainer < Fact >::Solution s = sc->top();
    int pos;
    double v = VecUtils::maxAbs(n, (double*)s.mX, &pos);
    printf("Maximum %lf at position %d\n", v, pos);
  }
  if(acomm.getMyId() == 1) {
    class MyPrinter : public GOSolutionSurface < double >::SolutionRecordPrinter {
      std::string print(const GOSolutionSurface < double >::SolutionRecord& rec){
        std::ostringstream os;
        os << GOSolutionSurface < double >::SolutionRecordPrinter::print(rec);
        int pos;
        double v = VecUtils::maxAbs(rec.getSolution().mX.size(), (double*)rec.getSolution().mX, &pos);
        os << "<" << BNB_MAXIMUM_COMP_TAG << ">\n";
        os << "<" << BNB_MAXIMUM_COMP_VALUE_TAG << ">";
        os << v;
        os << "</" << BNB_MAXIMUM_COMP_VALUE_TAG << ">";        
        os << "<" << BNB_MAXIMUM_COMP_POSITION_TAG << ">";
        os << pos + 1;
        os << "</" << BNB_MAXIMUM_COMP_POSITION_TAG << ">\n";        
        os << "</" << BNB_MAXIMUM_COMP_TAG << ">\n";        
        return os.str();
      }
    } prn;    
    gos.setSolutionRecordPrinter(&prn);
    std::string ssurf = gos.toString();    
    FILE *f;
    if(ssfname != NULL) {
      f = fopen(ssfname, "w");
    } else
      f = stdout;
    fprintf(f, "SOLUTION SURFACE:\n%s\n", ssurf.c_str());
    if(ssfname != NULL) {
      fclose(f);
    }
  }

  free(dnacode);
}
