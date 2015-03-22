#ifndef __CONCHELPER_HPP__
#define __CONCHELPER_HPP__

#include <pthread.h>
#include <util/common/bnberrcheck.hpp>

#define BNB_MAX_THREADS 1024

extern void* plauncher(void*);

class ConcHelper {
  public:
    
    ConcHelper()
    {
      mTid = 0;
      pthread_mutex_init(&mMutConc, NULL);
      pthread_key_create(&mTidKey, NULL);
    }

    ~ConcHelper()
    {
      pthread_mutex_destroy(&mMutConc);
      pthread_key_delete(mTidKey);
    }

  virtual void cmethod() = 0;

  /**
    * Setup number of threads
    * @param nt number of threads
    */
  void setNumberOfThreads(int nt)
  {
    BNB_ASSERT(mNumberOfThreads < BNB_MAX_THREADS);
    mNumberOfThreads = nt;
  }

  /**
    * Retrieve number of threads
    * @return number of threads
    */
  static int getNumberOfThreads()
  {
    return mNumberOfThreads;
  }

  /**
    * Retrieve thread number
    * @return thread number
    */
  static int getThreadRank()
  {
     void* p = pthread_getspecific(mTidKey);
     int rv;
     if(p == NULL)
       rv = 0;
     else 
       rv = *(int*)p;
     return rv;
  }

  
  /**
    * Choose thread id (number)
    */
  void assignTid()
  {
    pthread_mutex_lock(&mMutConc);
    int tid = mTid ++;  
    mTids[tid] = tid;
    pthread_setspecific(mTidKey, (void*)(mTids + tid));
    pthread_mutex_unlock(&mMutConc);
  }

  protected:

    static int mTid;
    static int mTids[BNB_MAX_THREADS];
    static int mNumberOfThreads;
    static pthread_mutex_t mMutConc;
    static pthread_key_t mTidKey;
};
#endif

