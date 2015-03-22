#ifndef _CLMS_HPP_
#define _CLMS_HPP_
/**
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file clms.hpp
 *
 * Classical master-slave scheme with record update.
 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h>
#include <string>
#include <queue>
#include <sstream>
#include <map>

#include <util/common/util.hpp>
#include "bnb.hpp"
#include <util/aclib/aclib.hpp>
#include <ei/external/bnbcommand.hpp>
#include <ei/external/bnbcommpi.hpp>
#include <ei/external/bnbprotocol.hpp>
#include <ei/external/bnbgate.hpp>
#include "bnbdiag.hpp"
#include <util/common/bnblog.hpp>
#include <util/common/infinipar.hpp>
#include <util/common/stdvector.hpp>
#include <util/common/vec.hpp>

#define DEFAULT_ADAPT_STRATEGY_STEP 128
#define MAX_HISTORY_LENGTH 1024

#define PROBE_CHECK_RATIO 64


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
 * Stop solving
 */
#define CLMS_STOP_CMD 1

/**
 * New record arrived
 */
#define CLMS_RECORD_UPDATE_CMD 2

/**
 * New sets arrived
 */
#define CLMS_SET_UPDATE_CMD 3

/**
 * New solutions arrived
 */
#define CLMS_SOLUTION_UPDATE_CMD 4

/**
 * Flush sets from slaves to the master
 */
#define CLMS_FLUSH_CMD 5

/**
  * Update information (tresholds etc.)
  */
#define CLMS_INFO_UPDATE_CMD 6


/**
 * Main class for BNB solver.
 *
 * This class is aimed at branch-and-bound solving integer programming
 * problems. The solution is found by iterative refining of problem
 * space.  
 * 
 * The class maintains two ordered queues: task queue and solution queue. 
 * Solving starts from task queue consisting of the initial set. On
 * each step top set from the task queue is removed and divided 
 * into a number of subsets. The process is finished when all leave 
 * subsets contains one element. 
 *
 * The subset is discarded if the best objective value found by the 
 * moment is better then the best estimation for this subset, i.e. 
 * the top of the solution queue is better then this subset.  
 * 
 * This class has two template parameters:
 *   SetFactory - class for creating search tree nodes
 *   Traverse   - class resposible for branching strategy
 *    
 *   @attention It is supposed that SetFactory::Set instances are copied as a whole, 
 *    i.e. if it contains pointer to data 
 *    objects then these data objects are copied with the set. This approach can be a bit expensive 
 *    because BNBSolver extensively copies sets from one data structure to another. Alternative
 *    feasible approach is provided by smart pointers with reference counting: when copiing only 
 *    the pointer to a data structure is copied and the reference counter is increased, the memory
 *    is released when the counter drops to zero.
 *
 * @author \b Mikhail \b Posypkin 
 * The author can be reached at posypkin@ispras.ru
 *
 * @warning Using this library requires an explicit permition from author. 
 */
