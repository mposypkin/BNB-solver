#ifndef __PTRAVERSE_H__
#define __PTRAVERSE_H__
/**
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file ptraverse.hpp
 */

#include <vector>
#include <string>
#include <sstream>
#include <queue>
#include <stack>
#include <pthread.h>
#include <sys/time.h>

#include <util/common/bnblog.hpp>
#include <util/common/util.hpp>
#include "bnb.hpp"
#include "singlesolucont.hpp"
#include "conchelper.hpp"
#include <util/common/simpmemmanager.hpp>
#include <util/common/gvector.hpp>
#include <util/common/stdvector.hpp>
#include <util/common/fixedvector.hpp>
#include <util/common/smartptr.hpp>



/**
 * Simple width search strategy.
 * At any step the subset with the highest estimation is selected
 * for branching.
 *
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 */

#define DEFAULT_MAX_CHUNKS 1024 * 1024
#define DEFAULT_MAX_SIZES 32




template < class ProblemFactory, 
	   class TraverseQueue = std::priority_queue <typename ProblemFactory::Set> > class PTraverse : public ConcHelper
{
public:

  typedef ProblemFactory Factory;

  typedef typename ProblemFactory::Set Set;

  typedef typename ProblemFactory::Solution Solution;

  struct LocalCounters {
    LocalCounters()
    {
      init();
    }

    void init()
    {
      mGet = 0;
      mPut = 0;
      mDonat = 0;
      mStarv = 0;
      mStarvTime = 0.;
      mDonatTime = 0.;
    }
    std::string toString()
    {
      std::ostringstream os;
      os << "Starvations " << mStarv << "(" << mStarvTime << " sec)";
      os << ", donations " << mDonat << "(" << mDonatTime << " sec)";
      os << ", got " << mGet << ", put " << mPut;
      return os.str();
    }

    unsigned int mGet;
    unsigned int mPut;
    unsigned int mDonat;
    unsigned int mStarv;
    double mStarvTime;
    double mDonatTime;
  };

 /**
  * Constructor
  *
  * @param fact set factory
  */
  PTraverse (ProblemFactory * fact) : ConcHelper()
  {
    mSetFactory = fact;
    mRecord = ProblemFactory::absoluteBound();
    mDebug = 0;
    mSolutionContainer = & mSingleSolutionContainer;
    mNumThreads = 0;
    mLocalSteps = 1024;
    mPutChunk = 1;
    mThreads = NULL;
    mInfos = NULL;    
    mLocalCounters = NULL;
    mManagers = NULL;
    mAuxMemManager = NULL;
    pthread_mutex_init(&mMutRec, NULL);
    pthread_mutex_init(&mMutTaskQueue, NULL);
    pthread_cond_init(&mCV, NULL);
    MMRegistry::concurrentInit();
    setMaxSizes();
  }

  ~PTraverse()
  {
    pthread_mutex_destroy(&mMutRec);
    pthread_mutex_destroy(&mMutTaskQueue);
    pthread_cond_destroy(&mCV);
    
    delete[] mInfos;
    delete[] mLocalCounters;
/*    for(int i = 0; i < mNumThreads; i ++) {
      delete mManagers[i];
    }
    delete mManagers;*/
    free(mThreads);
    MMRegistry::concurrentFinit();
  }    

/**
 * Return the number of candidate problems.
 *
 * @return number of candidate problems
 *
 */
  TraverseQueue& getCandidateProblemsList()
  {
    return mTaskQueue;
  }


/**
 * Gets current record value
 *
 * @return record value
 *
 */
  typename ProblemFactory::ValueType getRecord()
  {
    return mRecord;
  }

/**
 * Sets record value
 *
 * @param record new record value
 *
 */
  void setRecord(typename ProblemFactory::ValueType record)
  {
    mRecord = record;
  }
  
  /**
    * Updates (if necessary) record value
    * @param record record to update
    * @return true if record was updated
    */
  bool updateRecord(typename ProblemFactory::ValueType record)
  {
    bool rv = false;
    if(ProblemFactory::getProblemType() == BNB_MINIMIZE) {
      if(record < mRecord) {
	rv = true;
      }
    } else {
      if(record > mRecord) {
	rv = true;
      }
    }
    if(rv) {
      pthread_mutex_lock(&mMutRec);
      mRecord = record;
      pthread_mutex_unlock(&mMutRec);
    }
    return rv;
  }
  
  /**
   * Setup debug options
   * @param options 
   */
  void setDebugOptions(unsigned int options)
  {
    mDebug = options;
  }

