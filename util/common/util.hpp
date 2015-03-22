#ifndef _UTIL_HPP_
#define _UTIL_HPP_
/**
 * Useful stuff for optimization problems
 *
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file util.hpp
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/times.h>
#include <sstream>
#include <float.h>
#include "utilmacro.hpp"
#include "infini.hpp"
#include "mmregistry.hpp"
#include "stdmemmanager.hpp"


/**
 * Basic class for time measuring.
 *
 * @author \b Mikhail \b Posypkin 
 * The author can be reached at posypkin@ispras.ru
 *
 * @warning Using this library requires an explicit permition from author.
 */
class Timer
{

public:

/**
 * The constructor.
 */
  Timer (const char *name)
  {
    mName = name;
    mTotalTime = 0;
    mUserTime = 0;
    mSystemTime = 0;
  }

/**
 * Starts time measuring. Should be called just before the measured event.
 */
  void start ()
  {
    struct tms ts;

    mTmpTotalTime = times (&ts);
    mTmpUserTime = ts.tms_utime;
    mTmpSystemTime = ts.tms_stime;
  }

/**
 * Stops time measuring. Should be called just after the measured event.
 */
  void stop ()
  {
    struct tms ts;

    mTotalTime += times (&ts) - mTmpTotalTime;
    mUserTime += ts.tms_utime - mTmpUserTime;
    mSystemTime += ts.tms_stime - mTmpSystemTime;
  }


/**
 * Retrieves total time in secs
 *
 * @return total time
 *
 */
  double getTotalTime ()
  {
    return mTotalTime / (double) sysconf (_SC_CLK_TCK);
  }

/**
 * Converts gathered information to string representation.
 *
 * @return string representation
 */
  std::string toString ()
  {
    std::ostringstream os;

    os << mName << ": "
      << "total = " << (double) mTotalTime /
      (double) sysconf (_SC_CLK_TCK) << " (sec), " << "user = " << (double)
      mUserTime /
      (double) sysconf (_SC_CLK_TCK) << " (sec), " << "system = " << (double)
      mSystemTime / (double) sysconf (_SC_CLK_TCK) << " (sec)";

    return os.str ();
  }

private:
  const char *mName;
  clock_t mTotalTime;
  clock_t mUserTime;
  clock_t mSystemTime;
  clock_t mTmpTotalTime;
  clock_t mTmpUserTime;
  clock_t mTmpSystemTime;
};


/** 
 * Basic class for performance counters.
 *
 * @author \b Mikhail \b Posypkin 
 * The author can be reached at posypkin@ispras.ru
 *
 * @warning Using this library requires an explicit permition from author.
 */
class Counter
{

public:

/**
 * The constructor.
 *
 * @param name counter name
 * @param value counter value
 */
  Counter (const char *name, int value = 0)
  {
    mName = name;
    mValue = value;
  }

/**
 * Gets the value of the counter.
 *
 * @return value of the counter
 */
  InfiniInt get () const
  {
    return mValue;
  }

/**
 * Sets the value of the counter.
 *
 * @param value new value
 */
  void set (InfiniInt value)
  {
    mValue = value;
  }

  /**
   * Increments the value of the counter.
   */
  void inc ()
  {
    mValue ++;
  }

/**
 * Increments the value of the counter.
 * @param value  the value of increment
 */
  void inc (InfiniInt value)
  {
    mValue = mValue + value;
  }

/**
 * Decrements the value of the counter.
 *
 * @param value  the value of increment
 *
  void dec (int value = 1)
  {
    mValue -= value;
  }
*/

/**
 * Converts counter name and value to string.
 *
 * @return string to output
 */
  std::string toString ()const
  {
    std::ostringstream os;
    os << mName << " = " << mValue.toString();
    return os.str ();
  }

private:
  InfiniInt mValue;
  const char *mName;
};


/**
 *
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 *
 * Basic class for monitoring value changing
 */
class Monitor
{
public:

/**
 * The constructor
 *
 * @param name name of the counter
 *
 * @return 
 *
 */
  Monitor (const char *name)
  {
    mName = name;
    mMinValue = mMaxValue = 0;
    mN = 1;
    mAverageValue = 0.0;
  }

/**
 * Observe new value
 *
 * @param value
 *
 */
  void set (unsigned int value)
  {
    if (value > mMaxValue)
      mMaxValue = value;
    if (value < mMinValue)
      mMinValue = value;
    mValue = value;
    mAverageValue =
      ((double) mN / (double) (mN + 1)) * (mAverageValue +
					   ((double) value) / (double) mN);
    mN++;
  }

/**
 * Serialize as a string
 *
 * @return created string
 *
 */
  std::string toString ()const
  {
    std::ostringstream os;

    os << mName << ": max = " << mMaxValue
      << ", min = " << mMinValue << ", average = " << mAverageValue;

    return os.str ();
  }


private:
  unsigned int mValue;
  unsigned int mMaxValue;
  unsigned int mMinValue;
  double mAverageValue;
  unsigned int mN;
  const char *mName;
};

/**
 * Support for the resizable buffers
 */
 
class Resizeable
{
public:
  
  /**
   * The constructor 
   * @param initSize initial size
   * @param chunkSize chunk size  
   */
  Resizeable(const int initSize = 0, const int chunkSize = 1)
  {
    mChunkSize = chunkSize;
    mBuf = NULL;
    mSize = 0;
    resize(initSize);
  }

  /**
   * Change size of the buffer
   */
  void* resize(const int newsize)
  {        
    if(newsize > mSize){
      int nsize;
      nsize = newsize - (newsize % mChunkSize) + mChunkSize;
      mBuf = realloc(mBuf, nsize);
      if(mBuf == NULL) {
	fprintf(stderr, "Resizeable: low on memory on resize (%d bytes)\n", nsize);
	exit(-1);
      }
      mSize = newsize;      
    }
    return mBuf;
  }

  /**
   * Retriev buffer pointer
   */
  void* getBuf() const
  {
    return mBuf;
  }
 
  /**
   * Retrieve size of the buffer
   */
  int getSize() const
  {
    return mSize;
  }
  
  /**
   * Return true if empty
   */
  bool empty() {
    return (mSize == 0);
  }
private:

  /**
   * Buffer pointer
   */
  void* mBuf;

  /**
   * Current buffer size
   */
  int mSize;

  /**
   * Chunk size (enlarge memory with such steps)
   */
  int mChunkSize;
};

/**
 * Return maximum for the given type
 */
template <class FloatType> FloatType getMaxValue ();

/**
 * Return minimum for the given type
 */
template <class FloatType> FloatType getMinValue ();


#endif

