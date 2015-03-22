#ifndef _MSHEUR2_HPP_
#define _MSHEUR2_HPP_
/**
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file msheur.hpp
 *
 * Master-slave solver with heuristic part added
 */


#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>
#include <string>
#include <queue>
#include <sstream>
#include <algorithm>
#include <map>

#include <util/common/util.hpp>
#include <util/common/infinipar.hpp>
#include "bnb.hpp"
#include <util/aclib/aclib.hpp>
#include "bnbdiag.hpp"
#include <util/common/bnblog.hpp>
#include "heur.hpp"
#include "ashed.hpp"
#include "bnbdebug.hpp"
#include "asolupdfilter.hpp"
#include "heurmaster.hpp"
#include "heurslave.hpp"
#include "asolgenerator.hpp"
#include <util/common/stdvector.hpp>

#define DEFAULT_ADAPT_STRATEGY_STEP 128
#define MAX_HISTORY_LENGTH 1024

/**
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file probe.hpp
 * Contains strategy for master-worker branch-and-bound solver. 
 * Master evaluates starvation cycles and ajusts treshold value accordingly.
 */


/**
 * Switch logging off 
 */
//#define LOGGING_OPTION BNB_NO_LOGGING

/**
 * Switch logging on
 */
#define LOGGING_OPTION BNB_DO_LOGGING


/**
 * NULL command
 */
#define HEUR_NIL_CMD -1


/**
 * Start solve process
 */
#define HEUR_START_CMD 0

/**
 * Stop process
 */
#define HEUR_STOP_CMD 1


/**
 * Finshed task
 */
#define HEUR_FINISH_CMD 2


/**
 * Process terminated
 */
#define HEUR_TERMINATED_CMD 3

/**
 * New record arrived
 */
#define HEUR_RECORD_UPDATE_CMD 4

/**
 * New sets arrived
 */
#define HEUR_SET_UPDATE_CMD 5


/**
 * New solutions arrived
 */
#define HEUR_SOLUTION_UPDATE_CMD 6

/**
 * Sets are requested
 */
#define HEUR_SET_GET_CMD 7

/**
 * Solution get
 */
#define HEUR_SOLUTION_GET_CMD 8

/**
 * Information 
 */
#define HEUR_INFO_CMD 9

/**
 * Statistics results
 */
#define HEUR_STAT_CMD 10

/**
 * Update incumbent solution
 */
#define HEUR_INCUMBENT_UPDATE_CMD 11

/**
 * Auxilary check command
 */
#define HEUR_PING_CMD 12




/**
 * Main class for BNB solver.
 *
 * This class has three template parameters:
 *   ProblemFactory - class for creating search tree nodes
 *   Traverse   - class resposible for branching strategy
 *   Heur - class supporting heuristic searches
 *    
 * @author \b Mikhail \b Posypkin 
 * The author can be reached at posypkin@isa.ru
 *
 * @warning Using this library requires an explicit permition from author. 
 */
