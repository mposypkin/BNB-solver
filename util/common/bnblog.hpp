#ifndef _BNBLOG_HPP_
#define _BNBLOG_HPP_
/**
 * Supports switchable logging.
 *
 * @author Mikhail Posypkin, ISA RAS, posypkin@isa.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file bnblog.hpp
 */

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include "bnberrcheck.hpp"

#define BNB_DO_LOGGING 1

#define BNB_NO_LOGGING 2

template <int LOG> class BNBLog;
/* 
{
public:
  template <class T> BNBLog& operator << (const T& item);

};
*/
template <> class BNBLog <BNB_DO_LOGGING> 
{
public:

/**
 * The constructor
 */
  BNBLog()
  {
    mName = "LOG";
    mFName = "stdout";
    mPermit = true;
    mBuffer.precision(6);
    mBuffer.setf(std::ios::fixed, std::ios::floatfield);
  }

/**
 * Insert item to the string buffer
 *
 * @param item item to put
 *
 * @return pointer to the log class
 *
 */
  template <class T> BNBLog& operator << (const T& item)
  {
    if(mBuffer.str().size() > mCriticalBufferSize) {
      printf("((((****\n");
      write();
      printf("****))))\n");
      BNB_ERROR_REPORT("Logger exceeds critical size!");
    }
    mBuffer << item;
    return *this;
  }
  
  /**
   * Prints (adds to buffer) formatted output
   * @param fmt format string
   */
  void print(const char* fmt, ...)
  {
    char buf[1024 * 1024];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    mBuffer << buf;    
    va_end(args);
  }
  
/**
 * Write the buffer contents to the terminal or file
 *
 */
  void write()
    {
      time_t t;
      std::string s;
      std::ostringstream os;
      static int cnt = 0;

      if(mPermit) {
	t = time(NULL);
	s = ctime(&t);
	s[s.size() - 1] = ' ';
	os << "[("<< mProcessNumber << ") " << mName <<  ":" << cnt ++ << ": " << s << "]" << mBuffer.str();
	if(mFName == "stdout") {
	  printf("%s", os.str().c_str());
	  fflush(stdout);
	} else if(mFName == "stderr") {
	  fprintf(stderr, "%s", os.str().c_str());
	  fflush(stderr);
	} else {
	  FILE* f = fopen(mFName.c_str(), "a");
	  if(f == NULL) {
	    BNB_ERROR_REPORT("Can't open file for logging\n");
	  }
	  fprintf(f, "%s", os.str().c_str());
	  fclose(f);
	}

      }
      mBuffer.str("");
    }

  /**
   * Turn the logging on
   */
  void turnOn()
  {
    mPermit = true;
  }

  /**
   * Turn the logging off
   */
  void turnOff()
  {
    mPermit = false;
  }

  /**
   * Setup the logger name
   */
  void setName(std::string name)
  {
    mName = name;
  }

  /**
    * Set file name to output
    */
  void setFile(std::string fname)
  {
    mFName = fname;
  }

  /**
   * Retrieve the logger name
   */
  std::string getName()
  {
    return mName;
  }

/**
 * Set up Universal logger
 *
 */
  static void setUniLog(BNBLog<BNB_DO_LOGGING>* log)
  {
    mUniLog = log;
  }

/**
 * @return Universal logger
 *
 */
  static BNBLog<BNB_DO_LOGGING>* getUniLog()
  {
    return mUniLog;
  }
  
  /**
   * Set up process number
   */
  static void setProcessNumber(int pn)
  {
    mProcessNumber = pn;
  }

private:


  /**
   * Content storage
   */
  std::ostringstream mBuffer;

  /**
   * Logger name 
   */
  std::string mName;

  /**
    * Output file name
    */
  std::string mFName;
  
  /**
   * The do logging or not 
   */
  bool mPermit;   

/**
 * Pointer to the universal logger
 */
  static BNBLog<BNB_DO_LOGGING>* mUniLog;

  /**
   * Critical buffer size
   */
  const static int mCriticalBufferSize = 10000;
  
  
  /**
   * Process number
   */
  static int mProcessNumber;
  
};

/**
 * A dummy logger implementation
 */
template <> class BNBLog <BNB_NO_LOGGING>
{
public:
  template <class T> BNBLog& operator << (const T& item)
    {
    }
  
  inline void write()
   {
   }

  void turnOn()
  {
  }

  void turnOff()
  {
  }

  void setName(std::string name)
  {
    mName = name;
  }

  std::string getName()
  {
    return mName;
  }
  
  private:
    
  std::string mName;
  std::string mFName;

};

#endif
