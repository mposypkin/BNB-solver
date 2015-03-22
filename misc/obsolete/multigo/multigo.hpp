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
 * @file multigo.hpp
 *
 * Multiobjective global optimization with branch and bound method.
 */


#include <math.h>
#include <vector>
#include <kernel/bnblib/bnb.hpp>
#include <util/common/util.hpp>
#include <util/common/smartptr.hpp>
#include "multiobjective.hpp"
#include <util/common/bnblog.hpp>
#include <problems/optlib/locoptim.hpp>
#include <util/xml/xmlpars.hpp>
#include <ei/external/bnbcmdtags.hpp>
#include <util/common/fixedvector.hpp>


/**
  * Upper limit for number of parameters
  */
#define MAX_NUMBER_OF_PARAMS 4096

/**
  * Upper limit to the number of criteria
  */
#define MAX_NUMBER_OF_CRITERIA 128

/**
  * Maximal number of discarders
  */
#define MAX_DISCARDERS 16

/**
  * Maximal number of children for BNB decomposition
  */
#define MAX_BNB_CHILDREN 8



/**
 * Initial data for Multicriteria Global Optimization
 *
 * @author Mikhail Posypkin posypkin@isa.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 */
struct MGOInitialData : public BNBProblemData{

  MGOInitialData()
  {
    mObj = NULL;
    mEps = NULL;
    mA = NULL;
    mB = NULL;
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
 */
  MGOInitialData(double* a, double* b, double* eps, MultiObjective *obj)
  {
    BNB_ASSERT(obj->getNumOfParams() <= MAX_NUMBER_OF_PARAMS);
    BNB_ASSERT(obj->getNumOfCriteria() <= MAX_NUMBER_OF_CRITERIA);
    mObj = obj;
    mEps = eps;
    mA = a;
    mB = b;
  }

  
  MultiObjective *mObj;

/**
 * Precision for value
 */
  double *mEps;
  
/**
 * "Left" bound for the recvangle
 */
  double *mA;

/**
 * "Right" bound for the rectangle
 */
  double *mB;
};


/**
 * Factory class for multi-criterial global optimization.
 *
 * @author Mikhail Posypkin posypkin@isa.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 */

class MGOFactory {
  public:
    /**
     * Class for holding the point and the respective vector function value
     */
    struct Point
    {
      /**
       * Constructor
       * @param paramd number of parameters
       * @critd number of criteria
       */
      Point(int paramd, int critd)
      {
	mParamD = paramd;
	mCritD = critd;
	mParams.alloc(mParamD);
	mCriteria.alloc(mCritD);
      }

      std::string toString() const
      {
	std::ostringstream os;
	os << "<x> ";
	for(int i = 0; i < mParamD; i ++) {
	  os << mParams[i];
	  os << " ";
	}
	os << "</x>";
	os << "<f> ";
	for(int i = 0; i < mCritD; i ++) {
	  os << mCriteria[i];
	  os << " ";
	}
	os << "</f>";
	return os.str();
      }

      bool operator <= (const Point& operand) const
      {
	bool rv = true;
	for(int i = 0; i < mCritD; i ++) {
	  if(mCriteria[i] > operand.mCriteria[i]) {
	    rv = false;
	    break;
	  }
	}
	return rv;
      }

      int mParamD;
      int mCritD;
      SmartArrayPtr<double> mParams;
      SmartArrayPtr<double> mCriteria;
    };

    /**
     * MANDATORY : value type
     */
    typedef std::vector<Point> ValueType;


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
       * @author Mikhail Posypkin 
       *
       * @warning Using the code below requires an explicit permition from the author
       * @warning Any other use is prohibited
       *
       * @date Copyright by Mikhail Posypkin 2005-2015
       */
      struct Set 
      {

	Set(int dim = 1) : mA(dim), mB(dim)
	{
	  mDim = dim;
	  mDiscardRho = 0.;
	}

	bool operator < (const Set & s) const
	{
	  //return mEstim > s.mEstim;
	  return true;
	}

	std::string toString ()const
	{
	  std::ostringstream ostr;

	  for(int i = 0; i < mDim; i ++) 
	    ostr << "(" << mA[i] << "," << mB[i] << ")";

	  return ostr.str ();
	}

	ValueType mEstim;

	double mDiscardRho;

	int mDim;

	SmartArrayPtr<double> mA;

	SmartArrayPtr<double> mB;
      };



    /**
     * Structure for holding the solution
     */
    struct Solution {

      void pushPoint(const Point& point) 
      {
	bool add = true;
	if(!mPoints.empty()){
	  for(std::vector<Point>::iterator i = mPoints.begin(); i != mPoints.end();) {
	    if(point <= (*i)) {
	      i = mPoints.erase(i);
	    } else if((*i) <= point) {
	      add = false;
	      break;
	    } else
	      i ++;
	  }
	}
	if(add)
	  mPoints.push_back(point);
      }

      std::string toString() const
      {
	std::ostringstream os;
	os << "<solu>\n";
	for(std::vector<Point>::const_iterator i = mPoints.begin(); i != mPoints.end(); i ++) {
	  os << "  ";
	  os << i->toString();
	  os << "\n";
	}
	os << "</solu>";
	return os.str();
      }

