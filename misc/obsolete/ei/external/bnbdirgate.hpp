#ifndef __BNBDIRGATE_HPP__
#define __BNBDIRGATE_HPP__


#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <dirent.h>
#include "bnbgate.hpp"
#include "bnbcmdtags.hpp"
#include "bnbcmdparser.hpp"
#include <util/common/fileutils.hpp>
#include <util/common/filelock.hpp>

#define LOCK_FILE_NAME "lck"


static int one (const struct dirent *unused)
{
  int rc = 0;
  if(isdigit(unused->d_name[0]))
    rc = 1;
  return rc;
}


class BNBDirGate : public BNBGate {
  public:


    BNBDirGate(std::string ifolder, std::string ofolder) 
    {
      mIDir = ifolder;
      mODir = ofolder;
    }

    void open() 
    {
      FileUtils::removeFiles(mODir.c_str(), "*");
    }

    bool probe()
    {
      struct dirent **eps;
      int n;

      n = scandir (mIDir.c_str(), &eps, one, alphasort);
      if(n < 0)
	BNB_ERROR_REPORT("can't open output folder\n");
      return (n > 0);
    }

    bool get(BNBCommand & cmd)
    {
      bool rv;
      struct dirent **eps;
      int n;
      std::string s;

      n = scandir (mIDir.c_str(), &eps, one, alphasort);
      if(n < 0)
	BNB_ERROR_REPORT("failed to list directory");
      if(n > 0) {
	std::string fname = mIDir;
	fname += "/";
	fname += eps[0]->d_name;
	std::string lockfname = mIDir;
	lockfname += "/";
	lockfname += LOCK_FILE_NAME;
	FileLock fl(lockfname);
	fl.lock();
	FILE* f = fopen(fname.c_str(), "r");
	if(f == NULL)
	  BNB_ERROR_REPORT("Failed to open command file\n");
	while(true) {
	  int c = fgetc(f);
	  if(c == EOF)
	    BNB_ERROR_REPORT("incorrect file format for command");
	  if(c == BNB_COMMAND_START) 
	    s.erase(); 
	  if(c == BNB_COMMAND_END)
	    break;
	  s += c;
	}
	fclose(f);
	int rc = unlink(fname.c_str());
	if(rc != 0)
	  BNB_ERROR_REPORT("failed to delete file");
	fl.unlock();
	BNBCommandParser::fromString(s, cmd);
	rv = true;
      } else {
	rv = false;
      }
      return rv;
    }

    void put(BNBCommand & cmd) 
    {
    }

    void close()
    {
    }

  private:

    std::string mIDir;
    std::string mODir;
};

#endif
