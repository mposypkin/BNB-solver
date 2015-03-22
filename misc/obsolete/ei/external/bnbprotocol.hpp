#ifndef _BNBPROTOCOL_HPP_
#define _BNBPROTOCOL_HPP_
/**
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file protocol.hpp
 *
 * Protocol constants for BNB external interface
 * 
 */

#include <string>

/*************************************
 * General constants
 */

/**
 * "NULL" constant
 */
#define BNB_NIL "NIL"

/**
 * Search tree top
 */
#define BNB_TOP "TOP"

/**************************************
 * Keyword printed by BNB-Server when started
 **/

#define BNB_SERVER_STARTED_KEYWORD "BNBSRVSTART"

/*************************************
 * Command types
 *************************************/

/**
 * Synchronous mode
 */
#define BNB_SYNC "SYNC"

/**
 * Asynhronous command
 */
#define BNB_ASYNC "ASYNC"

/**
 * Local command (only inside the solver)
 */
#define BNB_LOCAL "LOCAL"


/*************************************
 * Command names
 *************************************/

/**
 * Run solver
 */
#define BNB_RUN "RUN"

/**
 * Connection broken from peer
 */
#define BNB_BRK_CONN "BRK_CONN"


/**
 * "Handshaking" message
 */
#define BNB_HELLO "HELLO"


/**
 * Set up diganostic level
 */
#define BNB_SET_DIAG_LEVEL "SET_DIAG_LEVEL"


/**
 * Requests certain number of processes
 * for the solver
 */
#define BNB_PUT_PROCS_NUMBER "PUT_PROCS_NUMBER"

/**
 * Put solver configuration
 */
#define BNB_PUT_SOLVER_CFG "PUT_SOLVER_CFG"

/**
 * Interrupt solving 
 */
#define BNB_INT "INTRPT"

/**
 * Terminate application
 */
#define BNB_EXIT "EXIT"

/**
 * Put problem description
 */
#define BNB_PUT_PROBLEM_DSC "PUT_PROBLEM_DSC"

/**
 * Put problem data
 */
#define BNB_PUT_PROBLEM_DATA "PUT_PROBLEM_DATA"

/**
 * Create solver
 */
#define BNB_CREATE_SOLVER "CREATE_SOLVER"


/**
 * Initiate solving process
 */
#define BNB_SOLVE "SOLVE"

/**
 * Start solving process
 */
#define BNB_START_SOLVE "START_SOLVE"


/**
 * Terminate solver. "Backup sets".
 */
#define BNB_STOP "STOP"

/**
 * Solver process finishes
 */
#define BNB_FINISH "FINISH"

/**
  * Solver process exit status
  */
#define BNB_EXIT_STATUS "EXIT_STATUS"


/**
 * Get the solver state
 */
#define BNB_GET_STATE "GET_STATE"

/**
 * Information command
 */
#define BNB_INFO "INFO"

/**
 * Get the solver load
 */
#define BNB_GET_LOAD "GET_LOAD"

/**
 * Load information
 */
#define BNB_LOAD_INFO "LOAD_INFO"

/**
 * Requests sets
 */
#define BNB_GET_SETS "GET_SETS"

/**
 * Puts sets (vertices) for branch-and-bound
 */
#define BNB_PUT_SETS "PUT_SETS"

/**
 * Requests solutions
 */
#define BNB_GET_SOLUTIONS "GET_SOLUTIONS"


/**
 * Put solutions
 */
#define BNB_PUT_SOLUTIONS "PUT_SOLUTIONS"


/**
 * Record update command
 */
#define BNB_RECORD_UPDATE "RECORD_UPDATE"

/**
 * Incumbent solution update command
 */
#define BNB_INCUMBENT_UPDATE "INCUMBENT_UPDATE"


/*******************************************
 * Codes
 *******************************************/
#define INCUMBENT_UPDATE_CODE  "exec $SINCUM; forward"
//#define INCUMBENT_UPDATE_CODE  "echo PIPISKA"


/********************************************
 * Command status
 ********************************************/

/**
 * Command ready
 */
#define BNB_READY "READY"

/**
 * Command done
 */
#define BNB_DONE "DONE"

/**
 * Command failed
 */
#define BNB_ERROR "ERROR"

/********************************************
 * Exit statuses
 ********************************************/

