#ifndef __BNBDEFS_HPP__
#define __BNBDEFS_HPP__

/**
  * Various BNB-Grid definitions
  */

/**
  * Command to run application  
  */
#define BNB_RUN_APP_CMD "runsolver.sh"

/**
  * Command to submit application in the grid
  */
#define BNB_SUBMIT_CMD "./bnbexec.sh"

/**
  * Input file for application.
  */
#define BNB_IN_FILE "bnbin.scr"

/**
  * Input file for application.
  */
#define BNB_OUT_FILE "bnbout.scr"

/**
 * Number of application processors
 */
#define RUNSOLVER_NP_TAG "np"

/**
 * Maximal time allocated for an application
 */
#define RUNSOLVER_MAXTIME_TAG "maxtime"

#endif
