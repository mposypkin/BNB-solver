#ifndef __PROBE__H__
#define __PROBE__H__


#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>
#include <string>
#include <queue>
#include <sstream>

#include "bnb.hpp"
#include <util/aclib/aclib.hpp>


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
 *   Set - class for main set data
 *   SetFactory - class for creating instances of tSet
 *    
 * \b Set is supposed to provide the following interface:
 *
 *   \b bool \b isFinal()  - returns true if the set contains only 
 *                one element and false otherwise
 *
 *   \b tValue \b getValue() - returns best possible estimation for 
 *                elements of the set (in the case the 
 *                set if final it is the value of the 
 *                objective function) 
 *
 *   \b void \b setRecord(tValue record) - sets the value of the record (best
 *                objectivevalue found by the moment).  
 *
 *
 * \b SetFactory is supposed to provide the following interface:  
 *
 *   \b bool \b getInitialSet(Set& set) - fills the starting set to start search
 *                      (usually the search space), returns false if the search
 *                      space id empty
 *
 *   \b void \b branch(tSet set, std::vector < tSet > &sv) - derives 
 *                      vector sv of descendants from the set given 
 *                      as set
 *
 *   \b void \b send(std::vector <tSet> &sv, int rank) - sends a vector of sets to the 
 *                      process with the number rank  
 *
 *   \b int \b recv(std::vector <tSet> &sv, bool block) - recieves a vector of sets from 
 *                      an arbitrary process and returns its rank. If block is set to true
 *                      then function blocks until new message arraives. Otherwise it returns 
 *                      -1 if the message has not yet arrived.
 *
 *   \b int \b recv(std::vector <tSet> &sv, int rank) - recieves a vector of sets from 
 *                      the process with a given rank and returns it
 *
 *   @attention It is supposed that Set is copied as a whole, i.e. if it contains pointer to data 
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
template < class Set, class SetFactory, class Traverse > class BNBProbeSolver:public  Traverse
{
public:

  /**
   * The constructor.
   *
   * @param space computing space descriptor
   * @param fact set factory
   * @param streshold maximal number of sets on a slave process
   * @param btreshold maximal number of brnaches on a slave process
   * @param masterTreshold maximal number of sets to generate on the master process
   * @param solutionQueueTreshold maximal number of solution 
   */
  BNBProbeSolver (AbstractCommunicator * ac, 
		  SetFactory * fact, int streshold, int btreshold,
		  int masterTreshold = 1) : Traverse (fact)
  {
    mAC = ac;
    mSTreshold = streshold;
    mBTreshold = btreshold;
    mMasterTreshold = masterTreshold;
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

    r = mAC->getMyId();

    if (r == 0)
      {
	master ();
      }
    else if (r != -1)
      {
	slave ();
      }
  }


private:

/**
 * Master process
 *
 * @return number of branches
 *
 */
  int master ()
  {
    Counter crecv ("receives");
    Counter csend ("sends");
    Counter cbranch ("branches");
    Counter csentsets ("sent sets");
    Counter crecvsets ("recv sets");

    Timer toverall ("overall time");
    Timer tbusy ("busy time");
    Timer tstarvation ("starvation time");
    Timer trecv ("recv time");
    Timer tsend ("send time");
    Timer tbranch ("branch time");

    int n;
    int ST;
    int BT;
    Set s;
    std::vector < Set > sv;
    std::deque < int >freeSlaves;
    int BTHistory[MAX_HISTORY_LENGTH];
    int BTHistoryCount = 0;
    int maxrank = 0;
    int abranch = 0;
    double mint = -1.;
    double maxt = -1.;
    int minb = -1;
    int maxb = -1;
    int maxc = -1;


    toverall.start ();

    ST = mSTreshold;
    BT = mBTreshold;

    if (mSetFactory->getInitialSet (s))
      sv.push_back (s);

    if (mSetFactory->getInitialSolution (s))
      sv.push_back (s);

    pushSets (sv);

    tbranch.start ();
    if (mMasterTreshold > 1)
      {
	while (1)
	  {
	    bool h;

	    n = branch (h);
	    if (h)
	      cbranch.inc ();
	    if (n == 0)
	      break;
	    else if (n >= mMasterTreshold)
	      break;
	  }
      }
    tbranch.stop ();



    for (int i = 1; i < mAC->getProcessNumber(); i++)
      freeSlaves.push_back (i);


    while (1)
      {

	bool starvate;
	bool busy;

	int rank;
	int msize;
	std::vector < Set > sv;

	starvate = false;
	busy = false;

	if (mTaskQueue.empty ())
	  {
	    if (freeSlaves.size () == (mAC->getProcessNumber() - 1))
	      break;
	    else if (freeSlaves.empty ())
	      busy = true;
	    else
	      starvate = true;
	  }
	else if (freeSlaves.empty ())
	  {
	    busy = true;
	  }
	else
	  {
	    Set s;

	    s = mTaskQueue.top ();
	    mTaskQueue.pop ();

	    if (!s.discard (mRecord))
	      {
		int rank;
		std::vector < Set > sv;

		csend.inc ();
		csentsets.inc ();
		tsend.start ();

		sv.push_back (s);
		rank = freeSlaves.front ();
		maxrank = BNBMAX (rank, maxrank);
		freeSlaves.pop_front ();
		*mAC << sv;
		*mAC << mRecord;
		*mAC << BT;
		*mAC << ST;

		mAC->send (rank);

		tsend.stop ();
	      }
	  }

	if (busy == true)
	  tbusy.start ();
	else if (starvate == true)
	  tstarvation.start ();

	if ((busy == false) && (starvate == false))
	  {
	    rank = mAC->probe (&msize);
	    if (rank == -1)
	      continue;
	  }
	else
	  rank = mAC->probe (&msize, true);

	if (busy == true)
	  tbusy.stop ();
	else if (starvate == true)
	  tstarvation.stop ();


	trecv.start ();

	rank = mAC->recv (rank, msize);
	*mAC >> sv;
	crecv.inc ();
	crecvsets.inc (sv.size ());
	freeSlaves.push_front (rank);
	pushSets (sv);

	trecv.stop ();
      }

    n = freeSlaves.size ();

    tsend.start ();
    while (!freeSlaves.empty ())
      {
	int r;
	std::vector < Set > sv;

	r = freeSlaves.front ();
	freeSlaves.pop_front ();

	*mAC << 0;
	mAC->send (r);

      }
    tsend.stop ();
    toverall.stop ();

    while (n > 0)
      {
	int r;
	double tall;
	double trecv;
	double tpack;
	double tsend;
	double tbranch;
	int cbranch;
	int crecv;
	int csend;
	int c;

	r = mAC->recv (-1, 0);
	*mAC >> tall
	     >> trecv >> tpack >> tsend >> tbranch >> cbranch >> crecv >> csend >> c;

	maxc = BNBMAX (c, maxc);

	abranch += cbranch;
	if (mint < 0)
	  mint = tbranch;
	else
	  mint = BNBMIN (mint, tbranch);

	if (maxt < 0)
	  maxt = tbranch;
	else
	  maxt = BNBMAX (maxt, tbranch);

	if (minb < 0)
	  minb = cbranch;
	else
	  minb = BNBMIN (minb, cbranch);

	if (maxb < 0)
	  maxb = cbranch;
	else
	  maxb = BNBMAX (maxb, cbranch);

	std::ostringstream os;
	
	os << "[slave " << r << "]"
	  << "(tall = " << tall << ","
	  << "trecv = " << trecv << ","
	   << "tpack = " << tpack << ","
	  << "tsend = " << tsend << ","
	  << "tbranch = " << tbranch << ","
	  << "cbranch = " << cbranch << ","
	  << "crecv = " << crecv << "," << "csend = " << csend << ","
	  << "one branch time = " << (double) tbranch /
	  BNBMAX((double) cbranch, 1.) << ")\n";

	printf ("%s\n", os.str ().c_str ());
	fflush (stdout);
        
	n--;
      }
    
    std::ostringstream os;
    os << "[master (probe)]\n\t"
      << toverall.toString () << "\n\t "
      << trecv.toString () << "\n\t"
      << tsend.toString () << "\n\t"
      << tbranch.toString () << "\n\t"
      << tbusy.toString () << "\n\t"
      << tstarvation.toString () << "\n\t"
      << csend.toString () << "\n\t "
      << csentsets.toString () << "\n\t"
      << crecv.toString () << "\n\t"
      << crecvsets.toString () << "\n\t"
      << cbranch.toString () << "\n\t"
      << "total branches = " << abranch << "\n\t"
      << "slaves involved = " << maxrank << "\n\t"      
      << "one send+pack time = " << (double) tsend.getTotalTime () /
      (double) csend.get () << "\n\t"
      << "one recv+unpack time = " << (double) trecv.getTotalTime () /
      (double) crecv.get () << "\n\t"      
      << "min:max slave brnach time = " << mint << " : " << maxt << "\n\t"
      << "min:max slave branches = " << minb << " : " << maxb << "\n\t"      
      << "average slave branches = " << abranch / maxrank << "\n\t"      
      << "biggest task size = " << maxc << "\n\t"      
      //   << "average task size = " << abranch / mMasterTreshold << "\n";
      << "average task size = " <<  mMasterTreshold << "\n";

    printf ("%s\n", os.str ().c_str ());
    fflush (stdout);
    
    return cbranch.get ();
  }

/**
 * Slave process.
 *
 *
 * @return number of branches
 *
 */
  int slave ()
  {
    int maxc = 0;
    double tauC;
    double tauE;
    Timer toverall ("overall time");
    Timer trecv ("recv time");
    Timer tsend ("send time");
    Timer tbranch ("branch time");
    Timer tidle ("idle time");
    Timer tpack("pack time");

    Counter cbranch ("branches");
    Counter csentportions ("sent portions");
    Counter csentsets ("sent sets");
    Counter ctaskrecv ("task received");

    tauC = tauE = 1.;

    toverall.start ();

    while (1)
      {
	double t;
	int n;
	int c;
	int ST;
	int BT;
	int rank;
	int msize;



	typename Set::ValueType oldrecord;
	std::vector < Set > sv;

	trecv.start ();
	mAC->recv (0, 0);
	*mAC >> sv;

	if (sv.empty ())
	  {
	    trecv.stop ();
	    break;
	  }
	else
	  {
	    ctaskrecv.inc ();
	    pushSets (sv);
	    *mAC >> mRecord;
	    *mAC >> BT;
	    *mAC >> ST;
	  }

	trecv.stop ();
	tbranch.start ();

	oldrecord = mRecord;
	c = 0;


	while (1)
	  {
	    bool h;

	    n = branch (h);
	    if (h)
	      {
		cbranch.inc ();
		c++;
	      }
	    if (n == 0)
	      break;
	    else if ((ST != -1) && (n >= ST))
	      break;
	    else if ((BT != -1) && (c > BT))
	      break;
	  }

	maxc = BNBMAX (c, maxc);

	tbranch.stop ();

	while (!mTaskQueue.empty ())
	  {
	    Set s;

	    s = mTaskQueue.top ();
	    mTaskQueue.pop ();

	    if (!s.discard (mRecord))
	      sv.push_back (s);
	     
	  }

	if (mRecord > oldrecord) {
	  Set s;
	  if(!mSolution.empty()) {
	    s = mSolution.top();
	    sv.push_back (s);
	  }
	}

	csentportions.inc ();
	n = sv.size ();
	csentsets.inc (n);

	tpack.start ();
	*mAC << sv;
	tpack.stop ();

	tsend.start ();
	t = mAC->time ();
	mAC->send (0);
	t = mAC->time () - t;

	tsend.stop ();
      }

    toverall.stop ();

    *mAC << toverall.getTotalTime ()
      << trecv.getTotalTime ()
	 << tpack.getTotalTime() 
      << tsend.getTotalTime ()
      << tbranch.getTotalTime ()
      << cbranch.get () << ctaskrecv.get () << csentsets.get () << maxc;
    mAC->send (0);

    return cbranch.get ();
  }


/**
 * A pointer to the abstract communicator
 */
  AbstractCommunicator *mAC;

/**
 * Set treshold (limitation on the number of sets generated before starting getting rid of sets).
 */
  int mSTreshold;

/**
 * Branches treshold (limitation on the number brnaches before starting getting rid pf sets).
 */
  int mBTreshold;

/**
 * Amount of sets to generated on the master before starting dessiminating of sets among slaves.
 */
  int mMasterTreshold;
};

#endif
