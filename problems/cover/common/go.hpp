#ifndef _GO_HPP_
#define _GO_HPP_
/**
 *
 * Classes for global optimization.
 *
 * @author Mikhail Posypkin posypkin@isa.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file go.hpp
 */


#include <math.h>
#include <vector>
#include <kernel/bnblib/bnb.hpp>
#include <util/common/util.hpp>
#include <util/common/vec.hpp>
#include <util/common/smartptr.hpp>
#include <problems/optlib/objective.hpp>
#include <util/common/bnblog.hpp>
#include <problems/optlib/locoptim.hpp>
#include <util/xml/xmlpars.hpp>
//#include <ei/external/bnbcmdtags.hpp>
#include <util/common/fixedvector.hpp>
#include "util/box/box.hpp"
#include "util/box/boxutils.hpp"
#include "util/common/vecseg.hpp"


/**
 * Upper limit for number of function variables
 */
#define MAX_NUMBER_OF_VARS 4096

/**
 * Maximal number of discarders
 */
#define MAX_DISCARDERS 64

/**
 * Maximal number of children for BNB decomposition
 */
#define MAX_BNB_CHILDREN 16



/**
 * Initial data for Global Optimization
 *
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 */
template <class FT> struct GOInitialData : public BNBProblemData{


  GOInitialData()
  {
    mObj = NULL;
    mValueEps = -1;
    mSolutionEps = -1;    
    mA = NULL;
    mB = NULL;
    mType = UNCONSTRAINED;
  }

  /**
   * The constructor
   *
   * @param f function to optimize
   *
   * @param dim dimension
   *
   * @param a "left" bound for the rectangle
   *
   * @param b "right" bound for the rectangle
   *
   * @param eps precision
   *
   * @param lipz lipzician constant
   */
  GOInitialData(FT * a, FT * b, FT valeps, FT soleps, Objective < FT > *obj)
  {
    BNB_ASSERT(obj->getDim() <= MAX_NUMBER_OF_VARS);
    mObj = obj;
    mValueEps = valeps;
    mSolutionEps = soleps;    
    mA = a;
    mB = b;
    mType = UNCONSTRAINED;
  }

  /**
   * Problem types:
   */

  /**
   * Unconstrained optimization
   */
  static const int UNCONSTRAINED = 1;

  /**
   * Box-constrained optimization
   */
  static const int BOXCONSTRAINED = 2;

  /**
   * General constrained optimization
   */
  static const int GENERCONSTRAINED = 3;

  /**
   * Objective
   */
  Objective < FT > *mObj;

  /**
   * Precision for value
   */
  FT mValueEps;

  /**
   * Precision for solution
   */
  FT mSolutionEps;

  /**
   * "Left" bound for the recvangle
   */
  FT *mA;

  /**
   * "Right" bound for the rectangle
   */
  FT *mB;

  /**
   * Problem type
   */
  int mType;
};


/**
 * Factory class for global optimization.
 *
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 */

