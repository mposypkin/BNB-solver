#ifndef __SERIALSOLVER_HPP__
#define __SERIALSOLVER_HPP__

#include <util/common/util.hpp>
#include <ei/external/bnbgate.hpp>

/**
  * Abstract class for solvers
  */
class SerialSolver {
  public:

    /**
     * Set solver configuration
     */
    void setGate(BNBGate* gate)
    {
      mGate = gate;
    }

    /**
     * Main solve process
     *
     */
    virtual void solve() 
    {
      BNB_INFO_REPORT("Solver's solve method not implemented");
    }

  protected:

    BNBGate *mGate;
};

#endif

