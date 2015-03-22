#ifndef __CONTHEUR_HPP__
#define __CONTHEUR_HPP__

#include <util/common/bnblog.hpp>
#include <kernel/bnblib/heurslave.hpp>
#include <problems/optlib/locoptim.hpp>
#include <kernel/bnblib/bnbdivers.hpp>
#include "go.hpp"
#include <problems/semilocal/mbh/pertruber.hpp>

template <class ProblemFactory> class ContHeur : public BNBHeurSlave <ProblemFactory>{
  public:
    typedef typename ProblemFactory::Solution Solution;
    typedef typename ProblemFactory::ValueType VT;
  
    ContHeur () : BNBHeurSlave <ProblemFactory> ()
    {
      mLocOpt = NULL;
      mAux = NULL;
      mAuxV = 0;
      mAmpl = 0;
      mNumberOfStarts = 0;
      mDivers = NULL;
      mStartFromZero = false;
      mGreedy = true;
      mDivideCube = false;
      mId = NULL;
      mMyId = 0;
      mNP = 1;      
      mPert = NULL;
    }
    
    ~ContHeur()
    {
      if(mAux)
        free(mAux);
    }
    
    /**
     * Setup the heuristic in accordance to process number 
     * @param p rank of concrete process
     * @param nofp number of processes for heuristic of this sort
     */
    virtual void setup(int p, int nofp)
    {
      srand(p);
      mMyId = p;
      mNP = nofp;
    }
    
    /**
     * Setup local optimizer
     * @param opt local optimizer
     */
    void setLocalOptimizer(LocalOptimizer<VT> *opt)
    {
      mLocOpt = opt;
      mDim = opt->getObjective()->getDim();
      mAux = (VT*) realloc(mAux, mDim * sizeof(VT));
    }

    /**
     * Set initial data
     * @param id initial data
     */
    void setInitialData(GOInitialData<VT>* id) 
    {
      mId = id;
    }
    
    /**
     * Performs one heuristic step
     * @param s starting point
     * @return true if the search is completed, false if the search is interrupted
     */
    virtual bool step(std::vector<Solution>& sv)
    {
      VT v;
      mLog << "Starting ";
      if(mStartFromZero)
        mLog << "START_FROM_ZERO";
      else
        mLog << "START_FROM_X";
      mLog << "(" << mNumberOfStarts << " starts)\n";
      BNB_ASSERT(mLocOpt);
      BNB_ASSERT(sv.size() == 1);
      Solution s = sv.at(0);
      sv.pop_back();
      BNB_ASSERT(mNumberOfStarts > 0);
      for(int i = 0; i < mNumberOfStarts;) {
        pertrub(i, s.mX);
        i ++;
        if(mLocOpt->search(mAux, &mAuxV)) {
          if(mAuxV < s.mFval) {
            pushPoint(sv);
            if(mGreedy)
              break;
          }
        }        
        mLog << "OLD = " << s.mFval << "AUXV" << mAuxV << "\n";
      }
      mLog.write();
      return true;
    }
    
    /**
     * Setup amplitude
     * @param ampl amplitude new value
     */
    void setAmplitude(VT ampl)
    {
      mAmpl = ampl;
    }
    
    /**
     * Setup greedy flag
     * @param greedy Greedy flag
     */
    void setGreedy(bool greedy)
    {
      mGreedy = greedy;
    }
    
    /**
     * Setup number of starts
     * @param ns number of starts
     */
    void setNumberOfStarts(int ns)
    {
      mNumberOfStarts = ns;
    }
    
    /**
     * Setup diversity measure
     * @param eps measure
     */
    void setDivers(BNBDivers<Solution>* divers)
    {
      mDivers = divers;
    }

    /**
     * Setup Start from O flag
     * @param value flag value 
     */
    void setStartFromZero(bool value = true)
    {
      mStartFromZero = true;
    }
    
    /**
     * Set divide cubic neighbourhood among processsors flag
     * @param flag true if divide false otherwise
     */
    void setDivideCube(bool flag)
    {
      mDivideCube = flag;
    }
    
    /**
     * Setup pertruber
     * @param pert pertruber
     */
    void setPertruber(Pertruber<VT>* pert)
    {
      mPert = pert;
    }
  private:
    
    bool pertrub(int cnt, VT* x)
    {
      BNB_ASSERT(mId);
      int s = 0;
      if(mPert) {
        mPert->pertrub(cnt, x, mAux);
      } else {
        if(mDivideCube) {
          VT h, o, alpha, delta;
          s = 1;
          h = mAmpl / mNP;
          o = - 0.5 * mAmpl + mMyId * h;
          alpha = o + h * ((double)rand() / (double)RAND_MAX);
          if(mStartFromZero)
            mAux[0] = alpha;
          else
            mAux[0] = x[0] + alpha;
        }      
        for(int i = s; i < mDim; i ++) {
          VT alpha, delta;
          alpha = mAmpl * (0.5 - ((double)rand() / (double)RAND_MAX));
          if(mStartFromZero)
            mAux[i] = alpha;
          else
            mAux[i] = x[i] + alpha;
        /*
          if(mId->mA[i] > mAux[i])
          mAux[i] = mId->mA[i];
          else if(mId->mB[i] < mAux[i])
          mAux[i] = mId->mB[i];
        */
        }
      }
      return true;
    }

    
    void pushPoint(std::vector<Solution>& points)
    {
      Solution s(mDim);
      typename std::vector<Solution>::iterator j;
      bool put;

      for(int i = 0; i < mDim; i ++)
        s.mX[i] = mAux[i];
      s.mFval = mAuxV;
      put = true;
      j = points.begin();
      while((!points.empty()) && (j != points.end())) {
        if(same(s, *j)) {
          if(s.mFval > j->mFval) {
            put = false; 
            j ++;
          } else {
            j = points.erase(j);
            continue;
          }
        }
      }
      if(put)
        points.push_back(s);            
    }

    bool same(Solution& s1, Solution& s2) 
    {
      bool rv;
      if(mDivers) 
        rv = mDivers->same(s1, s2);
      else 
        rv = true;

      return rv;
    }
    
    Pertruber<VT> * mPert;
    LocalOptimizer<VT> *mLocOpt;    
    BNBLog<BNB_NO_LOGGING> mLog;
    int mDim;
    int mNumberOfStarts;
    VT* mAux;
    VT mAuxV;
    VT mAmpl;
    BNBDivers<Solution>* mDivers;
    bool mStartFromZero;
    bool mGreedy;
    bool mDivideCube;
    GOInitialData<VT> *mId;
    int mMyId;
    int mNP;
};

#endif
