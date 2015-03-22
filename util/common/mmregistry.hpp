#ifndef __MMREGISTRY_HPP__
#define __MMREGISTRY_HPP__

#include <pthread.h>
#include "bnberrcheck.hpp"
#include "stdmemmanager.hpp"


class MMRegistry {
  public:
    
    static void defaultInit()
    {
      mDefaultManager =  new StdMemManager();
    }
    
    /**
     * Initialize concurrent thread specific registry
     * call in main thread
     */
    static void concurrentInit()
    {
      mConcurrent = true;
      pthread_key_create(&mMemManagerKey, NULL);
    }
    
    
    /**
     * Finalize concurrent thread specific registry
     * call in main thread
     */
    static void concurrentFinit()
    {
      mConcurrent = false;
      pthread_key_delete(mMemManagerKey);
    }

    /**
     * Register Memory Manager for a specific thread
     * call within specific thread
     * @param mm pointer to a memory manager
     */
    static void registerMemManager(MemManager* mm)
    {
      pthread_setspecific(mMemManagerKey, (void*)mm);
    }
        
    /**
     * Retrieve Memory Manager for a specific thread
     * call within a specific thread
     * @return pointer to a memory manager
     */
    static MemManager* concGetMemManager()
    {
      void* p = pthread_getspecific(mMemManagerKey);
      if(p == NULL)
        p = mDefaultManager;
      return (MemManager*)p;
    }
    
    /**
     * Retrieve pointer to a memory manager
     * @return memory manage pointer
     */
    static MemManager* getMemManager()
    {
      if(mDefaultManager == NULL) {
        defaultInit();
      } 
      if(mConcurrent) {
        return concGetMemManager();
      } else {
        return mDefaultManager;
      }
    }
    
  private:
    
    static const int mMaxMemManagers = 128;
    static MemManager *mDefaultManager;
    static bool mConcurrent;
    static pthread_key_t mMemManagerKey;
};

#endif