template <class FT>  class GOFactory {
  public:
    struct Options {
      static const unsigned int NO_OPTION = 0;
      static const unsigned int DO_LOCAL_SEARCH = 1;
      static const unsigned int STD_SOLTUION_GENERATION = 1 << 1;
    };

    struct DebugOptions {
      static const unsigned int NO_DEBUG = 0;
      static const unsigned int DO_DEBUG = 1;
    };

    /**
     * MANDATORY : value type
     */
    typedef FT ValueType;


    /**
     * Retrieve problem type
     *
     * @return problem type
     *
     */
    static int getProblemType() {
      return BNB_MINIMIZE;
    }

    /**
     * Class for holding optimization rectangles
     *
     * @author Mikhail Posypkin posypkin@ispras.ru
     *
     * @warning Using the code below requires an explicit permition from the author
     * @warning Any other use is prohibited
     *
     * @date Copyright by Mikhail Posypkin 2005-2015
     */
    struct Set : public Box<FT>
    {

      /**
       * The constructor
       *
       */
      Set(int dim = 1) : Box<FT>(dim)
      {
	//mDim = dim;
	mDiscardRho = 0.;
	mEstim = getMinValue<ValueType>();
      }

      /**
       * MANDATORY, needed for priority in a queue
       *
       * @param s other number
       *
       * @return true if current number is less then another
       *
       */
      bool operator < (const Set & s) const
      {
	return mEstim > s.mEstim;
      }



      /**
       * Evaluate box diameter
       * @return box diameter
       */
      ValueType diameter () const
      {
	ValueType vt;

	vt = 0.;
	for(int i = 0; i < Box<FT>::mDim; i ++) {
	  vt = BNBMAX(Box<FT>::mB[i] - Box<FT>::mA[i], vt);
	}
	return vt;
      }

      /**
       * MANDATORY, serialize the set to a string.
       *
       * @return string
       *
       */
      std::string toString ()const
      {
	std::ostringstream ostr;

	for(int i = 0; i < Box<FT>::mDim; i ++) 
	  ostr << "(" << Box<FT>::mA[i] << "," << Box<FT>::mB[i] << ")";

	ostr << /*" F = " << mFval <<*/ ", Estim = " << mEstim ;
	return ostr.str ();
      }

      /**
       * Function value in the center of the rectangle
       *
       ValueType mFval;
       */
      /**
       * Estimation value (possible least value in the rectangle)
       */
      ValueType mEstim;

      /**
       * Split dimension
       */
      int mSplitI;

#if 0
      /**
       * Dimension
       */
      int mDim;
#endif

      /**
       * A ball to discard radius
       */
      ValueType mDiscardRho;

      /**
	* Segments to extract
	*/
      VecSeg<MAX_BNB_CHILDREN, ValueType> mSegments;


#if 0
      /**
       * "Left" rectnagle bounds
       */
      SmartArrayPtr<ValueType> mA;

      /**
       * "Right" rectnagle bounds
       */
      SmartArrayPtr<ValueType> mB;
#endif
    };

    /**
     * Class for holding optimization solutions
     *
     * @author Mikhail Posypkin posypkin@ispras.ru
     *
     * @warning Using the code below requires an explicit permition from the author
     * @warning Any other use is prohibited
     *
     * @date Copyright by Mikhail Posypkin 2005-2015
     */
    struct Solution
    {

      /**
       * The constructor
       *
       */
      Solution(int dim = 1) : mX(dim)
      {
	mDim = dim;
	mFval = getMaxValue<ValueType>();
      }

      /**
       * MANDATORY, needed for priority in a queue
       *
       * @param s other number
       *
       * @return true if current number is less then another
       *
       */
      bool operator < (const Solution & s) const
      {
	return mFval > s.mFval;
      }


      /**
       * MANDATORY, retrieve the value of the number
       *
       * @return value of the number
       *
       */
      ValueType getValue () const
      {
	return mFval;
      }



      /**
       * MANDATORY, serialize the solution to a string.
       *
       * @return string
       *
       */
      std::string toString ()const
      {
          BNB_ERROR_REPORT("Feature not implemented\n");
          return "";
      }

      /**
       * Function value in the middle
       */
      ValueType mFval;

      /**
       * Dimension
       */
      int mDim;

      /**
       * Point
       */
      SmartArrayPtr<ValueType> mX;

    };

    /**
     * Base class for Discarders
     */
    class Discarder {
      public:


	/**
	  * Set discarder's name
	  * @param name discarder's name
	  */
	void setName(std::string name)
	{
	  mName = name;
	}

	/**
	  * Retrieve discarder name 
	  * @return discarder name
	  */
	virtual std::string getName() const 
	{
	  return mName;
	}

	/**
	 * Checks whether the set can be discarded
	 * @param s set to process
	 * @param sv vector of resulting sets to porcess
	 * @param solv vector of resulting solutions
	 * @param record incumbent value
	 * @param  info branch info
	 * @return 
	 */
	virtual void discard(Set& s, 
	    GVector< Set > &sv, 
	    GVector< Solution >& solv, 
	    ValueType record, 
	    BNBBranchInfo* info) = 0;

      private:

	std::string mName;

    };

    /**
     * Base class for making solution
     */
    class SolutionMaker {
      public:

	/**
	 * Makes a solution from a set
	 * @param s source set
	 * @param sol resulting solution
	 */
	virtual bool make(const Set& s, Solution& sol) = 0;
    };



    /**
     * Base class for constraints cheking
     */
    class ConstraintChecker {
      public:

	/**
	 * Checks constraints for a given point
	 * @param  x point 
	 * @return true if constraints are satisfied, false otherwise
	 */
	virtual bool check(FT* x) = 0;

	/**
	  * Prints constraint values for a given point
	  * @param x point 
	  */
	virtual void print(FT* x) = 0;
    };

    /**
     * The constructor
     * 
     * dim 
     */
    GOFactory() : mDiscarders(mADiscarders, MAX_DISCARDERS)
  {
    mReduceFactor = 1.;
    mLocalSearchCounter = 0;
    mLocalSearchFactor = 1;
    mOptions = Options::NO_OPTION  | Options::STD_SOLTUION_GENERATION;
    mDebugOptions = DebugOptions::NO_DEBUG;
    mBarrier = 400.;
    mLocOpt = NULL;
    mSolutionMaker  = NULL;
    mConsCheck = NULL;
    mPrev = getMaxValue<ValueType>();
  }

    virtual void setInitialData(GOInitialData<FT>* id)
    {
      mId = id;
    }

    /**
     * MANDATORY, upper bound for the function values.
     *
     * return lowest bound
     */
    static ValueType absoluteBound ()
    {
      return getMaxValue<ValueType>();
    }

    /**
     * MANDATORY, fills in initial set - search tree root.
     *
     * @param s set to fill
     *
     * @return true if the set exists (true always in our case)
     *
     */
    bool getInitialSet (Set & s)
    {
      Set ns (mId->mObj->getDim());
      for(int i = 0; i < mId->mObj->getDim(); i ++) {
	ns.mA[i] = mId->mA[i];
	ns.mB[i] = mId->mB[i];
      }
      //eval(ns, absoluteBound());
      //ns.mEstim = getMinValue<ValueType>();
      assignSplitItem(ns);
      s = ns;
      return true;
    }


    /**
     * MANDATORY, fills in initial solution (heuristic).
     *
     * @param s set to fill in
     *
     * @return true if initial solution is found
     *
     */
    bool getInitialSolution (Solution & s)
    {
      BNB_ASSERT(mId);
      Solution sol (mId->mObj->getDim());
      bool rv = true;
      for(int i = 0; i < mId->mObj->getDim(); i ++) {
	FT alph;
	alph = (FT) rand() / (FT) RAND_MAX;
	sol.mX[i] = mId->mA[i] + alph * (mId->mB[i] - mId->mA[i]);
      }
      if((mConsCheck != NULL) && mConsCheck->check(sol.mX)){      
	s = sol;
      } else
	rv = false;

      if(mOptions & Options::DO_LOCAL_SEARCH) {
	if(mLocOpt) {
	  mLocOpt->search(s.mX, &(s.mFval));
	} else {
	  //s.mFval = mId->mObj->func(sol.mX);
            BNB_ERROR_REPORT("Local search not set!\n");
	}
      } else {
	s.mFval = mId->mObj->func(sol.mX);
      }
      return rv;
    }


    /**
     * MANDATORY, decompose rectangle
     *
     * @param s set, representing rectangle to decompose
     *
     * @param sv resulting two rectangles
     * 
     * @param record the incumbent value
     *
     */
    virtual void  branch (Set & s, GVector < Set > & sv, GVector < Solution > & solv, ValueType record, BNBBranchInfo * info, InfiniInt ql)
    {
      bool rv;
      Set asetv[MAX_BNB_CHILDREN];
      FixedVector < Set > setv(asetv, MAX_BNB_CHILDREN);
      rv = false;
      sv.push_back(s);
      if(!mDiscarders.empty()) {
	int sz = mDiscarders.size();
	for(int i = 0; i < sz; i++){
	  if(sv.empty())
	    break;
	  if(mDebugOptions & DebugOptions::DO_DEBUG) {
	    mLog << "Invoking discarder \"" << mDiscarders[i]->getName();
	    mLog << "\" on " << sv.size() << " sets\n";
	    mLog.write();
	  }
	  while(!sv.empty()) {
	    Set s = sv.back();
	    sv.pop_back();
	    mDiscarders[i]->discard(s, setv, solv, record, info);
	  }
	  int szv = setv.size();
	  for(int j = 0; j < szv; j ++) {
	    sv.push_back(setv[j]);
	  }
	  setv.clear();
	  //sv.insert(sv.end(), setv.begin(), setv.end());
	  if(mDebugOptions & DebugOptions::DO_DEBUG) {
	    mLog << "After invoking discarder \"" << mDiscarders[i]->getName();
	    mLog << "\" we have " << sv.size() << " sets\n";
	    mLog.write();
	  }
	}
      }
      //TODO make selection of sets to produce solutions
      int sz = sv.size();
      for(int i = 0; i < sz; i ++) {
	int n = mId->mObj->getDim();
	Solution sol(n);
	if(makeSolution(sv[i], sol)) 
	  solv.push_back(sol);
      }
      if(mOptions & Options::DO_LOCAL_SEARCH) {
	int sz = solv.size();
	for(int i = 0; i <  sz; i++){
	  improveSolution(solv[i]);
	}
      }
    }

    /**
     * Checks whether the set can be discarded by incumbent value (record)
     *
     * @param s set
     *
     * @param record incumbent
     *
     * @return true if the set can be discarded, false otherwise
     *
     */
    bool discard(Set & s, ValueType record) const
    {
      return s.mEstim >= record; 
    }

    /**
     * Checks whether the solution can be discarded by incumbent value (record)
     *
     * @param s set
     *
     * @param record incumbent
     *
     * @return true if the set can be discarded, false otherwise
     *
     */
    bool discard(Solution & s, ValueType record)
    {
      return s.mFval > record; 
    }

    /**
     * Retrieve initial data references
     *
     * @return initial data reference
     *
     */
    GOInitialData < FT >*  getInitialData()
    {
      return mId;
    }

    /**
     * MANDATORY, used for communicating with the parallel solver from the outside.
     *
     * @param str string
     *
     * @param sv candidate problems (sets) list
     *
     *
     */
    int constructFromString(std::string str, GVector <Set> & sv)
    {
      return 1;
    }

    /**
     * Constructs string from sets
     *
     * @param ss string
     *
     * @param sv vector of sets 
     *
     */
    void serializeToString(std::string & ss, GVector <Set> & sv)
    {
    }

    Solution getSolutionFromString(std::string& s)
    {
      class SP : public XMLListener {
	public:
	  SP(Solution* sol)
	  {
	    mSol = sol;
	  }

	  void fireTag(int level, const std::string& tag, const std::string& s, int begin, int end)
	  {
	    std::string ss(s, begin, end - begin + 1);
	    std::istringstream iss(ss);
	    if(tag == "x"){
	      int n;
	      n = mSol->mDim;
	      for(int i = 0; i < n; i ++) {
		if(!iss.good()) {
		  fprintf(stderr, "at %d\n", i);
		  BNB_ERROR_REPORT("Parsing X-vector for a Solution failed\n");
		}
		iss >> mSol->mX[i];
		fprintf(stderr, "%d: %lf\n", i, mSol->mX[i]);
	      }
	    } else if(tag == "f") {
	      iss >> mSol->mFval;
	      if(iss.bad())
		BNB_ERROR_REPORT("Parsing func value for a Solution failed\n");
	    } 
	  }
	private:
	  Solution* mSol;
      };

      Solution sol(mId->mObj->getDim());
      SP spl(&sol);
      XMLPars pars;
      pars.addListener(&spl);
      pars.parse(s);
      return sol;
    }

    /**
     * Obtain solution from the (center of) rectangle
     *
     * @param s set (rectangle)
     * @param sol solution
     *
     * @return true if solution is constructed
     *
     */
    bool makeSolution(const Set & s, Solution & sol)
    {
      int n = mId->mObj->getDim();  
      bool rv;
      if(mSolutionMaker) {
	rv = mSolutionMaker->make(s, sol);
      } else {
	for(int i = 0; i < n; i ++) {
	  sol.mX[i] = 0.5 * (s.mA[i] + s.mB[i]);
	}
      }
      if(mConsCheck != NULL){
	if(mConsCheck->check(sol.mX)){
	  sol.mFval = mId->mObj->func(sol.mX);
	  if(mDebugOptions & DebugOptions::DO_DEBUG){
	    VecUtils::vecPrint(n, (FT*)sol.mX);
	    mConsCheck->print(sol.mX);
	  }
	  rv = true;
	} else {
	  rv = false;
	}
      } else {
	sol.mFval = mId->mObj->func(sol.mX);
	rv = true;      
      }
      return rv;
    }


    void improveSolution(Solution& sol)
    {
      FT v;
      BNB_ASSERT(mLocOpt);
      if(mLocOpt->search(sol.mX, &v)) {
	sol.mFval = v;
      } else {
	/*        mLog << "Local Search failed\n";
		  mLog.write();*/
      }
    }

    /**
     * Caclulates function value and estimation.
     *
     * @param s set to evaluate
     * @param record record
     * @param eps tolerance
     */
    /*
       virtual  void eval(Set &s, ValueType record)
       {
       }
     */


    /**
     * Retrieve options
     * @return reference to options 
     */
    unsigned int & getOptions()
    {
      return mOptions;
    }

    /**
      * Retrieve debug options
      * @return reference to debug options
      */
    unsigned int & getDebugOptions()
    {
      return mDebugOptions;
    }


    /**
     * Setup loca search factor
     * @param lsf factor
     */
    void setLocalSearchFactor(int lsf)
    {
      mLocalSearchFactor = lsf;
    }

    /**
     * Setup local optimizer
     * @param opt local optimizer
     */
    void setLocalOptimizer(LocalOptimizer<FT> *opt)
    {
      mLocOpt = opt;
    }

    /**
     * Set up solution maker
     * @param solm solution maker
     */
    void setSolutionMaker(SolutionMaker* solm) 
    {
      mSolutionMaker = solm;    
    }

    /**
     * Set up constraint checker
     * @param cc constraint checker
     */
    void setConstraintChecker(ConstraintChecker* cc)
    {
      mConsCheck = cc;
    }

    /**
     * Adds a new discarder
     * @param dsc discarder
     */
    void addDiscarder(Discarder* dsc)
    {
      mDiscarders.push_back(dsc);
    }

    /**
      * Assigns a split item (varaible) for a newly created set
      * @param s set to process
      */
    void assignSplitItem(Set& s) const
    {
      s.mSplitI = BoxUtils::getLongestEdge(s);
    }

    //protected:


    /**
     * Initial data
     */
    GOInitialData <FT> * mId;

    unsigned int mOptions;  
    unsigned int mDebugOptions;  

    FT mReduceFactor;
    int mLocalSearchFactor;  
    int mLocalSearchCounter;
    FT mBarrier;
    FT mPrev;
    LocalOptimizer<FT> *mLocOpt;
    BNBLog<BNB_DO_LOGGING> mLog;
    FixedVector <Discarder*> mDiscarders;
    Discarder* mADiscarders[MAX_DISCARDERS];
    SolutionMaker* mSolutionMaker;
    ConstraintChecker* mConsCheck;
};


#endif