   /**
    * Options to perform debugging
    */
  struct Options {
    static const int NO_DEBUG = 0;
    static const int PRINT_STEP = (1 << 1);
    static const int PRINT_STATE = (1 << 2);
  };

  /**
   * Get solution container
   * @param cont solution container
   */
  SolutionContainer < ProblemFactory >* getSolutionContainer()
  {
    BNB_ASSERT(mSolutionContainer);
    return mSolutionContainer;
  }
  /**
   * Setup solution container
   * @param cont solution container
   */
  void setSolutionContainer(SolutionContainer < ProblemFactory > * cont)
  {
    mSolutionContainer = cont;
  }

  /**
   * Set number of threads
   * @param nt number of threads
   */
  void setNumThreads(int nt)
  {
    ConcHelper::setNumberOfThreads(nt);
    mNumThreads = nt;
    mThreads = (pthread_t*)realloc(mThreads, nt * sizeof(pthread_t));
    mInfos =  new BNBBranchInfo[nt];    
    mLocalCounters = new LocalCounters[nt];
    mManagers = new MemManager*[nt];
  }
  
  /**
   * Setup local steps performed by each thread
   * @param ls num of loca steps
   */
  void setLocalSteps(int ls)
  {
    mLocalSteps = ls;
  }
  
  /**
   * Setup update ratio
   * @param ur update ratio
   */
  void setUpdateRatio(int ur)
  {
    mUpdateRatio = ur;
  }
  
  
  /**
   * Setup put chunk
   * @param pt put chunk
   */
  void setPutChunk(int pt)
  {
    mPutChunk = pt;
  }
  
  /**
   * Initialize memory managent
   * @param memsize  maximal size per thread
   * @param maxchunks maximal memory blocks
   * @param maxsizes expected maximum of different memory blocks
   */
  void init(int memsize, int maxchunks = DEFAULT_MAX_CHUNKS, int maxsizes = DEFAULT_MAX_SIZES)
  {
    BNB_ASSERT(mNumThreads);
    for(int i = 0; i < mNumThreads; i ++) {
      SimpMemManager* mm = new SimpMemManager(memsize, maxchunks, 8);
      mManagers[i] = mm;
    }
    mAuxMemManager = new SimpMemManager(memsize, maxchunks, 8);
  }


  /**
    * Setup maximal sizes
    * @param msetbsize maximal set buffer size
    * @param msolbsize maximal solution buffer size
    * @param mlqsize maximal local queue size
    */
  void setMaxSizes(unsigned int msetbsize = 2, unsigned int msolbsize = 2, unsigned int mlqsize = 1024)
  {
    mMaxLocalSetBufferSize = msetbsize;
    mMaxLocalSolutionBufferSize = msolbsize;
    mMaxLocalQueueSize = mlqsize;
  }

  void cmethod()      
  {
    cbranch();
  }
protected:


  
  /**
   * Push sets into the set container 
   *
   * @param sv set vector
   * @param info informational parameter 
   */
  void pushSets (GVector < Set > &sv, BNBBranchInfo * info = NULL)
  {
    while (!sv.empty ()) {
      Set s;

      s = sv.back ();
      sv.pop_back ();
      //BNB_ASSERT(!mSetFactory->isFinal(s));
      if(!mSetFactory->discard (s, getRecord())) {
	mTaskQueue.push (s);
      } else if(info) {
	info->mDiscardedByRecord ++;
      }
    }
    
    if(info)  {
      InfiniInt tmpi;
      tmpi = mTaskQueue.size();
      info->mMaxCandidateProblems = BNBMAX(info->mMaxCandidateProblems, tmpi);
    }
  }
  /**
   * Push solutions into the solution container
   * 
   * @param sv solution vector
   * @param info informational parameter 
   */
  void pushSolutions (GVector < Solution > &sv, BNBBranchInfo * info = NULL)
  {
    while (!sv.empty ()) {
      Solution s;
      s = sv.back ();
      sv.pop_back ();
      
      if(!mSetFactory->discard (s, getRecord())){
	setRecord(s.getValue());
      }
      mSolutionContainer->push(s); 
    }
  }

  
  int branch (BNBBranchInfo * info = NULL)
  {
    void* rv;
    mStarv = 0;
    mSteps = 0;
    mThreadId = 0;
    mQLen = mTaskQueue.size();
    mMaxQLen = mQLen;
    for(int i = 0; i < mNumThreads; i ++) {
      mInfos[i].mBranched = 0;
      mInfos[i].mSolutioned = 0;
      mInfos[i].mDiscardedByRecord = 0;
      mInfos[i].mDiscardedByConstraints = 0;
      mInfos[i].mMaxCandidateProblems = 0;
      mInfos[i].mEleminated = 0;
      mLocalCounters[i].init();
    }
    mLogger << mTaskQueue.size() <<" sets in task queue befor concurrent branching\n";
    mLogger.write();
    for(int i = 0; i < mNumThreads; i ++) {
      pthread_create(mThreads + i, NULL, plauncher, this);
    }    
    for(int i = 0; i < mNumThreads; i ++) {
      pthread_join(mThreads[i], &rv);      
    }
    for(int i = 0; i < mNumThreads; i ++) {
      info->mBranched = info->mBranched + mInfos[i].mBranched;
      info->mSolutioned = info->mSolutioned + mInfos[i].mSolutioned;
      info->mDiscardedByRecord = info->mDiscardedByRecord + mInfos[i].mDiscardedByRecord;
      info->mDiscardedByConstraints = info->mDiscardedByConstraints + mInfos[i].mDiscardedByConstraints;
      info->mMaxCandidateProblems = info->mMaxCandidateProblems  + mInfos[i].mMaxCandidateProblems;
      info->mEleminated = info->mEleminated + mInfos[i].mEleminated;
    }
    for(int i = 0; i < mNumThreads; i ++) {
      mLogger << "Thread " << i << " branched " << mInfos[i].mBranched.toString() << " nodes ";
      mLogger << " , counters: " <<  mLocalCounters[i].toString() << "\n";
    }
    mLogger << "Maximal task queue length: " <<  mMaxQLen << ".\n";
    mLogger.write();

    return mTaskQueue.size();
  }
  
