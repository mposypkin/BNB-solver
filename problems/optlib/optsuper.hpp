#ifndef __OPTSUPER_HPP__
#define __OPTSUPER_HPP__

#include <vector>
#include "locoptim.hpp"

template <typename FT> class OptSuper : public LocalOptimizer <FT> {
  public:
    /**
     * Pushes new optimizer
     * @param lo local optimizer pointer
     */
    void pushBack(LocalOptimizer<FT> * lo)
    {
      mLoList.push_back(lo);
    }
    /**
     * Perform search
     * @param x start point and result
     * @param v 
     * @return true if search converged and false otherwise
     */
    bool search(FT* x, FT* v)
    {
      bool success, res;
      success = false;
      for(int i = 0; i < mLoList.size(); i ++) {
        LocalOptimizer <FT>* lop;
        lop = mLoList.at(i);
        res = lop->search(x, v);
        printf("OPTSU: i = %d, v = %lf, %s\n", i, *v, (res ? "success" : "failure"));
        success |= res;
      }
      return success;
    }
 
    /**
     * Retrieve objective
     * @return objective
     */
    Objective<FT>* getObjective()
    {
      if(mLoList.empty())
        return NULL;
      else
        return mLoList.at(0)->getObjective();
    }

  private:
    
    std::vector<LocalOptimizer <FT> *> mLoList;
    
};
#endif
