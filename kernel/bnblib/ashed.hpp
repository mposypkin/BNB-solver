#ifndef _ASHED_HPP_
#define _ASHED_HPP_

#include <util/common/infini.hpp>

/**
 * Abstract interface for all schedulers.
 *
 * @author Mikhail Posypkin, ISA RAS, posypkin@isa.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 */
  class AbstractScheduler 
  {
  public:
   /**
    * Check whether scheduler should be activated;
    *
    * @return true if "yes"
    *
    */
    virtual bool activate(int qlen, int freeSlaves, int totalSlaves) = 0;

    /**
     * @return  complexity treshold for the master process
     */
    virtual InfiniInt getMasterCT() = 0;

    /**
     * @return complexity treshold for the slave process
     *
     */
    virtual InfiniInt getSlaveCT() = 0;

    /**
     * @return  Slave "chunk" (number of subproblems to excahnge within single operation)
     *
     */
    virtual InfiniInt getSlaveChunk() = 0;  

  };
#endif