  void cbranch ()
  {
    int tid;
    pthread_mutex_lock(&mMutRec);
    tid = mThreadId ++;
    mTids[tid] = tid;
    pthread_setspecific(mTidKey, (void*)(mTids + tid));
    MMRegistry::registerMemManager(mManagers[tid]);
    pthread_mutex_unlock(&mMutRec);
    SmartArrayPtr < Set > aq(mMaxLocalQueueSize);
    FixedVector < Set > ltq((Set*)aq, mMaxLocalQueueSize);
    Solution asolv[2];
    FixedVector < Solution > solv(asolv, 2);
    SmartArrayPtr < Set > alsetv(mMaxLocalSetBufferSize);
    FixedVector < Set > lsetv(alsetv, mMaxLocalSetBufferSize);
    SmartArrayPtr < Solution > alsolv(mMaxLocalSolutionBufferSize);
    FixedVector < Solution > lsolv(alsolv, mMaxLocalSolutionBufferSize);
    for(int step = 1; ; step ++) {
      Set s;
      if(ltq.empty()) {
        pthread_mutex_lock(&mMutTaskQueue);
        mStarv ++;
	mSteps = BNBMAX(mSteps, step);
        while(mTaskQueue.empty() && (mStarv != mNumThreads) && (mSteps < mLocalSteps)) {
	  mLocalCounters[tid].mStarv ++;
	  struct timeval tv;
	  double t1, t2;
	  gettimeofday(&tv, NULL);
	  t1 = (double)tv.tv_sec + (double)tv.tv_usec * 0.000001;
          pthread_cond_wait(&mCV, &mMutTaskQueue); 
	  gettimeofday(&tv, NULL);
	  t2 = (double)tv.tv_sec + (double)tv.tv_usec * 0.000001;
	  mLocalCounters[tid].mStarvTime += (t2 - t1);
        }       
	if(mSteps >= mLocalSteps) {
	  pthread_cond_broadcast(&mCV);
          pthread_mutex_unlock(&mMutTaskQueue);
          break;
	} else if(!mTaskQueue.empty()) {
          s = mTaskQueue.top ();
          mTaskQueue.pop ();
          mStarv --;
	  mLocalCounters[tid].mGet ++;
	  pthread_mutex_unlock(&mMutTaskQueue);
        } else {
	  pthread_cond_broadcast(&mCV);
          pthread_mutex_unlock(&mMutTaskQueue);
          break;
        }
      } else {
        s = ltq.back();
        ltq.pop_back();
      }
      if (!mSetFactory->discard (s, getRecord())){
        mSetFactory->branch (s, lsetv, lsolv, getRecord(),  mInfos + tid, ltq.size());
        typename ProblemFactory::ValueType rec = getRecord();
        while(!lsolv.empty()) {
          Solution s = lsolv.back();
          lsolv.pop_back();
          if(((Factory::getProblemType() == BNB_MAXIMIZE) && (s.getValue() > rec)) ||
	     ((Factory::getProblemType() == BNB_MINIMIZE) && (s.getValue() < rec))) {
            rec = s.getValue();
            if(!solv.empty())
              solv.pop_back();
            solv.push_back(s);
          }
        }
	updateRecord(rec);
        while(!lsetv.empty()) {
          Set s = lsetv.back();
          lsetv.pop_back();
          if(!mSetFactory->discard (s, rec))
            ltq.push_back(s);
          else 
            mInfos[tid].mDiscardedByRecord ++;         
        }
        
	mSteps = BNBMAX(mSteps, step);
	if(mSteps >= mLocalSteps) {
	  pthread_mutex_lock(&mMutTaskQueue);
	  if(mStarv)
	    pthread_cond_broadcast(&mCV);
	  pthread_mutex_unlock(&mMutTaskQueue);
	  break;
	}

        if((step % mUpdateRatio) == 0) {
          if(!ltq.empty()) {
	    pthread_mutex_lock(&mMutTaskQueue);
	    struct timeval tv;
	    double t1, t2;
	    gettimeofday(&tv, NULL);
	    t1 = (double)tv.tv_sec + (double)tv.tv_usec * 0.000001;
	    mLocalCounters[tid].mDonat ++;
	    for(int i = 0; i < mPutChunk; i ++) {
	      if(!ltq.empty()) {
                MMRegistry::registerMemManager(mAuxMemManager);
		Set s = ltq.back();
		mTaskQueue.push(s);
                MMRegistry::registerMemManager(mManagers[tid]);
		ltq.pop_back();
	        mLocalCounters[tid].mPut ++;
	      } else
		break;
	    }
	    mQLen = mTaskQueue.size();
	    mMaxQLen = BNBMAX(mQLen, mMaxQLen);
	    if(mStarv)
	      pthread_cond_broadcast(&mCV);
	    t2 = (double)tv.tv_sec + (double)tv.tv_usec * 0.000001;
	    mLocalCounters[tid].mDonatTime += t2 - t1;
	    pthread_mutex_unlock(&mMutTaskQueue);
	  }
        }
      } else {
        mInfos[tid].mDiscardedByRecord ++;
      }
    }
    
    pthread_mutex_lock(&mMutTaskQueue);
    while(!ltq.empty()) {
      Set s = ltq.back();
      ltq.pop_back();
      mTaskQueue.push(s);
    }   
    pushSolutions (solv, mInfos + tid);
    pthread_mutex_unlock(&mMutTaskQueue);
  }


/**
 * Produces state description
 *
 * @return string representation of the current state
 *
 */
  std::string getStateDescription()
  {
    std::ostringstream os;

    os << " *** INCUMBENT VALUE = " << getRecord();
    if(mSolutionContainer->empty())
      os << "EMPTY";
    else 
      os << "\n *** SOLUTION :\n"<< mSolutionContainer->top().toString();
    os << "\n *** CANDIDATE PROBLEMS :\n" << toString (&mTaskQueue);

    return os.str();
  }

/**
 * Creates a string representation for a 
 * prioritized queue 
 * 
 * @param inQueue
 *
 */
  template <class Queue> std::string toString (Queue *inQueue)
  {
    typename Queue::value_type s;
    Queue dupq = *inQueue;
    std::ostringstream ostr;
    int sz;

    sz = dupq.size ();
    if(sz == 0 ) {
      ostr << "empty";
    } else {
      ostr << "size = " << sz ;
      ostr<< ", data: \n";
      while (!dupq.empty ())
	{
          s = dupq.top ();
	  ostr << s.toString () << "\n";
          dupq.pop ();
	}
    }
    return ostr.str ();
  }

  
/**
 *  The task queue
 */
  TraverseQueue mTaskQueue;


/**
 * The set factory
 */
  ProblemFactory *mSetFactory;


private:
  
  
  
  typename ProblemFactory::ValueType mRecord;
  BNBLog<BNB_DO_LOGGING> mLogger;
  unsigned int  mDebug;
  SolutionContainer < Factory > * mSolutionContainer; 
  SingleSolutionContainer < Factory > mSingleSolutionContainer;
  pthread_t *mThreads;
  BNBBranchInfo* mInfos;
  LocalCounters* mLocalCounters;
  MemManager** mManagers;
  MemManager* mAuxMemManager;
  int mThreadId;
  int mNumThreads;
  int mLocalSteps;
  int mUpdateRatio;
  int mStarv;
  int mSteps;
  int mPutChunk;
  unsigned int mMaxLocalSetBufferSize;
  unsigned int mMaxLocalSolutionBufferSize;
  unsigned int mMaxLocalQueueSize;
  unsigned int mQLen;
  unsigned int mMaxQLen;
  pthread_cond_t mCV;
  pthread_mutex_t mMutTaskQueue;
  pthread_mutex_t mMutRec;

};

#endif
