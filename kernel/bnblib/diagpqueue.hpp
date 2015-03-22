#ifndef _DIAGPQUEUE_HPP_
#define _DIAGPQUEUE_HPP_
/**
 * The priority queue for diagnostic purposes.
 *
 * @author Mikhail Posypkin, ISA RAS, posypkin@isa.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file diagpqueue.hpp
 */

#include <time.h>
#include <util/common/bnblog.hpp>

template <class Factory> class BNBDiagPqueue {

public:

  BNBDiagPqueue() {
    mWorst = Factory::absoluteBound();
  }

  /**
   * Below goes standard queue interface
   */
  typename Factory::Set top()
  {
    return mBase.top();
  }

  void pop()
  {
    mBase.pop();
    checkBoundChanged();
  }

  void push(typename Factory::Set& entry)
  {
    mBase.push(entry);
    checkBoundChanged();
  }

  void clear()
  {
    mBase.clear();
  }

  bool empty()
  {
    return mBase.empty();
  }

  int size()
  {
    return mBase.size();
  }
 
private:

/**
 * Checks whether bound changed
 *
 */
  void checkBoundChanged()
  {
    if(BNBLog<BNB_DO_LOGGING>::getUniLog()) {
      if(mWorst != mBase.top().getValue()) {
	mWorst = mBase.top().getValue();
	(*BNBLog<BNB_DO_LOGGING>::getUniLog()) << "Worst bound changed: " <<  " | " << (double)(clock())/ (double)(CLOCKS_PER_SEC) << " " << mWorst <<"\n";
	BNBLog<BNB_DO_LOGGING>::getUniLog()->write();
      }
    }    
  }

  std::priority_queue<typename Factory::Set> mBase;
  
  typename Factory::ValueType mWorst;
};

#endif
