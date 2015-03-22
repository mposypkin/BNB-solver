#ifndef _ACLIB_HPP_
#define _ACLIB_HPP_
/**
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file aclib.hpp
 *
 * This file contains an interface for abstract communicator 
 * responsible for basic poin-to-point communication functions.
 */


#include <string>
#include <vector>
#include <map>
#include <util/common/stdvector.hpp>
#include <util/common/util.hpp>
#include <util/common/bnberrcheck.hpp>
/**
 * Communicator's exceptions
 */
struct AbstractCommunicatorException
{

/**
 * Constructor
 *
 * @param info error description
 *
 */
  AbstractCommunicatorException (const char *info)
  {
    mInfo = info;
  }

/**
 * Retrieves error description
 *  
 * @return error description
 *
 */
  const char *getInfo ()
  {
    return mInfo;
  }

/**
 * Pointer to the error description
 */
  const char *mInfo;
};

/**
 * Provides platform-independent convinient communication 
 * layer
 *
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 */
class AbstractCommunicator
{
public:

/**
 * The constructor
 *
 * @param pargc pointer to the command line arguments counter
 *
 * @param pargv pointer to the command line argument array
 *
 */
  AbstractCommunicator (int *pargc, char **pargv[]);



/**
 * The constructor
 *
 * @param old old communicator
 *
 * @param flag true if the process joins the communicator, false otherwise
 *
 */
  AbstractCommunicator (AbstractCommunicator* old, bool flag);

/**
 * The destructor
 */
   ~AbstractCommunicator ();


/**
 * Checks whether the process belongs to the comunicator
 *
 * @return true if the answer is positive false otherwise
 *
 */
  bool isIn();


/**
 * Transfers packed data to the destination process
 *
 * @param procid the destination process identifier
 * 
 * @param mode communication mode
 */
  void transfer (const int procid, const int mode);

/**
 * Sends packed data to the destination process
 * and empties the send buffer
 *
 * @param procid the destination process identifier
 * 
 * @param mode communication mode
 * 
 */
  void send (const int procid, const int mode = async)
  {
    transfer (procid, mode);
    empty ();
  }


/**
 * Sends data to the process and preserves the buffer
 *
 * @param procid destination process 
 *
 * @param async mode
 *
 */
  void drop (const int procid, const int mode = async)
  {
    transfer (procid, mode);
  }


/**
 * Sends packed data to the number of destination processes
 *
 * @param procids destination processes identifiers
 *
 * @param mode communication mode
 *
 */
  void send(const std::vector<int> procids, const int mode = async)
  {
    for(std::vector<int>::const_iterator i = procids.begin(); i != procids.end(); i ++)
      transfer(*i, mode);
    empty ();
  }

/**
 * Sends packed data to the number of destination processes
 * and preserves the buffer
 *
 * @param procids destination processes identifiers
 *
 * @param mode communication mode
 *
 */
  void drop(const std::vector<int> procids, const int mode = async)
  {
    for(std::vector<int>::const_iterator i = procids.begin(); i != procids.end(); i ++)
      transfer(*i, mode);
  }





/**
 * Checks for pending messages (non-blocking or blocking)
 *
 * @param msize message size
 *
 * @param block block or not until the message arrives
 *
 * @return the id of the process sent message, -1 if no message is available
 *
 */
  int probe (int *msize, bool block = false);

/**
 * Blocking receive from the process with procid.
 * The message is stored in the internal buffer
 * 
 * @param procid process identificator from which the message is expected; -1 for template
 *
 * @param msize message size (if 0 then mpi_probe is executed first)
 *
 * @return rank of the process sent message
 */
  int recv (const int procid, const int msize = 0);



/**
 * Empties send buffer
 *
 */
  void empty ()
  {
    mSendBufferPosition = 0;
  }

/**
 * Broadcasts current buffer from the given processor to 
 * remaining
 *
 * @param root root processor
 *
 * @param mode communication mode (sunchronous or asynchronous)
 *
 */
  void bcast (int root, int mode = async)
  {
    if (mMyId == root)
      for (int i = 0; i < mProcessNumber; i++)
	transfer (i, mode);
    recv (root);
    if (mMyId == root)
      empty ();
  }


/**
 * Aborts execution of the whole program
 *
 * @param rc return code
 */
  void abort (int rc = -1);


/**
 * Barrier synchronization
 */
  void barrier ();

/**
 * Retrieves wall time measured in seconds from the program startup
 *
 * @return time value
 *
 */
  double time ();


/**
 * Retreives the total number of processes.
 *
 * @return the total number of processes
 *
 */
  const int getProcessNumber ()
  {
    return mProcessNumber;
  }

/**
 * Retreives the local process identifier
 *
 * @return the local process identifier
 *
 */
  const int getMyId ()
  {
    return mMyId;
  }


/**
 * Packs data into send buffer
 *
 * @param data data to pack
 */
  template < class T > void pack (const T * data);

/**
 * Unpacks data from the receive buffer
 *
 * @param data where to unpack
 */
  template < class T > void unpack (T * data);

/**
 * Packs data into send buffer
 *
 * @param data data to pack
 * @param n how many items to unpack
 */
  template < class T > void packArray (const T * data, int n);

/**
 * Unpacks data from the receive buffer
 *
 * @param data where to unpack
 * @param n how many items to unpack
 */
  template < class T > void unpackArray (T * data, int n);


/**
 * Packs string
 *
 * @param s string to unpack
 *
 */
  void pack (const std::string * s)
  {
    int n;

    n = s->size ();
    pack (&n);
    packArray (s->c_str (), s->size ());
  }


/**
 * Unpacks string
 *
 * @param s string to pack
 *
 */
  void unpack (std::string * s)
  {
    int n;
    char *p;

    unpack (&n);
    p = new char[n + 1];
    unpackArray (p, n);
    p[n] = 0;
    *s = std::string (p);
  }

