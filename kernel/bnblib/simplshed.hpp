#ifndef _SIMPLSHED_HPP_
#define _SIMPLSHED_HPP_

#include "ashed.hpp"

/**
 * Simple scheduler. When queue grows to the limit of maxQLen - forbid exchange.
 * when queue drops to minQLen - permit exchange.
 *
 * @author Mikhail Posypkin, ISA RAS, posypkin@isa.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 */
  class SimpleScheduler : public AbstractScheduler 
  {
  public:

    /**
     * States of the scheduler
     */
    struct States
    {
      /**
       * The queue was shorter than the treshold
       */
      static const int mShortQueue = 1;

      /**
       * The queue was long than the treshold
       */
      static const int mLongQueue = 2;
    };

    /**
     * The constructor
     * 
     * @param masterCT the complexity treshold for the master process
     *
     * @param maxQLen maximal task queue length
     *
     * @param minQLen minimal task queue length
     *
     * @param slaveCT slave complexity treshold
     *
     * @param slaveChunk slave chunk 
     */
    SimpleScheduler()
    {
      mState = States::mShortQueue;
    }

    /**
     * Set up master treshold
     * @param tresh treshold value
     */
    void setMasterTreshold(InfiniInt tresh)
    {
      mMasterCT = tresh;
    }
    
    /**
     * Set slave treshold
     * @param tresh treshold value
     */
    void setSlaveTreshold(InfiniInt tresh)
    {
      mSlaveCT = tresh;
    }
    
    /**
     * Set up slave chunk
     * @param chunk chunk value
     */
    void setSlaveChunk(InfiniInt chunk)
    {
      mSlaveChunk = chunk;
    }
    
    /**
     * Set up maximum queue length
     * @param len length
     */
    void setMaxQueueLength(int len)
    {
      mMaxQLen = len;
    }
    
    /**
     * Set up minimum queue length
     * @param len length
     */
    void setMinQueueLength(int len)
    {
      mMinQLen = len;
    }
    
   /**
    * Check whether scheduler should be activated;
    *
    * @return true if "yes"
    *
    */
    virtual bool activate(int qlen, int freeSlaves, int totalSlaves)
    {
      if((qlen < mMinQLen) && (mState != States::mShortQueue)) {
	mState = States::mShortQueue;
	return true;
      } else if((qlen > mMaxQLen) && (mState != States::mLongQueue)) {
	mState = States::mLongQueue;
	return true;
      } else
	return false;
    }
    /**
     * @return slave complexity treshold
     *
     */
    virtual InfiniInt getSlaveCT() {
      /*
      if(mState == States::mShortQueue)
	return mSlaveCT;
      else {
        InfiniInt a;
	return a;
      }
      */
      return mSlaveCT;
    }

    /**
     * @return  complexity treshold for the master process
     */
    virtual InfiniInt getMasterCT() {
      return mMasterCT;
    }

    /**
     * @return  Slave "chunk"
     *
     */
    virtual InfiniInt getSlaveChunk() {
      if(mState == States::mShortQueue)
	return mSlaveChunk;
      else {
        InfiniInt zero(0);
	return zero;
      }
    }

  private:
    
    /**
     * Maximal task queue length
     */
    int mMaxQLen;
    
    /**
     * Minimal queue length
     */
    int mMinQLen;

    /**
     * Slave chunk
     */
    InfiniInt mSlaveChunk;

    /**
     * Master complexity treshold
     */
    InfiniInt mMasterCT;

    /**
     * Slave complexity problem treshold
     */
    InfiniInt mSlaveCT;
    
    /**
     * Holds the class state
     */
    int mState;
  };
#endif