/**
  * Success status
  */
#define BNB_STATUS_SUCCESS "SUCCESS"

/**
  * Failed status
  */
#define BNB_STATUS_FAILED "FAILED"

/*********************************************
 * Errors
 *********************************************/

/**
 * Illegal state
 */
#define BNB_ERR_ILLEGAL_STATE "ILLEGAL_STATE"

/**
 * Unknown command
 */
#define BNB_ERR_UNKNOWN_COMMAND "UNKNOWN_COMMAND"


/********************************************
 * Solver state
 ********************************************/


/**
 * Process number for running solver is set
 */
#define BNB_IS_PROCS_NUMBER_SET "IS_PROCS_NUMBER_SET"

/**
 * Solver configuration is set field
 */
#define BNB_IS_SOLVER_CFG_SET "IS_SOLVER_CFG_SET"

/**
 * Problems description is set field
 */
#define BNB_IS_PROBLEM_DESCRIPTION_SET "IS_PROBLEM_DESCRIPTION_SET"

/**
 * Problem data is set field
 */
#define BNB_IS_PROBLEM_DATA_SET "IS_PROBLEM_DATA_SET"


/**
 * Solver created field
 */
#define BNB_IS_SOLVER_CREATED "IS_SOLVER_CREATED"

/**
 * Solve in progress field 
 */
#define BNB_IS_SOLVE_IN_PROGRESS "IS_SOLVE_IN_PROGRESS"

/**
 * Command in progress field
 */
#define BNB_IS_COMMAND_IN_PROGRESS "IS_COMMAND_IN_PROGRESS"

/**
 * Current command id field
 */
#define BNB_CURRENT_COMMAND_ID "CURRENT_COMMAND_ID"

/**
 * Structure representing solver state (use in solver reply)
 */
struct BNBSolverState {

/**
 * The constructor
 */
  BNBSolverState()
  {
    mIsProcsNumberSet = false;
    mIsSolverCfgSet = false;
    mIsProblemDescriptionSet = false;
    mIsProblemDataSet = false;
    mIsSolverCreated = false;
    mIsSolveInProgress = false;
    mIsCommandInProgress = false;
    mCurrentCommandId = "";
  }
  

/**
 * Converts state to string representation
 *
 * @param s
 *
 */
  void toString(std::string & s)
  {
    s += BNB_IS_PROCS_NUMBER_SET;
    s += "=";
    s += (mIsProcsNumberSet ? "1" : "0");
    s += "\n";

    s += BNB_IS_SOLVER_CFG_SET;
    s += " = ";
    s += (mIsSolverCfgSet ? "1 " : "0");
    s += "\n";

    s += BNB_IS_PROBLEM_DESCRIPTION_SET;
    s += " = ";
    s += (mIsProblemDescriptionSet ? "1 " : "0");
    s += "\n";

    s += BNB_IS_PROBLEM_DATA_SET;
    s += " = ";
    s += (mIsProblemDataSet ? "1 " : "0");
    s += "\n";

    s += BNB_IS_SOLVER_CREATED;
    s += " = ";
    s += (mIsSolverCreated ? "1 " : "0");
    s += "\n";

    s += BNB_IS_SOLVE_IN_PROGRESS;
    s += " = ";
    s += (mIsSolveInProgress ? "1 " : "0");
    s += "\n";

    s += BNB_IS_COMMAND_IN_PROGRESS;
    s += " = ";
    s += (mIsCommandInProgress ? "1 " : "0");
    s += "\n";

    s += BNB_CURRENT_COMMAND_ID;
    s += "=";
    s += mCurrentCommandId;    
  }


/**
 * Process number set
 */
  bool mIsProcsNumberSet;

/**
 * Solver Configuration set
 */
  bool mIsSolverCfgSet;

/**
 * Problem description set
 */
  bool mIsProblemDescriptionSet;

/**
 * Problem data loaded
 */
  bool mIsProblemDataSet;

/**
 * Solver is created now
 */
  bool mIsSolverCreated;

/**
 * Solve is now in progress
 */
  bool mIsSolveInProgress;

/**
 * Command is now in progress
 */
  bool mIsCommandInProgress;

/**
 * Current command executing by the solver
 */
  std::string mCurrentCommandId;
};
 
#endif
