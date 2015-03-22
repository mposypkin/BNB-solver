#ifndef _BNBEI_HPP_
#define _BNBEI_HPP_
/**
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file bnbei.hpp
 *
 * BNB external interface for using BNB solver from the outside.
 */

#include "bnbcommand.hpp"
#include "bnbcmdparser.hpp"
#include "bnbcmdprocessor.hpp"
#include "bnbgate.hpp"
#include <util/aclib/aclib.hpp>
#include "bnbprotocol.hpp"
#include <ei/factory/prfact.hpp>
#include <ei/factory/solverfactory.hpp>
#include <util/common/bnblog.hpp>
#include "bnbactions.hpp"

class BNBEI
{
public:
/**
 * Constructor
 *
 * @param gate gate class for communicating with the outside world
 *
 * @param ac abstract communicator for communicating with the solver
 *
 * @return 
 *
 */
  BNBEI(BNBGate* gate, AbstractCommunicator* ac)
  {
    mGate = gate;
    mAC = ac;
    mGateId = ac->getProcessNumber() - 1 ;
    mAC->putAttr(std::string(BNB_GATE_ID_ATTR), mGateId);
    mMasterId = 0;
    mSC = new AbstractCommunicator(mAC, mAC->getMyId() < mGateId);
    for(int i = 1; i < mSC->getProcessNumber(); i ++)
      mSlaves.push_back(i);
    mLog.setProcessNumber(mAC->getMyId());    
  }

/**
 * Main solver class
 */
  void solve()
  {
    if(mAC->getMyId() == mGateId) 
      gate();
    else if(mAC->getMyId() == mMasterId)
      solver(true);
    else
      solver(false);
  }

private:

  void gate()
  {
    std::string id;
    BNBCommand cmd;
    std::deque <BNBCommand> outQueue;
    std::deque <BNBCommand> inQueue;
    BNBCommandProcessor cmdp;              
    BNBSolverState state;
    
    mLog.setName("Gate");
    mGate->open();
    mGate->put(cmd);

    while(1)
    {
      int r;
      int s;
      bool exiting = false;
      BNBCommand cmd;
	
      if(mGate->probe()) {
        mLog << "waiting for command \n";
        mLog.write();	  	  
        if(!mGate->get(cmd))
          continue;
        mLog << "got from outside:" << cmd.mName;
        mLog << ((state.mIsCommandInProgress) ? ("command in progress[" + state.mCurrentCommandId + "]") : " no command"
            "in progress") << "\n";
        mLog.write();
        outQueue.push_back(cmd);
      } 
        
      if((r = mAC->probe(&s)) != -1) {
        mAC->recv(r,s);
        *mAC >> cmd;
        mLog << "got from solver:" << cmd.mName << "\n";
        mLog.write();
        inQueue.push_back(cmd);
      }
        
      if(!inQueue.empty()) {
        int action;
        std::deque <BNBCommand>::iterator cmdi;
        cmdi = inQueue.begin();
        if(cmdi->mId == state.mCurrentCommandId){
          state.mCurrentCommandId = "";
          state.mIsCommandInProgress = false;
        }           
        do {
          cmdp.process(*cmdi, action);    
          if(action & BNBActions::FORWARD) {
            mGate->put(*cmdi);
            mLog << "Put " << cmdi->mName << "\n";
            mLog.write();
            if(cmdi->mName == BNB_EXIT) 
              break;              
          } 
          if(action & BNBActions::EXIT) {
            mLog << "Exit command received \n";
            mLog.write();
	    cmdi->mName = BNB_EXIT;
	    cmdi->mCode = "forward";
	    outQueue.push_back(*cmdi);
            break;              
          }              
          if(action & BNBActions::ERASE) {
            inQueue.erase(cmdi);
            break;              
          }
          if(action & BNBActions::REPEAT) {
            continue;              
          }              
          cmdi ++;
        } while(cmdi != inQueue.end());
      } 
      
      if((state.mIsCommandInProgress == false) && (!outQueue.empty())) {
        int action;
        std::deque <BNBCommand>::iterator cmdi;
        cmdi = outQueue.begin();
        do {
          cmdp.process(*cmdi, action);    
          if(action & BNBActions::FORWARD) {
            state.mIsCommandInProgress = true;
            state.mCurrentCommandId = cmdi->mId;
            *mAC << *cmdi;
            mAC->send(mMasterId);
            mLog << "Forwarding: " << cmdi->mName << "\n";
            mLog.write();
            if(cmdi->mName == BNB_EXIT) {
	      mLog << "Exiting...\n";
	      mLog.write();
	      exiting = true;
              break;              
	    } 
          } 
          if(action & BNBActions::DROP) {
            state.mCurrentCommandId = cmdi->mId;
            *mAC << *cmdi;
            mAC->send(mMasterId);
            mLog << "Dropping: " << cmdi->mName << "\n";
            mLog.write();
          } 

          if(action & BNBActions::ERASE) {
            outQueue.erase(cmdi);
            break;              
          }
          if(action & BNBActions::REPEAT) {
            continue;              
          }
          cmdi ++;
        } while(cmdi != outQueue.end());
      }          	  	  
      usleep(10);
      if(exiting)
	break;
    }
    mGate->close();    
  }