  /**
   * Packs FixedVector
   *
   * @param v vector to pack
   *
   * @return communicator parameter
   *
   */
  template < class T > void pack (const FixedVector < T > *v)
  {
    int n;

    n = v->size ();
    pack (&n);
    for (int i = 0; i < n; i++)
      pack (&((*v)[i]));
  }

  /**
   * Unpacks FixedVector (appends elements to the parameter vector)
   *
   * @param v
   *
   */
  template < class T > void unpack (FixedVector < T > *v)
  {
    int n;

    unpack (&n);
    v->clear();
    for (int i = 0; i < n; i++)
    {
      T e;
      unpack (&e);
      v->push_back (e);
    }
  }


  /**
   * Packs StdVector
   *
   * @param v vector to pack
   *
   * @return communicator parameter
   *
   */
  template < class T > void pack (const StdVector < T > *v)
  {
    int n;

    n = v->size ();
    pack (&n);
    for (int i = 0; i < n; i++)
      pack (&((*v)[i]));
  }

  /**
   * Unpacks StdVector (appends elements to the parameter vector)
   *
   * @param v
   *
   */
  template < class T > void unpack (StdVector < T > *v)
  {
    int n;

    unpack (&n);
    v->clear();
    for (int i = 0; i < n; i++)
    {
      T e;
      unpack (&e);
      v->push_back (e);
    }
  }

 /**
 * Packs vector
 *
 * @param v vector to pack
 *
 * @return communicator parameter
 *
 */
  template < class T > void pack (const std::vector < T > *v)
  {
    int n;

    n = v->size ();
    pack (&n);
    for (int i = 0; i < n; i++)
      pack (&((*v)[i]));
  }


/**
 * Unpacks vector (appends elements to the parameter vector)
 *
 * @param v
 *
 */
  template < class T > void unpack (std::vector < T > *v)
  {
    int n;

    unpack (&n);    
    v->clear();
    for (int i = 0; i < n; i++)
      {
	T e;
	unpack (&e);
	v->push_back (e);
      }
  }

/**
 * Packs map
 *
 * @param p map to pack
 *
 */
  template < class T1, class T2 > void pack (const std::map < T1, T2 > *p)
  {
    int n;

    n = p->size ();
    pack (&n);
    for (typename std::map < T1, T2 >::const_iterator i = p->begin (); i != p->end ();
	 i++)
      {
	pack (&(i->first));
	pack (&(i->second));
      }
  }

/**
 * Unpacks map
 *
 * @param m map to unpack to
 *
 */
  template < class T1, class T2 > void unpack (std::map < T1, T2 > *p)
  {
    int n;

    unpack (&n);
    for (int i = 0; i < n; i++)
      {
	T1 f;
	T2 s;

	unpack (&f);
	unpack (&s);
	(*p)[f] = s;
      }
  }


/**
 * Packs  data item to the send buffer
 *
 * @param ac abstract communicator
 *
 * @param data integral data item
 *
 * @return communicator parameter 
 *
 */
  template < class T > AbstractCommunicator & operator << (const T & data)
  {
    pack (&data);
    return *this;
  }

/**
 * Unpacks data item from the receive buffer
 *
 * @param ac abstract communicator
 *
 * @param data integral variable to be filled
 *
 * @return communicatot parameter
 *
 */
  template < class T > AbstractCommunicator & operator >> (T & data)
  {
    BNB_ASSERT(mRecvBufferPosition >= 0);
    unpack (&data);
    return *this;
  }


/**
 * Sets up the attribute
 *
 * @param attr attribute key
 *
 * @param value attribute value
 *
 */
  void putAttr(std::string attr, int value)
  {
    mAttributes[attr] = value;
  }


/**
 * Retrieves the attribute values
 *
 * @param attr attribue name
 *
 * @return attribute value associated with the provided name
 *
 */
  int getAttr(std::string attr)
  {
    return mAttributes[attr];
  }

/**
 * Asynchronous communication mode
 */
  static const int async = 1;

/**
 * Synchronous communication mode
 */
  static const int sync = 2;

private:

/**
 * Send data buffer. 
 */
  Resizeable mSendBuffer;

/**
 * Current position in the send buffer
 */
  int mSendBufferPosition;

/**
 * Recive buffer.
 */
  Resizeable mRecvBuffer;

/**
 * Current position in the receive buffer (for unpacking)
 */
  int mRecvBufferPosition;


/**
 * Local process identificator.
 */
  int mMyId;

/**
 * Total number of processes.
 */
  int mProcessNumber;

/**
 * Time measured at the startup
 */
  double mTime;

  /**
   * Implementation-specific information
   */
  void* mAuxData;

/**
 * Attributes associated with abstract communicator
 */
  std::map<std::string, int> mAttributes;
};



#endif
