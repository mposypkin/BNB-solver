#ifndef __LIPZGDISCARDER_HPP__
#define __LIPZGDISCARDER_HPP__

#include <math.h>
#include "go.hpp"
#include "lipzobjective.hpp"
#include <util/common/gvector.hpp>
#include <util/box/boxutils.hpp>
/**
 * A discarder based on a Lipzic formula for a gradient
 */
template <typename FT> class LipzGradDiscarder : public GOFactory<FT>::Discarder 
{
  
  public:
    struct Options {
      const static unsigned int NO_OPTION = 0;
      /**
	* Check lower bound
	*/
      const static unsigned int CHECK_LB = 1;
      /**
	* Check gradient against zero
	*/
      const static unsigned int CHECK_GRAD = 1 << 1;
      /**
	* Check gradient component against zero
	*/
      const static unsigned int CHECK_GRAD_COMP = 1 << 2;
      /**
	* Check convexity 
	*/
      const static unsigned int CHECK_CONV = 1 << 3;
      /**
	* Update improved record (incumbent), normally turn off for constraints
	*/
      const static unsigned int UPDATE_RECORD = 1 << 4;
      /**
	* Evaluate unconstrained (special) bound
	*/
      const static unsigned int EVAL_UNCONS_BOUND = 1 << 5;
      /**
	* Use eigenvalues (instead of Lipschitz constant)
	*/
      const static unsigned int USE_EIGEN_VALUES = 1 << 6;
      /**
	* Perform primary ball cut
	*/
      const static unsigned int DO_PRIMARY_BALL_CUT = 1 << 7;
      /**
	* Perform unconstrained ball cut
	*/
      const static unsigned int DO_UNCONS_BALL_CUT = 1 << 8;
      /**
	* Perform gradien-based ball cut
	*/
      const static unsigned int DO_GRAD_BALL_CUT = 1 << 9;
    };

    struct DebugOptions {

      /**
	* Performs no debugging
	*/
      const static unsigned int NO_DEBUG = 0;

      /**
	* Prints bound values
	*/
      const static unsigned int PRINT_BOUNDS = 1 << 1;

      /**
	* Prints discarded balls
	*/
      const static unsigned int PRINT_BALLS = 1 << 2;
      
      /**
	* Alert when convexity test is fulfilled
	*/
      const static unsigned int PRINT_CONV = 1 << 3;
    };


    /**
      * Sets discarder unique name
      * @param name name to set
      */
    void setName(std::string name) 
    {
      mName = name;
      mLog.setName(name);
    }

    /**
      * Retrieve discarder name
      * @return discarder name
      */
    std::string getName() const 
    {
      return mName;
    }
    
    LipzGradDiscarder()
    {
      mObj = NULL;
      mId = NULL;
      mOptions = 
          Options::CHECK_LB |
          Options::UPDATE_RECORD | 
	  Options::EVAL_UNCONS_BOUND |
	  Options::USE_EIGEN_VALUES;
      mDebugOptions = DebugOptions::NO_DEBUG;
      mTol = 0.0001;
      mName = "LipzGradDiscarder";
    }
    
    /**
      * Set Tolerance for ball discarding
      * @param tol tolerance to set
      */
    void setBallTolerance(FT tol)
    {
      mTol = tol;
    }
    
   /**
    * Retrieve options reference
    * @return options reference
    */
    unsigned int& getOptions()
    {
      return mOptions;
    }    

    /**
      * Retrieve debug options reference
      * @return debug options ref
      */
    unsigned int& getDebugOptions() 
    {
      return mDebugOptions;
    }

    /**
     * Setup objective pointer
     * @param obj objective
     */
    void setObjective(LipzObjective<FT>* obj)
    {
      mObj = obj;
    }
    
    /**
     * Setup precision
     * @param eps epsilon
     */
    void setEps(FT eps)
    {
      mEps = eps;
    }
    
    /**
     * Setup initial data pointer
     * @param id id pointer
     */
    void setInitialData(GOInitialData<FT>* id)
    {
      mId = id;
    }
    
    void discard(typename GOFactory<FT>::Set& s, GVector< typename GOFactory<FT>::Set >& sv, GVector< typename GOFactory<FT>::Solution >& solv, typename GOFactory<FT>::ValueType record, BNBBranchInfo* info)
    {
      int n;
      bool rv = false;
      bool bound = false;
      bool havegrad = false;
      FT l, emin, emax, v, r, normg;
      FT ac[MAX_NUMBER_OF_VARS];
      FT ag[MAX_NUMBER_OF_VARS];
      
      BNB_ASSERT(mObj);
      r = 0;
      n =  mObj->getDim();
      
      evalThings(s, ac, ag, v, r, false);
      if((!isConstraint()) && 
         (Options::CHECK_LB & mOptions) && 
         (Options::UPDATE_RECORD & mOptions) && 
	 (v < record)) {        
	typename GOFactory<FT>::Solution sol(n);
	for(int i = 0; i < n; i ++) {
	  sol.mX[i] = ac[i];
	}
	sol.mFval = v;
	solv.push_back(sol);    
	record = v;    
      }
      getEigenBounds(s, emin, emax);
      l = BNBMAX(BNBABS(emin), BNBABS(emax));
      BNB_ASSERT(mId);
      if(mId->mType == GOInitialData<FT>::UNCONSTRAINED){
	bound = false;
      } else if(mId->mType == GOInitialData<FT>::BOXCONSTRAINED){
	bool left, right;     
	for(int i = 0; i < n; i ++) {
	  left = false;
	  right = false;
	  if(s.mA[i] == mId->mA[i]) 
	    left = true;
	  if(s.mB[i] == mId->mB[i])
	    right = true;
	  bound = (bound || left || right);
	}
      } else {
	bound =true;
      }

      if((!isConstraint()) && 
	  ((Options::CHECK_GRAD & mOptions) 
	   || (Options::CHECK_GRAD_COMP & mOptions))) {
	havegrad = true;
	mObj->grad(ac, ag);
	rv = rv || discardByGrad(s, r, l, n, ac, ag, v, bound);
      }

      if((!rv) && (!isConstraint()) && 
	  (Options::CHECK_CONV & mOptions)) {
	if(emax < 0) {
	  if(mDebugOptions & DebugOptions::PRINT_CONV) {
	    mLog << "Concave region detected\n";
	    mLog.write();
	  }
	  rv = true;
	}
      }
      if((!rv) && (Options::CHECK_LB & mOptions)) {
	if(!havegrad) {
	  havegrad = true;
	  mObj->grad(ac, ag);
	}
	rv = rv || discardByLB(s, n, r, emin, emax, v, record, ac, ag, bound);
      }
      if(!rv)
	sv.push_back(s);  
      else 
	info->mDiscardedByRecord ++;
    }

  private:

    /**
      * Pruning based on gradient information
      * @param s box to process
      * @param r box radius
      * @param l lipschitz constant for gradient
      * @param n dimension
      * @param ac box center
      * @param ag gradient
      * @param bound false if the problem unconstrained in th egiven box
      * @return true if box should be discarded false otherwise
      */
    bool discardByGrad(typename GOFactory<FT>::Set& s, FT r, FT l, int n, FT* ac, FT* ag, FT& v, bool bound) const
    {
      bool rv = false;
      FT rd = 0.;
      for(int i = 0; i < n; i ++) {
	bool left, right;     
	left = false;
	right = false;
	if(mId->mType == GOInitialData<FT>::BOXCONSTRAINED) {
	  if(s.mA[i] == mId->mA[i]) 
	    left = true;
	  if(s.mB[i] == mId->mB[i])
	    right = true;
	}
	if(Options::CHECK_GRAD_COMP & mOptions) {
	  FT li, gi;
	  li = getLipzGrad(s, i);
	  gi = ag[i];         
	  if(BNBABS(gi) > (li * r + mTol)) {
	    if(gi > 0) {
	      if(left) {
		s.mB[i] = s.mA[i];
		evalThings(s, ac, ag, v, r);
	      } else {
		rv = true;
		break;
	      }
	    } else {
	      if(right) {
		s.mA[i] = s.mB[i];
		evalThings(s, ac, ag, v, r);
	      }
	      else{
		rv = true;
		break;
	      }
	    }
	  } else if(!((left && (gi > 0)) || (right && (gi < 0)))) {
	    if(li != 0) {
	      FT GI = BNBABS(gi) - mTol;
	      if(GI > 0) {
		rd = BNBMAX(rd, GI / li);
	      }
	    }
	  }
	}
      }
      if((Options::CHECK_GRAD & mOptions) && (!bound)) {
	FT  normg = VecUtils::vecNormTwo(n, ag);
	if(normg > (l * r + mTol)) {
	  rv = true;
	}
	rd = BNBMAX(rd, (normg - mTol)/ l);
      } 
      if(Options::DO_GRAD_BALL_CUT & mOptions) {
	Segment<FT> seg;
	if(BoxUtils::cutL2(s.mSplitI, s, ac, rd * rd, seg)) {
	  s.mSegments.push(seg);
	}
      }
      return rv;
    }

    /**
      * Discard based on comparison of lower and upper bounds
      * @param s box to process
      * @param n dimension
      * @param r box radius
      * @param emin lower Eigen value bound
      * @param emax upper Eigen value bound
      * @param v objective value in the center of the box
      * @param record best value found so far
      * @param ac box center
      * @param ag gradient in the box center (if needed)
      * @param bound false if the problem unconstrained in th egiven box
      * @return true if box is a subject to discard
      */
    bool discardByLB(typename GOFactory<FT>::Set& s, int n, FT r, FT emin, FT emax, FT v, FT record, FT* ac, FT* ag, bool bound)
    {
      FT estim = getMinValue<FT>();
      FT estimun = getMinValue<FT>();
      bool rv = false;
      FT rdq = 0.;
      FT l;
      FT normq = 0.;
      FT nc[MAX_NUMBER_OF_VARS];

      getLowerBounds(s, n, r, emin, emax, v, ac, ag, bound, estim, estimun);

      l = BNBABS(emin);
      if(mDebugOptions & DebugOptions::PRINT_BOUNDS) {
	mLog << "Box " << BoxUtils::toString(s) << " ";
	mLog << "Lower bound " << estim << "\n";
	mLog << "Uncons Lower bound " << estimun << "\n";
	mLog.write();
      }

      if(Options::DO_PRIMARY_BALL_CUT & mOptions) {
	normq = VecUtils::vecNormTwoSqr(n, ag);
      }
      if(isConstraint()) {
	rv = rv || (estim > 0.0);
      } else {
	s.mEstim = BNBMAX(estim, estimun);
	rv = rv || (s.mEstim > record - mEps);
      }
      if((!rv) && (Options::DO_PRIMARY_BALL_CUT & mOptions)) {
	FT l, u;
	if(emin >= 0) 
	  l = mMinLipz;
	else
	  l = BNBABS(emin);
	if(isConstraint()) 
	  u = v;
	else 
	  u = v - record + mEps;
	rdq = (2./l) * (u + normq / (2. * l));
	if(rdq > 0) {
	  for(int i = 0; i < n; i ++) {
	    nc[i] = ac[i] + ag[i] / l;
	  }
	  Segment<FT> seg;
	  if(BoxUtils::cutL2(s.mSplitI, s, nc, rdq, seg)) {
	    s.mSegments.push(seg);
#if 0

	       printf("box = %s\n", BoxUtils::toString(s).c_str());

	       printf("Primary Ball:");
	       for(int j = 0; j < n; j ++) {
		 printf(" %lf", nc[j]);
	       }
	       printf("[%lf, %lf], %lf\n", seg.mA, seg.mB, s.mA[s.mSplitI]);
	       /*
	       printf(", r = %lf, L2 cut: [%lf, %lf]\n", sqrt(rdq), seg.mA, seg.mB);
	       printf("(record = %lf, v = %lf)\n", record, v);
	       */
#endif
	  }
	}
      }
      if((!rv) && (!bound) && (!isConstraint()) && (Options::DO_UNCONS_BALL_CUT & mOptions)) {
	FT u;
	BNB_ASSERT(emax > 0);
	if(isConstraint()) 
	  u = v;
	else 
	  u = v - record + mEps;
	rdq = (2. * u / emax);
	Segment<FT> seg;
	if(BoxUtils::cutL2(s.mSplitI, s, ac, rdq, seg)) {
	  s.mSegments.push(seg);
	  /*
	  printf("Uncons Ball:");
	  for(int j = 0; j < n; j ++) {
	    printf(" %lf", ac[j]);
	  }
	  printf(", r = %lf, L2 cut: [%lf, %lf]\n", sqrt(rdq), seg.mA, seg.mB);
	  */
	}
      }
      return rv;
    }

    /**
      * Calculates lower bounds
      * @param s set to process
      * @param n problem dimension
      * @param r box radius
      * @param emin minimanl eigenvalue
      * @param emax maximal eigenvalue
      * @param v function value in the center of the box
      * @param ac box center coordinates
      * @param ag gradient in the center of the box
      * @param bound true if the problem is constrained
      * @param estim general bound
      * @param estimun bound for unconstrained problems
      */
    void getLowerBounds(typename GOFactory<FT>::Set& s, int n, FT r, FT emin, FT emax, FT v, FT* ac, FT* ag, bool bound, FT& estim, FT& estimun)
    {
      FT nc[MAX_NUMBER_OF_VARS];
      if(mObj->getKind() == Objective<FT>::EQCONSTRAINT)
	BNB_ERROR_REPORT("Equality constraints not yet implemented");
      if((!(bound || isConstraint())) && (Options::EVAL_UNCONS_BOUND & mOptions)) {
	estimun = v - 0.5 * emax * r * r;
      }
      if(emin <= 0) {
	for(int i = 0; i < n; i ++) {
	  v -= 0.5 * (s.mB[i] - s.mA[i]) * BNBABS(ag[i]);
	}
	estim = v + 0.5 * emin * r * r;
      } else {
	for(int i = 0; i < n; i ++) {
	  nc[i] = ac[i] - ag[i] / emin;
	  if(nc[i] < s.mA[i])
	    nc[i] = s.mA[i];
	  else if(nc[i] > s.mB[i])
	    nc[i] = s.mB[i];
	}
	estim = v;
	for(int i = 0; i < n; i ++) {
	  FT d = nc[i] - ac[i];
	  estim += ag[i] * d;
	  estim += 0.5 * emin * d * d;
	}
      }
    }

    bool isConstraint() const
    {
      return ((mObj->getKind() == Objective<FT>::EQCONSTRAINT) ||
	 (mObj->getKind() == Objective<FT>::INEQCONSTRAINT));
    }

    void getEigenBounds(const typename GOFactory<FT>::Set& s, FT &emin, FT &emax) const 
    {
      FT lemin, lemax;
      mObj->getEigenBounds(s.mA, s.mB, lemin, lemax);
      if(mOptions & Options::USE_EIGEN_VALUES) {
	emin = lemin; 
	emax = lemax;
      } else {
	emax = BNBMAX(BNBABS(lemin), BNBABS(lemax));
	emin = -BNBMAX(BNBABS(lemin), BNBABS(lemax));
      }
      /*** TMP ****
      if(emin >= 0) {
	printf("convex:");
	printf("%s\n", s.toString().c_str());
      } else if(emax <= 0) {
	printf("concave\n");
      } else {
	printf("mixed\n");
      }
      *** TMP ****/
    }

    FT getLipzGrad(const typename GOFactory<FT>::Set& s, const int i) const 
    {
      return mObj->getGradCompLipzConstant(s.mA, s.mB, i);
    }
  
  private:

    void evalThings(const typename GOFactory<FT>::Set& s, FT* ac, FT* ag, FT& v, FT& r, bool evalgrad = true) const
    {
      int n = mObj->getDim();
      for(int i = 0; i < n; i ++) {
	ac[i] = 0.5 * (s.mA[i] + s.mB[i]);
	r += BNBSQR(0.5 * (s.mB[i] - s.mA[i]));
      }
      r = sqrt(r);
      v = mObj->func(ac);
      if(evalgrad)
	mObj->grad(ac, ag);
    }


    BNBLog<BNB_DO_LOGGING> mLog;
    FT mEps;
    FT mTol;
    LipzObjective<FT>* mObj;
    GOInitialData<FT>* mId;
    unsigned int mOptions;
    unsigned int mDebugOptions;
    std::string mName;
    static const FT mMinLipz;
};

template<typename FT>
const FT LipzGradDiscarder<FT>::mMinLipz = 0.01;

#endif