template < class Traverse > class BNBClmsSolver:public
Traverse, public BNBSolver
{
public:

  typedef typename Traverse::Factory::Set Set;

  typedef typename Traverse::Factory::Solution Solution;
  
  typedef typename Traverse::Factory SetFactory;
  
   /**
   * Option values
    */
  struct DebugOptions {
    static const unsigned int NO_DEBUG = 0;
    static const unsigned int PRINT_RESULTING_STAT = (1 << 1);
    static const unsigned int PRINT_MAIN_STEPS = (1 << 2);
  };



/**
 * The constructor
 *
 * @param gac communicator for interaction with the gate process
 *
 * @param ac abstract communicator for internal comunications
 *
 * @param fact                 problem-specific factory
 *
 * @param masterBranchTreshold maximal number of branches to perform on the master before start 
 *                             dessiminating sets to slaves (-1 for infinite)
 *             
 * @param masterSetTreshold    maximal number of sets to generated on the master before start
 *                             dessiminating sets to slaves (-1 for infinite)
 *
 * @param slaveBranchTreshold  maximal number of branches to perform on the slave before start
 *                             sending sets to master
 *
 * @param slaveSetTreshold     maximal number of sets to generate on the slave before start 
 *                             sending sets to master
 *
 * @param recordUpdate         whether to perform record broadcast as soon as record is generated
 *
 * @param stop                 if true then stop after performing branchin on the master
 */
  BNBClmsSolver (AbstractCommunicator* gac, 
		 AbstractCommunicator * ac, 
		 typename Traverse::Factory * fact) : 
    Traverse (fact)
  {
    mGC = gac;
    mAC = ac;
    mRecordUpdate = true;
    mStop = false;
    mDebugOptions = DebugOptions::NO_DEBUG;
    mTotalVerts = 0;
  }

/**
 * The reduced version of the constructor.
 */
  BNBClmsSolver (AbstractCommunicator * ac, 
		 typename Traverse::Factory * fact):
  Traverse (fact)
  {
    mGC = NULL;
    mAC = ac;
    mRecordUpdate = true;
    mStop = false;
    mDebugOptions = DebugOptions::NO_DEBUG;
    mTotalVerts = 0;
  }

  /**
   * Setup master branch treshold
   * @param tresh treshold value
   */
  void setMasterBranchTreshold(InfiniInt tresh)
  {
    mMasterBranchTreshold = tresh;
  }

  /**
   * Setup set branch treshold
   * @param tresh treshold value
   */
  void setMasterSetTreshold(InfiniInt tresh)
  {
    mMasterSetTreshold = tresh;
  }
  
  /**
   * Set slave branch treshold
   * @param tresh  treshold value
   */
  void setSlaveBranchTreshold(InfiniInt tresh)
  {
    mSlaveBranchTreshold = tresh;
  }
  
  /**
   * Set slave set treshold
   * @param tresh 
   */
  void setSlaveSetTreshold(InfiniInt tresh)
  {
    mSlaveSetTreshold = tresh;
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

    if (r == 0)
      {
	master ();
      }
    else if (r != -1)
      {
	slave ();
      }
  }

   /**
   * Set solver configuration
   */
 void configure(std::string config)
  {
    std::map < std::string, std::string >solveData;
    const std::string left = "left";
    const std::string right = "right";
    std::string mode = left;
    std::string sl;
    std::string sr;
   
    for(int i = 0; i < config.size(); i ++)
      {
	char c;
	
	c = config[i];
	if(c == ' ') {
	} else if(mode == left) {
	  if(c == '=') 
	    mode = right;
	  else
	      sl += c;
	} else if(mode == right) {
	  if(c != '\n')
	    sr += c;
	  if((c == '\n') || (c == ':') || (i == (config.size() - 1))) {
	    solveData[sl] = sr;
	    sl = sr = "";
	    mode = left;
	    } 
	}	    
      }
      std::map < std::string, std::string >::iterator i;
      printf("Solver data:\n");
      for (i = solveData.begin (); i != solveData.end (); i++)
	printf ("(%s) = (%s)\n", i->first.c_str (), i->second.c_str());
      fflush(stdout);

      for(i = solveData.begin(); i != solveData.end(); i ++) {
	if(i->first == "MVT")
	  mMasterSetTreshold = atoi(i->second.c_str());
	else if(i->first == "MCPT")
	  mMasterBranchTreshold = atoi(i->second.c_str());
	else if(i->first == "SVT")
	  mSlaveSetTreshold = atoi(i->second.c_str());
	else if(i->first == "SCPT")
	  mSlaveBranchTreshold = atoi(i->second.c_str());
	else if(i->first == "RecordUpdate")
	  mRecordUpdate = (i->second == "true");
	else if(i->first == "Stop")
	  mStop = (i->second == "true");				    
      }
  }

   /**
   * Retrieve the total complexity
   * @return complexity
    */
  InfiniInt getTotalVerts()
  {
    return mTotalVerts;
  }
  
  /**
   * Get debugging options
   * @return reference to debugging options 
   */
  unsigned int& getDebugOptions()
  {
    return mDebugOptions;
  }


private:

/**
 * Master process
 *
 * @return number of branches
 *
 */
  void master ()
  {
    Counter crecv ("packages from workers");
    Counter csend ("packages to workers");
    Counter csentsets ("sent sets to workers");
    Counter crecvsets ("recv sets from workers");
    Counter crecupd ("record updates from workers");
    Counter crecupdgate ("record updates from gate");
    Counter crecvgate("packages received from gate");
    Counter crecvsetsgate("sets received from gate");
    Counter csentgate("packages sent to gate");
    Counter csentsetsgate("sets sent to gate");

    Timer toverall ("overall time");
    Timer tbusy ("busy time");
    Timer tstarvation ("starvation time");
    Timer trecv ("recieve from workers time");
    Timer trecvgate("receive from gate time"); 
    Timer tsend ("send to workers time");
    Timer tsendgate("send to gate time");
    Timer tbranch ("branch time");
    BNBBranchInfo info;
    BNBLog<BNB_DO_LOGGING> log;
    int cmd;
    InfiniInt n;
    InfiniInt c;
    /*
    Set s;
    Solution sol;
    */
    StdVector < Set > gsv;
    StdVector < Solution > gsolv;
    std::deque < int >freeSlaves;
    int BTHistory[MAX_HISTORY_LENGTH];
    int BTHistoryCount = 0;
    int maxrank = 0;
    InfiniInt minb, maxb(0);
    double mint = -1.;
    double maxt = -1.;
    typename Traverse::Factory::ValueType oldrecord;
    BNBCommand gcmd;
    bool stoping;
    char *slavesInfoUpdated = new char[mAC->getProcessNumber()];
    VecUtils::vecSet(mAC->getProcessNumber(), (char)0, slavesInfoUpdated);
  
    log.setName("Master process");
    stoping = mStop;

    if(mDebugOptions & DebugOptions::PRINT_MAIN_STEPS) {
      log << "Started...\n";
      log.write();
    }
    toverall.start ();



    if(mDebugOptions & DebugOptions::PRINT_MAIN_STEPS) {
      log << "Before asking GC\n";
      log.write(); 
    }
    if(mGC) {
      trecvgate.start();
      mGC->recv(-1);
      *mGC >> gcmd;
      crecvgate.inc();
      if(gcmd.mName == BNB_PUT_SETS) {
	if(gcmd.mDsc == std::string(BNB_TOP)) {
	  Set s;
	  if (Traverse::mSetFactory->getInitialSet (s))
	    gsv.push_back (s);
      
	  Solution sol;
	  if (Traverse::mSetFactory->getInitialSolution (sol))
	    gsolv.push_back (sol);	  
	} else {
          if(mDebugOptions & DebugOptions::PRINT_MAIN_STEPS) {
           log << "Before parsing sets\n";
           log.write(); 
          }
	  crecvsetsgate.inc(Traverse::mSetFactory->constructFromString(gcmd.mDsc, gsv));
          if(mDebugOptions & DebugOptions::PRINT_MAIN_STEPS) { 
            log << "After parsing sets\n";
            log.write(); 
          }
	}
      }
      trecvgate.stop();
    } else {
      Set s;
      if (Traverse::mSetFactory->getInitialSet (s))
	gsv.push_back (s);
      
      Solution sol;
      if (Traverse::mSetFactory->getInitialSolution (sol))
	gsolv.push_back (sol);
    }
    if(mDebugOptions & DebugOptions::PRINT_MAIN_STEPS) {
      log<< "Before pushing sets\n";
      log.write();
    }
    Traverse::pushSets (gsv, &info);
    if(mDebugOptions & DebugOptions::PRINT_MAIN_STEPS) {
      log<< "Before pushing solutions\n";
      log.write();
    }
    Traverse::pushSolutions (gsolv, &info);
    if(mDebugOptions & DebugOptions::PRINT_MAIN_STEPS) {
      log<< "Before branch\n";
      log.write();
    }
    tbranch.start ();
    c = 0;
    if ((mMasterBranchTreshold != 0) && (mMasterSetTreshold != 0))
      {
	while (1)
	  {
	    bool h;

	    n = Traverse::branch (&info);
            ++ c;
	    if (n == 0)
	      break;
	    else if (n >= mMasterSetTreshold)
	      break;
	    else if (c >= mMasterBranchTreshold)
	      break;
	  }
      }
    mTotalVerts = info.getTotalVerts();
    tbranch.stop ();
    log << "after master branch " << Traverse::getCandidateProblemsList().size()
        << " sets  gnerated\n" ;
    log.write();
    
    log << Traverse::getSolutionContainer()->top().toString() << "\n";
    log.write();

    for (int i = 1; i < mAC->getProcessNumber (); i++)
      freeSlaves.push_back (i);

    oldrecord = Traverse::getRecord();

    while (1)
      {

	bool starvate;
	bool busy;

	int rank;
	int msize;
	std::vector < Set > sv;

	starvate = false;
	busy = false;
	
	if(((SetFactory::getProblemType() == BNB_MAXIMIZE) ? oldrecord < Traverse::getRecord() : oldrecord > Traverse::getRecord()) && mRecordUpdate) {
	  crecupd.inc();
	  tsend.start();
	  *mAC << CLMS_RECORD_UPDATE_CMD;
	  *mAC << Traverse::getRecord();
	  for(int i = 1; i < mAC->getProcessNumber(); i ++)
	    {
	      mAC->transfer(i, AbstractCommunicator::async);
	    }
	  mAC->empty();
	  oldrecord = Traverse::getRecord();
	  tsend.stop();
	}

	if (Traverse::mTaskQueue.empty ()) {
	  if (freeSlaves.size () == (mAC->getProcessNumber () - 1))
	    break;
	  else if (freeSlaves.empty ())
	    busy = true;
	  else
	    starvate = true;
	} else if(stoping && (freeSlaves.size () == (mAC->getProcessNumber () - 1))) {
	  break;
	} else if (freeSlaves.empty ()){
	  busy = true;
	} else if(!stoping){
	  Set s;	  
	  s = Traverse::mTaskQueue.top ();
	  Traverse::mTaskQueue.pop ();
	  
	  if (!Traverse::mSetFactory->discard (s, Traverse::getRecord())) {
	    int rank;
	    std::vector < Set > sv;
	    
	    csend.inc ();
	    csentsets.inc ();

	    tsend.start ();
	    rank = freeSlaves.front ();
	    maxrank = BNBMAX (rank, maxrank);
	    freeSlaves.pop_front ();
	    if(!slavesInfoUpdated[rank]) {
	      *mAC << CLMS_INFO_UPDATE_CMD;
	      *mAC << mSlaveBranchTreshold;
	      *mAC << mSlaveSetTreshold;
	      mAC->send (rank);
	      slavesInfoUpdated[rank] = 1;
	    }
	    sv.push_back (s);
	    *mAC << CLMS_SET_UPDATE_CMD;
	    *mAC << sv;
	    *mAC << Traverse::getRecord();	    
	    mAC->send (rank);
	    tsend.stop ();
	  } else 
	    info.mDiscardedByRecord ++;
	}

	if (busy == true)
	  tbusy.start ();
	else if (starvate == true)
	  tstarvation.start ();

	if(mGC) {
	  for(int i = 0; i < 16; i ++) {
	    rank = mGC->probe(&msize);
	    if(rank != -1)
	      break;
	  }
	  if(rank != -1) {
	    trecvgate.start();
	    mGC->recv (rank, msize);
	    trecvgate.stop();
	    *mGC >> gcmd;
	    if(gcmd.mName == BNB_RECORD_UPDATE)
	      ;
	    else if(gcmd.mName == BNB_PUT_SETS){
	      StdVector<Set> sv;
	      crecvsetsgate.inc(Traverse::mSetFactory->constructFromString(gcmd.mDsc, sv));
	      Traverse::pushSets (sv, &info);
	    }
	    else if(gcmd.mName == BNB_STOP) {
	      stoping = true;
              if(mDebugOptions & DebugOptions::PRINT_MAIN_STEPS) {
	       log << "BNB_STOP command received from gate";
	       log.write();
              }
	      for(int i = 1; i < mAC->getProcessNumber(); i ++) {
		*mAC << CLMS_FLUSH_CMD;
		mAC->send(i);
	      }
	    } else if(gcmd.mName == BNB_GET_SETS){
	      int n;
	      StdVector<Set> sv;

	      tsendgate.start();
	      n = atoi(gcmd.mDsc.c_str());
	      
	      while((!Traverse::mTaskQueue.empty()) && (n != 0)) {
		sv.push_back(Traverse::mTaskQueue.top());
		Traverse::mTaskQueue.pop();
		n --;
	      }
	      
	      csentgate.inc();
	      csentsetsgate.inc(sv.size());
	      gcmd.mName = BNB_PUT_SETS;
	      if(sv.empty()) 
		gcmd.mDsc = BNB_NIL;
	      else
		Traverse::mSetFactory->serializeToString(gcmd.mDsc, sv);
	      *mGC << gcmd;
	      mGC->send(mGC->getAttr(BNB_GATE_ID_ATTR));	      
	      tsendgate.stop();
	    } else if(gcmd.mName == BNB_GET_LOAD) {
	      gcmd.mName = BNB_LOAD_INFO;
	      std::ostringstream os;
	      os << Traverse::mTaskQueue.size();
	      gcmd.mDsc = os.str();
	      *mGC << gcmd;
	      mGC->send(mGC->getAttr(BNB_GATE_ID_ATTR));	      
	    }
	  }	    
	}

	rank = mAC->probe (&msize);
	if (busy == true)
	  tbusy.stop ();
	else if (starvate == true)
	  tstarvation.stop ();
	if (rank == -1)
	  continue;



	trecv.start ();

	rank = mAC->recv (rank, msize);
	*mAC >> cmd;
	if(cmd == CLMS_RECORD_UPDATE_CMD) {
	  typename SetFactory::ValueType rec;

	  *mAC >> rec;
	  if((SetFactory::getProblemType() == BNB_MAXIMIZE) ? (rec > Traverse::getRecord()) : (rec < Traverse::getRecord ())) {
            log << "Record updated: " << rec << "\n";
            log.write();
	    Traverse::setRecord(rec, &info);
          }
	  
	} else {	 
	  StdVector<Set> sv;
	  *mAC >> sv;
	  crecv.inc ();
	  crecvsets.inc (sv.size ());
	  freeSlaves.push_front (rank);
	  Traverse::pushSets (sv, &info);
	} 
	trecv.stop ();
      }


    if(mGC) {
      tsendgate.start();
      std::ostringstream os;
      gcmd.mName = BNB_RECORD_UPDATE;
      os << Traverse::getRecord();
      gcmd.mDsc = os.str();
      *mGC << gcmd;
      mGC->send(mGC->getAttr(BNB_GATE_ID_ATTR));
    }
    
    int fs;
    fs = freeSlaves.size ();

    tsend.start ();
    while (!freeSlaves.empty ()) {
      int r;
      std::vector < Set > sv;
      
      r = freeSlaves.front ();
      freeSlaves.pop_front ();
      
      *mAC << CLMS_STOP_CMD;
      mAC->send (r);      
    }
    tsend.stop ();
    toverall.stop ();
    while (fs > 0)
    {
      int r;
      double tall;
      double trecv;
      double tpack;
      double tsend;
      double tbranch;
      InfiniInt cbranch;
      InfiniInt crecv;
      InfiniInt csend;
      StdVector <Solution> solv;

      r = mAC->recv (-1, 0);
      *mAC >> solv;
      Traverse::pushSolutions(solv, &info);
      *mAC >> tall >> trecv >> tpack >> tsend >> tbranch >> cbranch >> crecv >> csend;
      mTotalVerts = mTotalVerts + cbranch; 
      if (mint < 0)
        mint = tbranch;
      else
        mint = BNBMIN (mint, tbranch);
      if (maxt < 0)
        maxt = tbranch;
      else
        maxt = BNBMAX (maxt, tbranch);
      minb = BNBMIN (minb, cbranch);
      maxb = BNBMAX (maxb, cbranch);
      if(mDebugOptions & DebugOptions::PRINT_RESULTING_STAT) {
        log << "[slave " << r << "]"
            << "(tall = " << tall << ","
            << "trecv = " << trecv << ","
            << "tpack = " << tpack << ","
            << "tsend = " << tsend << ","
            << "tbranch = " << tbranch << ","
            << "vertices = " << cbranch.toString() << ","
            << "crecv = " << crecv.toString() << "," 
            << "csend = " << csend.toString() << "\n";
        log.write();
      }
      fs --;
    }
    
    if(mDebugOptions & DebugOptions::PRINT_RESULTING_STAT) {
      log << "[master (clms)]\n\t"
          << toverall.toString () << "\n\t"
          << trecv.toString () << "\n\t"
          << trecvgate.toString () << "\n\t"
          << tsend.toString () << "\n\t"
          << tsendgate.toString () << "\n\t"
          << tbranch.toString () << "\n\t"
          << tbusy.toString () << "\n\t"
          << tstarvation.toString () << "\n\t"
          << csend.toString () << "\n\t"
          << csentsets.toString () << "\n\t"
          << csentgate.toString () << "\n\t"
          << csentsetsgate.toString () << "\n\t"
          << crecv.toString () << "\n\t"
          << crecvsets.toString () << "\n\t"
          << crecvgate.toString () << "\n\t"
          << crecvsetsgate.toString () << "\n\t"
          << crecupd.toString() << "\n\t"
          << crecupdgate.toString() << "\n\t"	 
          << "total vertices = " << mTotalVerts.toString() << "\n\t"
          << "slaves involved = " << maxrank << "\n\t"
          << "min:max slave time = " << mint << " : " << maxt << "\n\t"
          << "min:max slave vertices = " << minb.toString() << " : " << maxb.toString() << "\n\t"
          << "MASTER:\n"
          << info.toString();
      log.write();
    } 
    if(mGC) {
      if(!Traverse::mTaskQueue.empty()) {	    
	tsendgate.stop();
	StdVector<Set> sv;
        if(mDebugOptions & DebugOptions::PRINT_MAIN_STEPS) {
	 log << "Start packing sets ...\n";
	 log.write();
        }
	tsendgate.start();
	while(!Traverse::mTaskQueue.empty()) {
	  sv.push_back(Traverse::mTaskQueue.top());
	  Traverse::mTaskQueue.pop();
	}
	csentgate.inc();
	csentsetsgate.inc(sv.size());
	gcmd.mName = BNB_PUT_SETS;
	Traverse::mSetFactory->serializeToString(gcmd.mDsc, sv);
	*mGC << gcmd;
	mGC->send(mGC->getAttr(BNB_GATE_ID_ATTR));	      
	tsendgate.stop();      
        if(mDebugOptions & DebugOptions::PRINT_MAIN_STEPS) {
	 log << "Stop sending sets\n";
	 log.write();
        }
      }
    }
  }

/**
 * Slave process.
 *
 *
 * @return number of branches
 *
 */
  void slave ()
  {
    Timer toverall ("overall time");
    Timer trecv ("recv time");
    Timer tsend ("send time");
    Timer tbranch ("branch time");
    Timer tidle ("idle time");
    Timer tpack("pack time");
    bool flush = false;
    InfiniInt csentsets (0);
    InfiniInt ctaskrecv (0);

    BNBLog<BNB_DO_LOGGING> log;
    BNBBranchInfo info;
    {
      std::ostringstream os;
      os << "Branch slave " << mAC->getMyId();
      log.setName(os.str());
    }
    toverall.start ();    

    while (1)
      {
	double t;
	int rank, msize, cmd;
        InfiniInt n, c;
	StdVector < Set > sv;
	StdVector < Solution > solv;


	typename SetFactory::ValueType oldrecord;

	trecv.start ();
	mAC->recv (0, 0);
	*mAC >> cmd;
	if(cmd == CLMS_STOP_CMD) {
	  break;
	} else if(cmd == CLMS_RECORD_UPDATE_CMD) {
	  typename SetFactory::ValueType rec;
	  *mAC >> rec;
	  if((SetFactory::getProblemType() == BNB_MAXIMIZE) ? rec > Traverse::getRecord() : rec < Traverse::getRecord())
	    Traverse::setRecord(rec, &info);
	  continue;
	} else if(cmd == CLMS_SET_UPDATE_CMD){
	  typename SetFactory::ValueType rec;
	  *mAC >> sv;
	  ctaskrecv ++;
	  Traverse::pushSets (sv, &info);
	  *mAC >> rec;
	  Traverse::setRecord(rec, &info);
	} else if(cmd == CLMS_INFO_UPDATE_CMD){
	  *mAC >> mSlaveBranchTreshold;
	  *mAC >> mSlaveSetTreshold;
	  continue;
	} else {
	  continue;
	}
	trecv.stop ();
	tbranch.start ();
	c = 0;
	oldrecord = Traverse::getRecord();
	while (1)
	  {
	    int ms;
	    int r = -1;
	    if(mRecordUpdate) {
	      {
		static int count = 0;
		if(((++ count) % PROBE_CHECK_RATIO) == 0) 
		  r = mAC->probe(&ms);
	      }
	      if(r != -1) {
		typename SetFactory::ValueType rec;
		mAC->recv(r, ms);
		*mAC >> cmd;
		if(cmd == CLMS_RECORD_UPDATE_CMD) {
		  *mAC >> rec;
		  if((SetFactory::getProblemType() == BNB_MAXIMIZE) ? rec > Traverse::getRecord() : rec < Traverse::getRecord()){
		    oldrecord = rec;
		    Traverse::setRecord(rec, &info);
		  }
		} else if(cmd == CLMS_FLUSH_CMD) {
		  flush = true;
		  log << "Start flushing\n" ;
		  log.write();
		} 
	      }
	    }
	    
	    n = Traverse::branch (&info);
	    ++ c;
	    if((Traverse::getRecord() != oldrecord) && mRecordUpdate) {
	      *mAC << CLMS_RECORD_UPDATE_CMD;
	      *mAC << Traverse::getRecord();
	      mAC->send(0);
	      oldrecord = Traverse::getRecord();
	    }
	    if (n == 0)
	      break;
	    else if (c > mSlaveBranchTreshold)
	      break;
	    else if (n > mSlaveSetTreshold)
	      break;
	    else if(flush)
	      break;
	  }
	tbranch.stop ();
	while (!Traverse::mTaskQueue.empty ())
	  {
	    Set s;
	    s = Traverse::mTaskQueue.top ();
	    Traverse::mTaskQueue.pop ();
	    if (!Traverse::mSetFactory->discard (s, Traverse::getRecord()))
	      sv.push_back (s);
	    else
	      info.mDiscardedByRecord ++;	    
	  }
        InfiniInt tmp(sv.size());
        csentsets = csentsets + tmp;
	tpack.start ();
	*mAC << CLMS_SET_UPDATE_CMD;
	*mAC << sv;
	tpack.stop ();
	tsend.start ();
	t = mAC->time ();
	mAC->send (0);
	t = mAC->time () - t;
	tsend.stop ();
      }
    toverall.stop ();
    StdVector<Solution> solv;
    if(!(Traverse::getSolutionContainer()->empty())) {
      solv.push_back (Traverse::getSolutionContainer()->top());
      //Traverse::getSolutionContainer()->pop();
    }
    *mAC << solv;
    *mAC << toverall.getTotalTime ()
        << trecv.getTotalTime ()
        << tpack.getTotalTime() 
        << tsend.getTotalTime ()
        << tbranch.getTotalTime ()
        << info.getTotalVerts()
        << ctaskrecv 
        << csentsets;
    mAC->send (0);
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
 * Maximal amount of branches slave perfroms before start sending sets to the master.
 */
  InfiniInt mSlaveBranchTreshold;
/**
 * Maximal amount of sets allowed for generating before slave starts sending sets to the master.
 */
  InfiniInt mSlaveSetTreshold;
/**
 * Maximal amount of branches to perform on the master before start dessiminating sets to slaves.
 */
  InfiniInt mMasterBranchTreshold;
/**
 * Maximal amount of sets to generate on the master before starting dessiminating of sets among slaves.
 */
  InfiniInt mMasterSetTreshold;
/**
 * Perform or not record update
 */
  bool mRecordUpdate;


/**
 * Stop or not after performed requested branching on the master
 */   
  bool mStop;

  /**
   * Options
   */
  unsigned int mDebugOptions;
  
  /**
   * The total number of vertices
   */
  InfiniInt mTotalVerts;
  
};

#endif
