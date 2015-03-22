#ifndef __FILEUTILS_HPP__
#define __FILEUTILS_HPP__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>

#include "bnberrcheck.hpp"

class FileUtils {
  public:

    /**
     * Remove files in a folder.
     * @param dir folder name
     * @param pattern string pattern to match against the file name
     */
    static void removeFiles(const std::string &dir, const std::string &pattern)
    {
      std::string cmd = "cd ";
      cmd += dir;
      cmd += "; rm -f ";
      cmd += pattern; 
      int rc = system(cmd.c_str());
      if(rc != 0)
	BNB_ERROR_REPORT("failed to remove files");
    }

};

#endif
