#ifndef __FILELOCK_HPP__
#define __FILELOCK_HPP__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include "bnberrcheck.hpp"

/**
 * Set a lock on a file
 */
class FileLock {
  public:

    /**
     * @param name of a file to lock
     */
    FileLock(const std::string &fname)
    {
      mFname = fname;
      mFd = -1;
    }

    /**
     * Lock file
     */
    void lock()
    {
      struct flock fl = { F_WRLCK, SEEK_SET, 0, 0, 0};
      int fd;

      fl.l_pid = getpid();

      if ((fd = open(mFname.c_str(), O_CREAT | O_RDWR, 0644)) == -1) {
	BNB_ERROR_REPORT("failed to open file for locking");
      } else {
	mFd = fd;
      }

      if (fcntl(mFd, F_SETLKW, &fl) == -1) {
	BNB_ERROR_REPORT("failed to lock the file");
      }
    }

    /**
     * Unlock file
     */
    void unlock()
    {
      struct flock fl = { F_UNLCK, SEEK_SET, 0, 0, 0};
      int fd;

      fl.l_pid = getpid();
      if(mFd == -1) {
	BNB_ERROR_REPORT("trying to unlock file that wasn't locked");
      }
      if (fcntl(mFd, F_SETLK, &fl) == -1) {
	BNB_ERROR_REPORT("failed to unlock the file");
      }
      close(mFd);
      mFd = -1;
    }

private:

    std::string mFname;
    int mFd;
};
#endif