template <class Traverse> class BNBMsHeurSolver:
  public Traverse, 
  public BNBSolver
{
public:

  typedef typename Traverse::Factory::Set Set;

  typedef typename Traverse::Factory::Solution Solution;
  
  typedef typename Traverse::Factory ProblemFactory;

/**
 * The reduced version of the constructor.
 */
  BNBMsHeurSolver (AbstractCommunicator * ac, 
		   ProblemFactory * fact, 
		   int numberOfBranchProcesses) : 
    Traverse (fact)
  {
    mGC = NULL;
    mAC = ac;
    mMasterRank = 0;
    BNB_ASSERT(numberOfBranchProcesses + 3 <= mAC->getProcessNumber());
    if(numberOfBranchProcesses > 0) {
      mBranchMasterRank = 1;
      for(int i = mBranchMasterRank + 1; i <= numberOfBranchProcesses + mBranchMasterRank; i ++)
        mBranchSlaves.push_back(i);
      mHeurMasterRank = mBranchMasterRank + numberOfBranchProcesses + 1;
    } else {
      mBranchMasterRank = -1;
      mHeurMasterRank = 1;
    }    
    for(int i = mHeurMasterRank + 1; i < mAC->getProcessNumber(); i ++)
      mHeurSlaves.push_back(i);    
    mScheduler = NULL;
    mCheckRatio = 100;
    mRecordUpdatesRatio = 1;
    mSolutionUpdateFilter = NULL;
    mSolutionGenerator = NULL;
    mHeurMaster = NULL;
    mHeurSlave = NULL;
    mNumberOfInitialSolutions = 1;
    mSleepTime = 1;
  }

  /**
   * Setupp global communicator pointe
   * @param gc global communicator pointer
   */
  void setGlobalCommunicator(AbstractCommunicator* gc)
  {
    mGC = gc;
  }
  
  /**
   * Set up a pointer to heuristic master manager
   * @param hm pointer
   */
  void setHeurMaster(BNBHeurMaster<ProblemFactory>* hm)
  {
    mHeurMaster = hm;
  }
  
  /**
   * Setup a pointer to a heuristic slave class
   * @param hs pointe
   */
  void setHeurSlave(BNBHeurSlave<ProblemFactory>* hs)
  {
    mHeurSlave = hs;
  }
  
  /**
   * Set up a solution update filter
   *
   * @param filter a pointer to the solution update filter
   */
  void setSolutionUpdateFilter(AbstractSolutionUpdateFilter< ProblemFactory > * filter)
  {
    mSolutionUpdateFilter = filter;
    Traverse::setSolutionContainer(filter);
  }
  
  /**
   * Setup solution generator
   * @param gen solution generator
   */
  void setSolutionGenerator(AbstractSolutionGenerator< ProblemFactory > * gen)
  {
    mSolutionGenerator = gen;
  }
  
  /**
   * Setup number of initial solution
   * @param n number to set
   */
  void setNumberOfInitialSolutions(int n) 
  {
    mNumberOfInitialSolutions = n;
  }
 
/**
 * Set configuration
 */
  void configure(std::string config)
  {
  }

/**
 * The solver process.
 * Invokes both master and slave processes.
 *
 * @return number of brnaches at each process
 *
 */
  void solve ()
  {

    int r;

    r = mAC->getMyId ();

    if (r == mMasterRank)
      master();
    else if(r == mBranchMasterRank)
      branchMaster ();
    else if (r == mHeurMasterRank)
      heurMaster();
    else if ((mBranchMasterRank < r) && (r < mHeurMasterRank))
      branchSlave();
    else if (mHeurMasterRank < r)
      heurSlave();
  }


  
/**
 * Set the to the scheduler
 *
 * @param sched pointer to the scheduler
 *
 */
  void setScheduler(AbstractScheduler* sched)
  {
    mScheduler = sched;
  }

/**
 * Set up record update ratio
 *
 * @param val new ratio
 *
 */
  void setRecordUpdatesRatio(int val)
  {
    mRecordUpdatesRatio = val;
  }


/**
 * Set up check ratio
 *
 * @param val new ratio
 *
 */
  void setCheckRatio(int val)
  {
    mCheckRatio = val;
  }

private:


/**
 * The overall master process.
 *
 */
  void master()
  { 
#if 0
    BNBCommand gcmd;
    Set s;
    int term;
    Timer trecvgate("receive from gate time"); 
    Timer tsendgate("send to gate time");
    Counter crecvgate("packages received from gate");
    Counter crecvsetsgate("sets received from gate");
    Counter csentgate("packages sent to gate");
    Counter csentsetsgate("sets sent to gate");
    StdVector<Set> sv;
    StdVector<Solution> solv;
    BNBLog<LOGGING_OPTION> log;

    log.setName("Master");
    log << "Master-Slave + Heuristic search: " << mBranchSlaves.size() << " for branch, " << mHeurSlaves.size() << " for heuristics.\n";
    log.write();
    term = 1 + ((mBranchMasterRank != -1) ? 1 : 0);
    if(mGC) {
      while(1) {
        trecvgate.start();
        mGC->recv(-1);
        *mGC >> gcmd;
        crecvgate.inc();
        if(gcmd.mName == BNB_PUT_SETS) {
          if(gcmd.mDsc != BNB_NIL) 
            crecvsetsgate.inc(Traverse::mSetFactory->constructFromString(gcmd.mDsc, sv));
        } else if(gcmd.mName == BNB_PUT_SOLUTIONS) {
          if(gcmd.mDsc != BNB_NIL) {
            class SP : public XMLListener {
              public:
                SP(GVector<Solution>* solv, ProblemFactory* fact)
                {
                  mSolv = solv;
                  mFact = fact;
                }
        
                void fireTag(int level, const std::string& tag, const std::string& s, int begin, int end)
                {
                  if(tag == BNB_SOLUTION){
                    std::string str(s, begin, end - begin + 1);
                    mSolv->push_back(mFact->getSolutionFromString(str));
                  }
                }
              private:
                GVector<Solution>* mSolv;
                ProblemFactory* mFact;
            };
    
            SP spl(&solv, Traverse::mSetFactory);
            XMLPars pars;
            pars.addListener(&spl);
            pars.parse(gcmd.mDsc);
          } 
        } else if(gcmd.mName == BNB_START_SOLVE) {
          break;
        }
      } 
    } 
    if(!sv.empty() && (mBranchMasterRank != -1)) {
      *mAC << HEUR_SET_UPDATE_CMD;
      *mAC << sv;
      mAC->send(mBranchMasterRank);    
    }
    if(mBranchMasterRank != -1) {
      log << "Sending start command to brach master\n";
      log.write();
      *mAC << HEUR_START_CMD;
      mAC->send(mBranchMasterRank);    
    }
    if(solv.empty()) {
      if(mSolutionGenerator) {
        mSolutionGenerator->generate(mNumberOfInitialSolutions, solv);
      } else {
        for(int i = 0; i < mNumberOfInitialSolutions; i ++) {
          Solution sol;
          if(Traverse::mSetFactory->getInitialSolution(sol)) {
            log << "generated initial solution " << sol.getValue() << "\n";
            //log <<  sol.toString() << "\n";
            log.write();
            solv.push_back(sol);
          }
        }
      }
    } 
    
    *mAC << HEUR_SOLUTION_UPDATE_CMD;
    *mAC << solv;
    mAC->send(mHeurMasterRank);    
    log << "Sending start command to heuristic master\n";
    log.write();
    *mAC << HEUR_START_CMD;
    mAC->send(mHeurMasterRank);    
    while(1) {
      int rank;
      int msize;
      if(mGC) {
	rank = mGC->probe(&msize);
	if(rank != -1) {
	  trecvgate.start();
	  mGC->recv (rank, msize);
	  trecvgate.stop();
	  *mGC >> gcmd;
	  if(gcmd.mName == BNB_RECORD_UPDATE)
	    ;
	  else if(gcmd.mName == BNB_PUT_SETS)
	    ;
	  else if(gcmd.mName == BNB_GET_SETS){
	  }
          else if(gcmd.mName == BNB_GET_SOLUTIONS){
            *mAC << HEUR_SOLUTION_GET_CMD;
            *mAC << -1;
            mAC->send(mHeurMasterRank);   
           }
        }	    
	rank = mAC->probe(&msize);
	if(rank != -1)
	  mAC->recv(rank, msize);
	else {
          sleep(mSleepTime);
	  continue;
        }
      } else {
	log << "master: enter waiting state\n";
	log.write();
	mAC->recv(-1);
	log << "master: leave waiting state\n";
	log.write();      
      }

      {
	int code;
	typename ProblemFactory::ValueType rec;
	
	*mAC >> code;
	if(code == HEUR_SET_UPDATE_CMD) {
	  sv.clear();
	  *mAC >> sv;
          if(!sv.empty()) 
	   Traverse::pushSets(sv);
	} else if(code == HEUR_SOLUTION_UPDATE_CMD) {
	  StdVector <Solution> solv;
          *mAC >> solv;

          if((mGC != NULL) && (!solv.empty())) {
            tsendgate.start();
            gcmd.mName = BNB_PUT_SOLUTIONS;
            gcmd.mDsc = "\n";
            int i = 0;            
            while(true) {
              std::ostringstream os;
              os << "<solu>" << solv[i].toString() << "</solu>\n";
              gcmd.mDsc.append(os.str());
              i ++;
              if(i == solv.size())
                break;
            }
            *mGC << gcmd;
            mGC->send(mGC->getAttr(BNB_GATE_ID_ATTR));
            tsendgate.stop();
          }
          Traverse::pushSolutions(solv);
	} else if(code == HEUR_STAT_CMD) { 
	  std::string s;
	  *mAC >> s;
	  log << s;
          log.write();
	} else if(code == HEUR_FINISH_CMD) {
	  log << "master: sending stop command to branch and heuristic masters\n";
	  log.write();
	  *mAC << HEUR_STOP_CMD;
          if(mBranchMasterRank != -1)
	   mAC->drop(mBranchMasterRank);
	  mAC->send(mHeurMasterRank);
	} else if(code == HEUR_TERMINATED_CMD) {
	    term --;
	  if(term == 0)
	    break;
        } else if(code == HEUR_INCUMBENT_UPDATE_CMD) {
          Solution sol;
          *mAC >> sol;
          if(mGC) {
            BNBCommand lcmd;
            tsendgate.start();
            std::ostringstream os;
            lcmd.mName = BNB_INCUMBENT_UPDATE;
            os << "<solu>" << sol.toString() << "</solu>\n";
            lcmd.mDsc = os.str();
            lcmd.mCode = INCUMBENT_UPDATE_CODE;
            *mGC << lcmd;
            mGC->send(mGC->getAttr(BNB_GATE_ID_ATTR));
            tsendgate.stop();
          }           
          StdVector<Solution> solv;
          solv.push_back(sol);
          Traverse::pushSolutions(solv);
        } else {
          BNB_ERROR_REPORT("Urecognized command");
        }
      }
    }
#else
    BNB_ERROR_REPORT("Feature  is obsolete\n");
#endif
  }

/**
 * Master process for branching
 *
 * @return number of branches
 *
 */
  void branchMaster ()
  {
    Counter crecv ("packages from workers");
    Counter csend ("packages to workers");
    Counter csentsets ("sent sets to workers");
    Counter crecvsets ("recv sets from workers");
    Counter crecupd ("record updates from workers");
    Counter crecupdgate ("record updates from gate");

    Timer toverall ("overall time");
    Timer tbusy ("busy time");
    Timer tstarvation ("starvation time");
    Timer tupack ("unpack time");
    Timer trecv ("recieve time");
    Timer tpack ("pack time");
    Timer tsend ("send time");
    Timer tbranch ("branch time");
    BNBBranchInfo info;
    InfiniInt MBT;

    int cmd,term;
    std::deque < int >freeSlaves;
    int maxrank = 0;
    double mint = -1.;
    double maxt = -1.;
    typename ProblemFactory::ValueType oldrecord;
    bool finalizing = false;
    bool started = false;
    InfiniInt Tb, Ts;
    std::ostringstream os;
    InfiniInt maxv(0), minv, abranch(0);
    
    std::map<double, std::string> tm;
    
    BNBLog<LOGGING_OPTION> log;
    {
      std::ostringstream os;
      os << "Branch Master " << mAC->getMyId();
      log.setName(os.str());
    }
    BNBLog<LOGGING_OPTION>::setUniLog(&log);
    log << " started | " << (double)(clock())/(double)(CLOCKS_PER_SEC) << "\n";
    log.write();
    if(!mScheduler) {
      log << "Scheduler not set\n";
      log.write();
      mAC->abort(-1);
    }
    Tb = mScheduler->getSlaveCT();
    Ts = mScheduler->getSlaveChunk();
 
    toverall.start ();
    
/*    for (std::vector<int>::const_iterator i = mBranchSlaves.begin(); 
	 i != mBranchSlaves.end(); i++) {
      freeSlaves.push_back (*i);
    }    
    */
    oldrecord = Traverse::getRecord();
    
    while (1) {
	int rank;
	int msize;
        if(!finalizing) {
          while((!Traverse::mTaskQueue.empty()) && (!freeSlaves.empty())) {
            Set s;
            s = Traverse::mTaskQueue.top ();
            Traverse::mTaskQueue.pop ();
            if (!Traverse::mSetFactory->discard (s, Traverse::getRecord())) {
              int rk;
              std::vector < Set > sv;

              csend.inc ();
              csentsets.inc ();
              rk = freeSlaves.back();
              maxrank = BNBMAX (rk, maxrank);
              freeSlaves.pop_back();
              tpack.start();
              sv.push_back(s);
              if(mScheduler &&  mScheduler->activate(Traverse::mTaskQueue.size(), freeSlaves.size(), mBranchSlaves.size())){
                Tb = mScheduler->getSlaveCT();
                Ts = mScheduler->getSlaveChunk();
                log << "scheduler action performed";
                log << "Sent branch treshold = " << Tb.toString();
                log << " and chunk = " << Ts.toString() << " to "
                    << " slaves\n";
                log.write();
              }
              tpack.start();
              *mAC << HEUR_SET_UPDATE_CMD;
              *mAC << Tb;
              *mAC << Ts;
              *mAC << Traverse::getRecord();
              *mAC << sv;
              tpack.stop();
              tsend.start();
              mAC->send (rk);
              tsend.stop ();
            } else 
              info.mDiscardedByRecord ++;
          }
        }          
        if (Traverse::mTaskQueue.empty () && (freeSlaves.size () == mBranchSlaves.size()) && (!finalizing) && started) {
          log << "Detected termination condition\n";
          log.write();
          finalizing = true;
          tpack.start() ;
          *mAC << HEUR_FINISH_CMD;
          tpack.stop();
          tsend.start();
          mAC->send(mMasterRank);	 
          tsend.stop();
	}
        
        trecv.start();
        rank = mAC->recv (-1);
        trecv.stop();
        *mAC >> cmd;
        
        if(cmd == HEUR_RECORD_UPDATE_CMD) {
	  typename ProblemFactory::ValueType rec;
	  *mAC >> rec;
	  if((ProblemFactory::getProblemType() == BNB_MAXIMIZE) ? rec > Traverse::getRecord() : rec < Traverse::getRecord()) 		      
	    Traverse::setRecord(rec, &info);	  
	} else if(cmd == HEUR_SET_UPDATE_CMD){	 
	  int n;
	  StdVector < Set > sv;
	  tupack.start();
	  *mAC >> sv;
	  n = sv.size();
          if(n > 0) {
            crecv.inc ();
            crecvsets.inc (n);
            Traverse::pushSets (sv, &info);
          }
          tupack.stop();
          if(mScheduler &&  mScheduler->activate(Traverse::mTaskQueue.size(), freeSlaves.size(), mBranchSlaves.size())){
            Tb = mScheduler->getSlaveCT();
            Ts = mScheduler->getSlaveChunk();
            log << "scheduler action performed";
            log << "Sent branch treshold = " << Tb.toString();
            log << " and chunk = " << Ts.toString() << " to "
                << rank << "\n";
            log.write();
          }
          *mAC >> cmd;
          if(cmd == HEUR_SET_GET_CMD) {
            if(std::find(mBranchSlaves.begin(), mBranchSlaves.end(), rank) != mBranchSlaves.end())
              freeSlaves.push_back(rank);
          } else if(cmd == HEUR_NIL_CMD) {
            tpack.start();
            *mAC << HEUR_SET_UPDATE_CMD;
            *mAC << Tb;
            *mAC << Ts;
            *mAC << Traverse::getRecord();
            sv.clear();
            *mAC << sv;
            tpack.stop();
            tsend.start();
            mAC->send(rank);
            tsend.stop();	  
          } else {
            BNB_ERROR_REPORT("Unrecognized command\n");
          }
	} else if(cmd == HEUR_START_CMD) {        
          Set s;
          Solution sol;
	  StdVector<Set> sv;
	  StdVector<Solution> solv;
          
          log << "Received START command\n";
          log.write();
	  if(Traverse::mTaskQueue.empty()) {
	    if (Traverse::mSetFactory->getInitialSet (s)){
		sv.push_back (s);
		Traverse::pushSets(sv);
            }
	  }
          log  << "Task queue contains " << Traverse::mTaskQueue.size() 
              << " elements afer intialization\n";
          log.write();
          
	  started = true;
	  tbranch.start ();
          
	  if(mScheduler)
	    MBT = mScheduler->getMasterCT();
          if ((MBT != (InfiniInt)0) && !Traverse::mTaskQueue.empty())
	    {
	      while (1)
		{
		  bool h;
                  InfiniInt c, n;
		  n = Traverse::branch (&info);
		  c = info.mBranched + info.mSolutioned + info.mDiscardedByRecord + info.mDiscardedByConstraints;
		  if (n == 0)
		    break;
		  else if (c >= MBT)
		    break;
		}
	    }
	  tbranch.stop ();
          log  << "Task queue contains " << Traverse::mTaskQueue.size() 
              << " elements afer branching\n";
          log.write();

          tpack.start();
          Tb = mScheduler->getSlaveCT();
          Ts = mScheduler->getSlaveChunk();
          *mAC << HEUR_START_CMD;
          *mAC << Tb;
          *mAC << Ts;
          tpack.stop();
          tsend.start();
          mAC->send(mBranchSlaves);
          tsend.stop();
	} else if(cmd == HEUR_STOP_CMD) {
	  *mAC << HEUR_STOP_CMD;
	  mAC->send(mBranchSlaves);
	  term = mBranchSlaves.size();
	} else if(cmd == HEUR_TERMINATED_CMD) {
	  term --;
	  if(term == 0)
	    break;
	} else if(cmd == HEUR_STAT_CMD) {
	  double tall, trecv, tpack, tsend, tbranch;
	  InfiniInt cbranch,  crecv, csend;

	  *mAC >> tall >> trecv >> tpack >> tsend >> tbranch;
          *mAC >> cbranch;
          *mAC >> crecv;
          *mAC >> csend;
	  abranch = abranch + cbranch;
	  if (mint < 0)
	    mint = tbranch;
	  else
	    mint = BNBMIN (mint, tbranch);
	  if (maxt < 0)
	    maxt = tbranch;
	  else
	    maxt = BNBMAX (maxt, tbranch);	  
          minv = BNBMIN (minv, cbranch);
          maxv = BNBMAX (maxv, cbranch);

	  os << "[branch slave " << rank << "]"
	     << "(tall = " << tall << ","
	     << "trecv = " << trecv << ","
	     << "tpack = " << tpack << ","
	     << "tsend = " << tsend << ","
	     << "tbranch = " << tbranch << ","
	     << "branches = " << cbranch.toString() << ","
	     << "crecv = " << crecv.toString() << "," 
             << "csend = " << csend.toString() << "\n";
        } else {
          log << "RECEIVED UNRECOGNIZED COMMAND " << cmd << " from " << rank <<  "\n";
          log.write();
        }
      }
          
    toverall.stop();
    


      os << "[branch master]\n\t"
	 << toverall.toString () << "\n\t"
	 << tupack.toString () << "\n\t"
	 << trecv.toString () << "\n\t"
         << tpack.toString () << "\n\t"
	 << tsend.toString () << "\n\t"
	 << tbranch.toString () << "\n\t"
	 << tbusy.toString () << "\n\t"
	 << tstarvation.toString () << "\n\t"
	 << csend.toString () << "\n\t"
	 << csentsets.toString () << "\n\t"
	 << crecv.toString () << "\n\t"
	 << crecvsets.toString () << "\n\t"
	 << "total slave branched nodes = " << abranch.toString() << "\n\t"
	 << "slaves involved = " << maxrank - mBranchMasterRank   << "\n\t"
	 << "min:max slave time = " << mint << " : " << maxt << "\n\t"
          << "min:max slave vertices = " << minv.toString() << " : " << maxv.toString() << "\n\t"
         << "MASTER:"
	 << info.toString();

      *mAC << HEUR_STAT_CMD;
      *mAC << os.str();
      mAC->send(mMasterRank);
      *mAC << HEUR_TERMINATED_CMD;
      mAC->send(mMasterRank);
      
  }

/**
 * Slave process for branching.
 */
  void branchSlave ()
  {
    int recordUpdates = 0;
    int cmd;
    BNBBranchInfo info;
    typename ProblemFactory::ValueType oldrecord;
    Timer toverall ("overall time");
    Timer trecv ("recv time");
    Timer tsend ("send time");
    Timer tbranch ("branch time");
    Timer tidle ("idle time");
    Timer tpack("pack time");
    Counter csentportions ("sent portions");
    Counter csentsets ("sent sets");
    Counter ctaskrecv ("task received");
    InfiniInt Tb, Ts;
    BNBLog<LOGGING_OPTION> log;
    {
      std::ostringstream os;
      os << "Branch Slave " << mAC->getMyId();
      log.setName(os.str());
    }
    BNBLog<LOGGING_OPTION>::setUniLog(&log);
    toverall.start ();    
    oldrecord = ProblemFactory::absoluteBound();

    trecv.start ();
    mAC->recv (-1);
    trecv.stop ();
    *mAC >> cmd;
    if(cmd == HEUR_START_CMD) {
      *mAC >> Tb;
      *mAC >> Ts;
    } else {
      BNB_ERROR_REPORT("Unrecognized command: SHOULD be BNB_START_CMD\n");
    }     

    while (1) {
      double t;
      int rank, msize, cmd, hs;
      
      if(!Traverse::mTaskQueue.empty()) {	    
        InfiniInt c(0), n;
        oldrecord = Traverse::getRecord();

        tbranch.start ();
        while(2){
          n = Traverse::branch (&info);
          c ++;
          if ((n == 0) || (c > Tb))
            break;
        }
        tbranch.stop ();
        
        if(Traverse::getRecord() != oldrecord) {
          log << " record " << Traverse::getRecord() 
              << " improved w.r.t. " << oldrecord << " on branch slave " << mAC->getMyId() << "\n";
          log.write();
        }
        if(mSolutionUpdateFilter && mSolutionUpdateFilter->activate()) {
          std::vector <Solution> solv;
          mSolutionUpdateFilter->setupSolutionPackage(solv);
          if(!solv.empty()) {
            tpack.start();
            *mAC << HEUR_SOLUTION_UPDATE_CMD;
            *mAC << solv;
            tpack.stop();
            tsend.start();
            mAC->send(mHeurMasterRank);	      
            tsend.stop();
            log << " solution " << solv[0].getValue() << " sent to heuristic master \n";
            log.write();
          }
        }
      }
      *mAC << HEUR_SET_UPDATE_CMD;
      StdVector < Set > sv;
      if(!Traverse::mTaskQueue.empty()) {	    
        InfiniInt cnt(0);
        while ((Traverse::mTaskQueue.size() > mMinSets) && (cnt < Ts))  {
          Set s;
          s = Traverse::mTaskQueue.top ();
          Traverse::mTaskQueue.pop ();
          if (!Traverse::mSetFactory->discard (s, Traverse::getRecord())) {
            sv.push_back (s);
            cnt ++;
          }  else
            info.mDiscardedByRecord ++;	    
        }		
        csentportions.inc ();
        csentsets.inc (sv.size());
      } 
      tpack.start ();
      *mAC << sv;
      tpack.stop ();
      if(Traverse::mTaskQueue.empty()) {
        *mAC << HEUR_SET_GET_CMD; 
      } else {
        *mAC << HEUR_NIL_CMD; 
      }
      
      tsend.start ();
      mAC->send (mBranchMasterRank);
      tsend.stop ();
      trecv.start ();
      mAC->recv (-1);
      trecv.stop ();
      *mAC >> cmd;
      if(cmd == HEUR_STOP_CMD) {
        break;
      } else if(cmd == HEUR_SET_UPDATE_CMD) {        
        *mAC >> Tb;
        *mAC >> Ts;
        typename ProblemFactory::ValueType rec;
        *mAC >> rec;
        if(ProblemFactory::getProblemType() == BNB_MAXIMIZE ? rec > Traverse::getRecord() : rec < Traverse::getRecord())
          Traverse::setRecord(rec, &info);	  
        StdVector < Set > sv;
        *mAC >> sv;
        if(!sv.empty()) {
          ctaskrecv.inc ();
          Traverse::pushSets (sv, &info);
        }
      } else {
        BNB_ERROR_REPORT("Unrecognized command");
      }
    }	 
    toverall.stop ();
    *mAC << HEUR_STAT_CMD;
    *mAC << toverall.getTotalTime ()
        << trecv.getTotalTime ()
        << tpack.getTotalTime() 
        << tsend.getTotalTime ()
        << tbranch.getTotalTime ()
        << info.getTotalVerts()
        << ctaskrecv.get () 
        << csentsets.get ()
        ;
    mAC->send(mBranchMasterRank);
    *mAC << HEUR_TERMINATED_CMD;
    mAC->send(mBranchMasterRank);
  }

  /**
   * Dessiminates incumbent among branch slaves and forwards it to a master process
   */
  void dessiminateIncumbent()
  {
    BNBLog<LOGGING_OPTION> log;
    std::vector<Solution> solv;
    typename ProblemFactory::ValueType incumbent;
    incumbent = mHeurMaster->getIncumbent();
    if(mBranchMasterRank != -1) {
      log << " incumbent " << incumbent << " is dessiminated among branch slaves\n";
      log.write();	     
      *mAC << HEUR_RECORD_UPDATE_CMD;
      *mAC <<  incumbent;
      mAC->send(mBranchMasterRank);
    }
    mHeurMaster->pickBestSolution(solv);
    if(!solv.empty()) {
      *mAC << HEUR_INCUMBENT_UPDATE_CMD;
      *mAC << solv[0];
      mAC->send(mMasterRank);
      log << " incumbent " << incumbent << " sent to master\n";
      log.write();	     
    }
  }
  
/**
 * Master process for heuristic search.
 *
 */
  void heurMaster()
  {
    int term, rank, nsend;
    bool terminating = false;
    bool starting = true;
    std::ostringstream os;
    double t;
    Solution sol;
    std::vector<typename ProblemFactory::Solution> solv;
    BNBLog<BNB_NO_LOGGING> log;
    
    nsend = 0;
    os << "Heur Master " << mAC->getMyId();
    log.setName(os.str());
    BNB_ASSERT(mHeurMaster);
    mHeurMaster->setHeurSlaves(mHeurSlaves);
    t = mAC->time();
    term = mHeurSlaves.size();    
    std::deque<int> freeSlaves;
    for(int i = 0; i < mHeurSlaves.size(); i ++) 
      freeSlaves.push_back(mHeurSlaves[i]);            
    while(1) {
      int cmd;                          
             
      if((!terminating) && (!starting) &&
          (freeSlaves.size() == mHeurSlaves.size()) &&
          (mBranchMasterRank == -1) &&
          mHeurMaster->isSolutionListEmpty()) 
      {
        *mAC << HEUR_FINISH_CMD;
        mAC->send(mMasterRank);	         
      }
      rank = mAC->recv (-1);
      *mAC >> cmd;      
      if(cmd == HEUR_START_CMD) {
        starting = false;
        log << " initial incumbent: " << mHeurMaster->getIncumbent() << " obtained at " << mAC->time() - t << " \n";
        log.write();
        dessiminateIncumbent();
      } else if(cmd == HEUR_RECORD_UPDATE_CMD) {
        BNB_ERROR_REPORT("Not supported record update");
      } else if(cmd == HEUR_STOP_CMD) {
        if(term == 0)
          break;
	log << " sending stop command to slaves\n";
	log.write();	
        for(std::deque<int>::const_iterator i = freeSlaves.begin();
            i != freeSlaves.end();
            i++) {
              *mAC << HEUR_STOP_CMD;
              mAC->send(*i);
        }
	terminating = true;
      } else if(cmd == HEUR_STAT_CMD) {
	std::string s;
	*mAC >> s;
	os << s << '\n';	
      } else if(cmd == HEUR_TERMINATED_CMD){
	term --;
	if(term == 0)
	  break;
      } else if(cmd == HEUR_PING_CMD) {
        if(terminating) {
          *mAC << HEUR_STOP_CMD;
        } else {
          *mAC << HEUR_PING_CMD;
        }
        mAC->send(rank);
      } else if(cmd == HEUR_NIL_CMD) {
        if(terminating) {
          *mAC << HEUR_STOP_CMD;
          mAC->send(rank);
        } else {
	  freeSlaves.push_back(rank);
        }
      } else if(cmd == HEUR_SOLUTION_UPDATE_CMD) {
        bool flag = false;	
        typename BNBHeurMaster<ProblemFactory>::PutStages stage;
        if(!terminating) {
          if(rank == mMasterRank) {
            stage = BNBHeurMaster<ProblemFactory>::FromMaster;
          } else if(std::find(mHeurSlaves.begin(), mHeurSlaves.end(), rank) != mHeurSlaves.end()) {
            stage = BNBHeurMaster<ProblemFactory>::FromHeur;
          } else {
            stage = BNBHeurMaster<ProblemFactory>::FromBNB;
          }
          std::vector < Solution > sv;
          *mAC >> sv;
          log << sv.size() << " solutions received from " << rank << "\n";
          log.write();
          if(mHeurMaster->pushSolutions(sv, stage)) {
            log << " record " << mHeurMaster->getIncumbent() << " improved at " << mAC->time() - t << " \n";
            log << (terminating ? "terminating" : "no-terminating") << "\n";
            log << (starting ? "starting" : "no-starting");
            log.write(); 
            if((!terminating) && (!starting))
              dessiminateIncumbent();
          }
          log << " solutions received from " << rank << "put to heur master\n";
          log.write();
        }
      } else if(cmd == HEUR_SOLUTION_GET_CMD) {
        int num;
        std::vector<Solution> solv;
        *mAC >> num;
        mHeurMaster->getSolutions(num, solv);
        *mAC << HEUR_SOLUTION_UPDATE_CMD;
        *mAC << solv;	
        mAC->send(mMasterRank);
      } 
      if(terminating)
	continue;
      while(true) {
        std::vector<Solution> sv;
        int s;
        if(mHeurMaster->findBestMatch(freeSlaves, sv, s)) {
          log << "sending " << sv.size() << " solutions to slave " << s << "\n";
          log << "totally " << nsend ++ << " sends\n";
          //log << mHeurMaster->toString() << "\n";
          log.write();
          *mAC << HEUR_SOLUTION_UPDATE_CMD;
          *mAC << sv;	
          mAC->send(s);
        } else
          break;
      }
    } 
    if(!mHeurMaster->pickBestSolution(solv)){
      BNB_ERROR_REPORT("NO SOLUTION FOUND!\n");
    } else {
      log << "sent to master best solution " << solv[0].getValue() << "\n";
      log.write();
    }
    *mAC << HEUR_SOLUTION_UPDATE_CMD;
    *mAC << solv;
    mAC->send (mMasterRank);
    *mAC << HEUR_STAT_CMD;
    *mAC << os.str();
    mAC->send(mMasterRank);
    *mAC << HEUR_TERMINATED_CMD;
    mAC->send(mMasterRank);
    log << " terminated | "<< mAC->time()  - t <<"\n";
    log.write();
  }
  
/**
 * Slave process for heuristic search.
 *
 */
  void heurSlave()
  {
    Counter crecv("solutions received");
    Counter csend("solutions sent");
    Counter cimpr("record improvements");
    Timer tidle("idle");
    Timer twork("work");
    std::string name;
    std::ostringstream os;
    std::vector <Solution> sv;
    BNBLog <BNB_NO_LOGGING> log;
    BNB_ASSERT(mHeurSlave);
    {
      std::ostringstream os;
      os << "Heuristic Slave " << mAC->getMyId();
      log.setName(os.str());
    }
    log << " started\n";
    log.write();   

    mHeurSlave->setup(mAC->getMyId() - mHeurMasterRank -1, mHeurSlaves.size());    
    while(1) {
      log << " enter wating state\n";
      log.write();
      tidle.start();
      mAC->recv (-1);
      tidle.stop();
      log << name << " leave wating state\n";      
      log.write();
      int cmd;
      *mAC >> cmd;
      if(cmd == HEUR_STOP_CMD) {
       break;
      } else if(cmd == HEUR_SOLUTION_UPDATE_CMD) {
        sv.clear();
        *mAC >> sv;   
        BNB_ASSERT(!sv.empty());
        crecv.inc();
        mHeurSlave->setIterCounter(0);
      } else if(cmd == HEUR_PING_CMD) {
        /* DO NOTHING */
      } else {
        BNB_ERROR_REPORT("Unrecognized command");
      }
        
      BNB_ASSERT(!sv.empty());
      twork.start();
      bool rv = mHeurSlave->step(sv);
      twork.stop();

      if(rv){
        if(sv.size()) {
          *mAC << HEUR_SOLUTION_UPDATE_CMD;
          *mAC << sv;
          mAC->send(mHeurMasterRank);
        }
        *mAC << HEUR_NIL_CMD;	
        mAC->send(mHeurMasterRank);	        
      } else {
        int r;
        int cmd;
        int ms;
        *mAC << HEUR_PING_CMD;	
        mAC->send(mHeurMasterRank);	        
      }
    }
    {
      std::ostringstream os;
      os << log.getName();
      os << ",";
      os << crecv.toString();
      os << ",";
      os << csend.toString();
      os << ",";
      os << cimpr.toString();      
      os << ",";
      os << tidle.toString();
      os << ",";
      os << twork.toString();
      *mAC << HEUR_STAT_CMD;
      *mAC << os.str();
      mAC->send(mHeurMasterRank);
    }
    *mAC << HEUR_TERMINATED_CMD;
    mAC->send(mHeurMasterRank);
    log << name << " terminated\n";
    log.write();
  }

/**
 * A pointer to the abstract communicator
 */
  AbstractCommunicator *mAC;


/**
 * Pointer to the communicator for interacting with the gate
 */
  AbstractCommunicator *mGC;


/**
 * The "supermaster" rank
 */
  int mMasterRank;

/**
 * The rank of the master process for branching.
 */
  int mBranchMasterRank;

/**
 * The rank of the master process for heuristic search.
 */
  int mHeurMasterRank;

  /**
   * Processes for branching.
   */
  std::vector<int> mBranchSlaves;

  /**
   * Processes for heuristic search.
   */
  std::vector<int> mHeurSlaves;


  
  /**
   * How often (number of branches) to check for pending messages 
   */
  int mCheckRatio;

  /**
   * How often (number of loops) branch slave send solution to heursitic master
   */
  int mRecordUpdatesRatio;
  
  /**
   * Number of initial solutions
   */
  int mNumberOfInitialSolutions;

  /**
   * Scheduler pointer
   */
  AbstractScheduler* mScheduler;

  /**
   * Pointer to the solution update filter
   */
  AbstractSolutionUpdateFilter<ProblemFactory> *mSolutionUpdateFilter;
  
  /**
   * Encapsulates the functionality of the heuristic master
   */
  BNBHeurMaster<ProblemFactory>* mHeurMaster;
  
  /**
   * Encapsulates the functionality of the heuristic slave
   */
  BNBHeurSlave<ProblemFactory>* mHeurSlave;
  
  /**
   * Solution generator
   */
  AbstractSolutionGenerator<ProblemFactory>* mSolutionGenerator;
  
  /**
   * Timeout to wait for supervisor
   */
  int mSleepTime;

  /**
    * Minimal number of sets on the branch slave to leave
    */
  static const  int mMinSets = 1;
};
#endif