  void solver(bool isMaster)
  {
    BNBProblemData* pd = NULL;
    BNBSolver* solver = NULL;
    std::string solverCfg = "";
    mLog.setName(isMaster ? "Master" : "Slave");
    while(1){
      BNBCommand cmd;
      bool success = true;
      
      if(isMaster) {
        mAC->recv(mGateId);      
        *mAC >> cmd;

        mLog << "got from gate: " << cmd.mName << "\n";
        mLog.write();
        *mSC << cmd;
        mSC->send(mSlaves);
      } else {
        mSC->recv(mMasterId);      
        *mSC >> cmd;
        mLog << "got command: " << cmd.mName << "\n";
        mLog.write();
      }
	
      if(cmd.mName == BNB_PUT_SOLVER_CFG) {
	if(solver)
	  solver->configure(cmd.mDsc);
      } else if(cmd.mName == BNB_PUT_PROBLEM_DSC) {
        pd = BNBProblemFactory::createProblemData(cmd.mDsc);
        mLog << "Problem data created\n";
        mLog.write();		
      } else if(cmd.mName == BNB_CREATE_SOLVER) {
	if(solver != NULL) 
	  fatal("Solver recreation is trying");	
	solver = BNBSolverFactory::createDistMemSolver(pd, mAC, mSC, cmd.mDsc);	
	mLog << "Solver created\n";
	mLog.write();
      } else if(cmd.mName == BNB_SOLVE) {
      }  else if(cmd.mName == BNB_EXIT) {
	mLog << "Exiting...\n";
	mLog.write();
        break;  
      } else if((cmd.mName == BNB_GET_SETS) || (cmd.mName == BNB_PUT_SETS)){
	success = false;
      } else {
	mLog << "Exceptional command:" << BNBCommandParser::toString(cmd);
	mLog.write();
      }
      
      if(isMaster) {
        if(success) {
	 cmd.mType = BNB_LOCAL;
	 cmd.mStatus = BNB_DONE;
        } else {
	 cmd.mStatus = BNB_ERROR;
        }
        *mAC << cmd;
        mAC->send(mGateId);
      }
      
      if(cmd.mName == BNB_SOLVE) {
	BNBCommand c;

	solver->solve();
        if(isMaster) {
          c.mName = BNB_EXIT_STATUS;
	  c.mDsc = BNB_STATUS_SUCCESS;
          c.mCode = "forward";
          *mAC << c;
          mAC->send(mGateId);
          c.mName = BNB_FINISH;
          c.mCode = "exec $ONFINISH";
          *mAC << c;
          mAC->send(mGateId);
        }
      } else if(cmd.mName == BNB_EXIT) {
	break;
      }
    }
  }


  void fatal(std::string diag)
  {
    BNBLog<BNB_DO_LOGGING> mLog;
    mLog << "FATAL ERROR: " << diag << "\n";
    mLog.write();
    mAC->abort(-1);
  }

/**
 * Pointer to the attached gate 
 */
  BNBGate* mGate;

  
/**
 * Pointer to the global communicator
 */
  AbstractCommunicator* mAC;
  
/**
 * Pointer to the communicator enclosing solver processors
 */
  AbstractCommunicator* mSC;


/**
 * Rank of the gate process in the global commuincator
 */
  int mGateId;

/**
 * Rank of the gate in the global communicator
 */
  int mMasterId;

/**
 * Ranks of  slaves processes in SolverC communicator
 */
  std::vector<int> mSlaves;
  
  /**
   * Logger
   */
  BNBLog<BNB_DO_LOGGING> mLog;

};

#endif