      std::vector<Point> mPoints;
    };

    /**
     * Merging two solutions. The result is stored in the first solution.
     *
     * @param first first solution
     * @param second second solution
     */
    void merge(Solution& first, Solution& second)
    {
      for(std::vector<Point>::iterator i = second.mPoints.begin(); i != second.mPoints.end(); i ++) 
	first.pushPoint(*i); 
    }


    /**
     * Base class for Discarders
     */
    class Discarder {
      public:


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
	    Solution &record, 
	    BNBBranchInfo* info) = 0;

    };


    /**
     * Base class for constraints checking
     */
    class ConstraintChecker {
      public:

	/**
	 * Checks constraints for a given point
	 * @param  x point 
	 * @return true if constraints are satisfied, false otherwise
	 */
	virtual bool check(double* x) = 0;
    };

    /**
     * The constructor
     * 
     * dim 
     */
    MGOFactory() : mDiscarders(mADiscarders, MAX_DISCARDERS)
  {
    mConsCheck = NULL;
  }

    /**
      * Setup initial data 
      * @param id pointer to the Initial data structure
      */
    void setInitialData(MGOInitialData* id)
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
      BNB_ERROR_REPORT("Absolute bound for mutivalue function is not implmented\n");
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
      int n = mId->mObj->getNumOfParams();
      Set ns (n);
      for(int i = 0; i < n; i ++) {
	ns.mA[i] = mId->mA[i];
	ns.mB[i] = mId->mB[i];
      }
      //eval(ns, absoluteBound());
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
      
    }

    /**
     * MANDATORY, decompose rectangle
     *
     * @param s set, representing rectangle to decompose
     *
     * @param sv resulting two (or more) rectangles
     * 
     * @param record the incumbent value
     *
     */
    virtual void  branch (Set & s, GVector < Set > & sv, GVector < Solution > & solv, Solution & incumbent, BNBBranchInfo * info, InfiniInt ql)
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
	  while(!sv.empty()) {
	    Set s = sv.back();
	    sv.pop_back();
	    mDiscarders[i]->discard(s, setv, solv, incumbent, info);
	  }
	  int sz = setv.size();
	  for(int i = 0; i < sz; i ++) {
	    sv.push_back(setv[i]);
	  }
	  setv.clear();
	  //sv.insert(sv.end(), setv.begin(), setv.end());
	}
      }
      //TODO make selection of sets to produce solutions
      int sz = sv.size();
      for(int i = 0; i < sz; i ++) {
	Solution sol;
	if(makeSolution(sv[i], sol)) 
	  solv.push_back(sol);
      }
      /*
      if(mOptions & Options::DO_LOCAL_SEARCH) {
	int sz = solv.size();
	for(int i = 0; i <  sz; i++){
	  improveSolution(solv[i]);
	}
      }
      */
    }


    /**
     * Checks whether the set can be discarded by incumbent value (record)
     *
     * @param s set
     *
     * @param incumbent incumbent
     *
     * @return true if the set can be discarded, false otherwise
     *
     */
    bool discard(Set & s, Solution & incumbent) const
    {
      return false;
    }


    /**
     * Retrieve initial data references
     *
     * @return initial data reference
     *
     */
    MGOInitialData*  getInitialData()
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

#if 0
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
#endif

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
      int n = mId->mObj->getNumOfParams();  
      int m = mId->mObj->getNumOfCriteria();
      bool rv;
      Point pt(n, m);
      
      for(int i = 0; i < n; i ++) {
	pt.mParams[i] = 0.5 * (s.mA[i] + s.mB[i]);
      }
      if(mConsCheck != NULL){
	if(mConsCheck->check(pt.mParams)){
	  mId->mObj->func(pt.mParams, pt.mCriteria);
	  rv = true;
	} else {
	  rv = false;
	}
      } else {
	mId->mObj->func(pt.mParams, pt.mCriteria);
	rv = true;      
      }
      sol.pushPoint(pt);
      return rv;
    }


    void improveSolution(Solution& sol)
    {
      /*
      double v;
      BNB_ASSERT(mLocOpt);
      if(mLocOpt->search(sol.mX, &v)) {
	sol.mFval = v;
      } else {
      }
      */
      BNB_ERROR_REPORT("Improving solutions not yet implemeneted for multicriteria optimization\n");
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
     * Install heuristic search options
     * @param options 
     */
    unsigned int & getOptions()
    {
      return mOptions;
    }


#if 0
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
    void setLocalOptimizer(LocalOptimizer<double> *opt)
    {
      mLocOpt = opt;
    }
#endif


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

    //protected:


    MGOInitialData * mId;

    unsigned int mOptions;  

    int mLocalSearchFactor;  
    int mLocalSearchCounter;
    BNBLog<BNB_DO_LOGGING> mLog;
    FixedVector <Discarder*> mDiscarders;
    Discarder* mADiscarders[MAX_DISCARDERS];
    ConstraintChecker* mConsCheck;
};


#endif
