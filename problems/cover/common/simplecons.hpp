#ifndef __SIMPLECONS_HPP__
#define __SIMPLECONS_HPP__

#include <vector>
#include <problems/optlib/function.hpp>
#include "go.hpp"
    

template <class FT> class SimpleConstraintChecker : public GOFactory<FT>::ConstraintChecker {
  public:
    
    struct FuncRec {
      Function<FT>* mPF;
      FT mEps;
    };
    
    
    bool check(FT* x) {
      bool rv = true;
      if(!mEq.empty()) {
        int sz = mEq.size();
        for(int i = 0; i < sz; i ++) {
          FT v =  mEq[i].mPF->func(x);
          if(BNBABS(v) > mEq[i].mEps) {
            rv = false;
            break;
          }
        }
        
      }
      
      if(!mIneq.empty()) {
        int sz = mIneq.size();
        
        for(int i = 0; i < sz; i ++) {
          FT v =  mIneq[i].mPF->func(x);
          if(v > mIneq[i].mEps){
            rv = false;
            break;
          }
        }        
      }
      return rv;
    }
    
    void addEqualityConstraint(Function<FT>* pf, FT eps)
    {
      FuncRec fr;
      fr.mPF = pf;
      fr.mEps = eps;
      mEq.push_back(fr);
    }
    
    void addInequalityConstraint(Function<FT>* pf, FT eps)
    {
      FuncRec fr;
      fr.mPF = pf;
      fr.mEps = eps;
      mIneq.push_back(fr);
    }
  
    void print(FT* x)
    {
      if(!mEq.empty()) {
        int sz = mEq.size();
        printf("%d equality constraints:\n", sz);
        for(int i = 0; i < sz; i ++) {
          FT v =  mEq[i].mPF->func(x);
          printf("%d: value = %lf (eps = %lf)\n", i, v, mEq[i].mEps);
        }
      }
      
      if(!mIneq.empty()) {
        int sz = mIneq.size();
        printf("%d inequality constraints:\n", sz);        
        for(int i = 0; i < sz; i ++) {
          FT v =  mIneq[i].mPF->func(x);
          printf("%d: value = %.10f (eps = %.10f)\n", i, v, mIneq[i].mEps);
        }        
      }
    }
  private:
    
    std::vector<FuncRec> mEq;
    
    std::vector<FuncRec> mIneq;
    
  
};

#endif
