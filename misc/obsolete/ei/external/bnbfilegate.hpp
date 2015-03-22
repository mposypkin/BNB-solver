#ifndef _BNBFILEGATE_HPP_
#define _BNBFILEGATE_HPP_
/**
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file bnbfilegate.hpp
 *
 * Implementation of the BNBGate inerface for files
 */

#include <stdio.h>
#include <string>
#include "bnbchargate.hpp"



/**
 *
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 *
 * File gate exception to throw
 */
class BNBFileGateError {
public:  
/**
 * Constructor
 *
 * @param errorDescr
 *
 */
  BNBFileGateError(const char* errorDescr)
  {
    fprintf(stderr, "%s\n", errorDescr);
    fflush(stderr);
    abort();
  }
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
 * Class implements BNBGate interface to files
 */
class BNBFileGate : public BNBCharGate{
public:

/**
 * Constructor
 *
 * @param iname name of the input file (NUL means no file to open)
 *
 * @param oname name of the output file (NUL means no file to open)
 *
 * @return 
 *
 */
  BNBFileGate(std::string iname, std::string oname)
  {
    mIFname = iname;
    mOFname = oname;
    mIF = NULL;
    mOF = NULL;
  }

/**
 * Open gate
 *
 */
  void open()
  {
    if(mIFname == "NUL") {
    }else if (mIFname ==  "IN") {
      mIF = stdin;
    }else {
      mIF = fopen(mIFname.c_str(), "r");
      if(mIF == NULL)
	{
	  throw BNBFileGateError("Wrong input file name");
	}
    }

    if(mOFname == "NUL") {
    }else if(mOFname == "OUT") {
      mOF = stdout;
    }else{
      mOF = fopen(mOFname.c_str(), "w");
      if(mOF == NULL)
	{
	  throw BNBFileGateError("Wrong output file name");
	}
    }
  }

/**
 * Close gate
 *
 */
  void close()
  {
    if(mIF)
      fclose(mIF);
    if(mOF)
      fclose(mOF);
  }


  /**
   * Probes whether data is ready to receive
   */
  bool probe()
  {
    if(mIF)
      return (feof(mIF) == 0);
    else 
      return false;
  }



private:


/**
 * Reads the input symbol
 *
 * @return read symbol
 *
 */
  int readc()
  {
    int c;
    c = fgetc(mIF);
    if(c == EOF)
      return -1;
    return c;
  }

/**
 * Writes byte seq the output buffer
 *
 * @param str byte seq
 *
 * @param sz number of bytes
 *
 */
  void writes(const char* str, int sz)
  {
    int rc;

    rc = fprintf(mOF,"%s\n",str);
    if(rc != sz + 1)
      throw BNBFileGateError("File error on write");      
  }


  
/**
 * Name of the input file
 */
  std::string mIFname;
  

/**
 * Name of the output file
 */
  std::string mOFname;

/**
 * File for reading commands
 */
  FILE* mIF;

/**
 * File for writing commands
 */
  FILE* mOF;
};

#endif
