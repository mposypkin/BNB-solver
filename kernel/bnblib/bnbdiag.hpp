#ifndef _BNBDIAG_HPP_
#define _BNBDIAG_HPP_
/**
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file bnbdiag.hpp
 *
 * This file is for supporting diganostic output in BNB-Solver library
 */
#include <time.h>
#include <stdio.h>
#include <string>
#include <queue>
#include <sstream>



/**
 * Diagnostic levels
 */

/**
 * No diagnostic
 */
#define BNB_DIAG_NONE 0 


/**
 * Only severe errors (default mode)
 */
#define BNB_DIAG_SEVERE 1

/**
 * Medium diagnostic output
 */
#define BNB_DIAG_MEDIUM 2

/**
 * All information goes here
 */
#define BNB_DIAG_FULL 3


/**
 * Message kinds
 */

/**
 * Error message
 */
#define BNB_ERROR_MESSAGE 1

/**
 * Warning message
 */
#define BNB_WARNING_MESSAGE 2

/**
 * Information message
 */
#define BNB_INFO_MESSAGE 3


/**
 * Main class for diagnostics. All diagnostics should go through it. 
 *
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 */
class BNBDiag {

public: 

  struct Message{

    /**
     * Message text 
     */
    std::string mText;

    /**
     * Message kind
     */
    int mKind;


    /**
     * Message priority
     */
    int mPriority;
  };

  static void error(std::string text, bool doFlush = true)
  {
    message(text, BNB_ERROR_MESSAGE, BNB_DIAG_SEVERE);
    if(doFlush)
      flush();
  }

  static void warning(std::string text, bool doFlush = true)
  {
    message(text, BNB_WARNING_MESSAGE, BNB_DIAG_MEDIUM);
    if(doFlush)
      flush();   
  }

  static void info(std::string text, int priority = BNB_DIAG_MEDIUM, bool doFlush = true)
  {
    message(text, BNB_INFO_MESSAGE, priority);
    if(doFlush)
      flush();
  }


  static int getPriority()
  {
    return mCurrentPriority;
  }

  static void setPriority(int priority)
  {
    mCurrentPriority = priority;
  }

private:

  static void message(std::string text, int kind, int priority)
  {
    Message m;
    std::string s;
    time_t t;
    static int c;
    std::ostringstream os;

    os << c ++ << "|";
    t = time(NULL);
    s = ctime(&t);
    m.mText = text;
    m.mText += " [";
    m.mText += os.str();
    m.mText += s;
    m.mText[m.mText.size()-1] = ']';
    m.mKind = kind;
    m.mPriority = priority;
    mMessages.push_back(m);    
  }

  static void flush()
  {
    while(!mMessages.empty())
      {
	Message m;

	m = mMessages.front();
	mMessages.pop_front();
	if(m.mPriority <= mCurrentPriority) 
	  if(m.mKind == BNB_ERROR_MESSAGE) {
	    fprintf(stderr, "ERROR: %s\n", m.mText.c_str());
	    fflush(stderr);
	  } else if(m.mKind == BNB_WARNING_MESSAGE) {
	    fprintf(stderr, "WARNNG: %s\n", m.mText.c_str());
	    fflush(stderr);
	  } else {
	    fprintf(stdout, "%s\n", m.mText.c_str());
	    fflush(stdout);
	  }
      }
	
  }

  static std::deque<Message> mMessages;

public:
  static int mCurrentPriority;
};


int BNBDiag::mCurrentPriority = BNB_DIAG_SEVERE;

std::deque<BNBDiag::Message> BNBDiag::mMessages;
#endif
