#ifndef __BASICHEURMASTER_HPP__
#define __BASICHEURMASTER_HPP__

#include <vector>
#include <sstream>
#include "heurmaster.hpp"
#include "bnbdivers.hpp"
#include "bnbtransformer.hpp"
#include "bnbsolutionsurface.hpp"

template <class ProblemFactory> class BNBBasicHeurMaster : public BNBHeurMaster <ProblemFactory> {
  public:
    typedef  BNBHeurMaster<ProblemFactory> BASE;
    typedef typename BNBHeurMaster<ProblemFactory>::Solution Solution;
    typedef typename ProblemFactory::ValueType VT;
    
    typedef struct {
      /**
       * Solution
       */
      Solution mSolution;
      /**
       * List of non-visited
       * processors
       */
      std::vector<int> mList;
    } SolutionRecord;
    
    struct UpdateModes {
      /**
       * Base
       */
      static const unsigned int NO_UPDATE = 0;
      /**
       * New solutions received from heuristic slaves are stored
       */
      static const unsigned int DO_UPDATE_FROM_HEUR = 1;      
      /**
       * New solutions received from branching slaves are stored
       */
      static const unsigned int DO_UPDATE_FROM_BNB = (1 << 1);      
      
      /**
       * New solutions received from master are stored
       */
      static const unsigned int DO_UPDATE_FROM_MASTER = (1 << 2);
    };
    
    
    BNBBasicHeurMaster()
    {
      mMaxPoolSize = -1;
      mDivers = NULL;
      mTransformer = NULL;
      mSolutionSurface = NULL;
      mDoPersist = false;
      mSendToAll = true;
      mUpdateMode = 
          UpdateModes::DO_UPDATE_FROM_HEUR |
          UpdateModes::DO_UPDATE_FROM_BNB |
          UpdateModes::DO_UPDATE_FROM_MASTER;
      mSolSurUpdateMode =
          UpdateModes::DO_UPDATE_FROM_HEUR |
          UpdateModes::DO_UPDATE_FROM_BNB |
          UpdateModes::DO_UPDATE_FROM_MASTER;
      
    }
    
    
    /**
     * Setup solution surface
     * @param ss solution surface
     */
    void setSolutionSurface(BNBSolutionSurface<ProblemFactory>* ss){
      mSolutionSurface = ss;
    }
        
    
    /**
     * Set Update mode
     * @param mode update mode
     */
    void setUpdateMode(unsigned int mode)
    {
      mUpdateMode = mode;
    }
    
    /**
     * Set update mode to solution surface
     * @param mode update mode
     */
    void setSolutionSurfaceUpdateMode(unsigned int mode)
    {
      mSolSurUpdateMode = mode;
    }
    
   /**
    * Retrieve number of solutions
    * @return number of solutions in container
    */
    int getNumberOfSolutions() const
    {
      return mPool.size();
    }
    
    /**
     * Set send_to_all flag
     * @param flag send to all value (true if solutions are sent to all heuristic nodes, false if to one only)
     */
    void setSendToAll(bool flag)
    {
      mSendToAll = flag;
    }

    /**
     * Return string representation of the solution pool
     * @return string representation
     */
    std::string toString() const
    {
      std::ostringstream os;
      os << getNumberOfSolutions() << " sets:[";
      for(int i = 0; i < mPool.size(); i ++) {
        if(i != 0)
          os << ", ";
        os << mPool.at(i).mSolution.getValue();
        os << "(" << mPool.at(i).mList.size()<< ")";
      }
      os << "]";
      return os.str();      
    }
    /**
     * Checks whether solution list is not empty
     * @return true if empty false otherwise
     */
    bool isSolutionListEmpty() const
    {
      bool rv;
      rv = true;
      if(!mPool.empty()) {
        typename std::vector<SolutionRecord>::const_iterator j;
        for(j = mPool.begin(); j != mPool.end(); j ++) {
          if(!(j->mList.empty())) {
//             printf("non-empty detected\n");
            rv = false;
            break;
          }
        }
      } 
      return rv;
    }
    
    /**
     * Setup maximal pool size (-1 for infinity)
     * @param size pool size
     */
    void setMaxPoolSize(int size)
    {
      mMaxPoolSize = size;
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
     * Set up transformer
     * @param transformer transformer 
     */
    void setTransformer(BNBTransformer<Solution>* transformer)
    {
      mTransformer = transformer;
    } 
    
  /**
   * Put a set of solutions to a local pool
   * @param  vector of solutions
   * @return true if solutions improved the incumbent
   */
    virtual bool pushSolutions(std::vector<Solution> &sv, typename BASE::PutStages stage)
    {
      bool flag = false;
      bool upd;
      
      if((stage == BASE::FromHeur) && (mUpdateMode & UpdateModes::DO_UPDATE_FROM_HEUR)) 
        upd = true;
      else if((stage == BASE::FromBNB) && (mUpdateMode & UpdateModes::DO_UPDATE_FROM_BNB))
        upd = true;
      else if((stage == BASE::FromMaster) && (mUpdateMode & UpdateModes::DO_UPDATE_FROM_MASTER))
        upd = true;
      else 
        upd = false;
      
      while(!sv.empty()) {
        SolutionRecord sr;
        sr.mSolution = sv.back();        
        sv.pop_back();      
        if(mSolutionSurface) {
          bool supd;
          if((stage == BASE::FromHeur) && (mSolSurUpdateMode & UpdateModes::DO_UPDATE_FROM_HEUR)) 
            supd = true;
          else if((stage == BASE::FromBNB) && (mSolSurUpdateMode & UpdateModes::DO_UPDATE_FROM_BNB))
            supd = true;
          else if((stage == BASE::FromMaster) && (mSolSurUpdateMode & UpdateModes::DO_UPDATE_FROM_MASTER))
            supd = true;
          else 
            supd = false;
          if(supd)
            mSolutionSurface->put(sr.mSolution);  
        }
        if(((ProblemFactory::getProblemType() == BNB_MAXIMIZE) &&
             (sr.mSolution.getValue() > BASE::mIncumbent)) || 
             ((ProblemFactory::getProblemType() == BNB_MINIMIZE) && 
             (sr.mSolution.getValue() < BASE::mIncumbent))){
          BASE::mIncumbent = sr.mSolution.getValue();
          BASE::mBestSolution = sr.mSolution;
          BASE::mHasBestSolution = true;
          flag = true;
             }
             if(upd) {               
               typename std::vector<SolutionRecord>::iterator i;
               bool put;
               put = true;
               transform(sr);
               if(put) {
                 i = mPool.begin();
                 while((!mPool.empty()) && (i != mPool.end())) {           
                   if(same(sr,*i)) {                 
                     if(((ProblemFactory::getProblemType() == BNB_MINIMIZE) && 
                          (sr.mSolution.getValue() >= i->mSolution.getValue())) ||
                          ((ProblemFactory::getProblemType() == BNB_MAXIMIZE) && 
                          (sr.mSolution.getValue() <= i->mSolution.getValue()))) 
                       put = false;
                     else {
                       mPool.erase(i);
                       i = mPool.begin();
                       continue;
                     }              
                   }
                   i ++;
                 }
               }
               if(put) { 
                 if(mSendToAll) {         
                   BNB_ASSERT(BASE::mSlaves);
                   for(int i = 0; i < BASE::mSlaves->size(); i ++) 
                     sr.mList.push_back(BASE::mSlaves->at(i));
                 } else 
                   sr.mList.push_back(-1);
                   mPool.push_back(sr);        
                   cleanPool();
               }
             }        
      }
      return flag;
    };
  
    /**
     * Retrieve solutions from a pool
     * @param n number of solutions
     * @param sv solution vector
     */
    void getSolutions(int n, std::vector<Solution> &sv)
    {
      typename std::vector<SolutionRecord>::iterator i;
      i = mPool.begin();
      while(true) {
        if(i == mPool.end())
          break;
        if(n == 0)
          break;        
        if(!(i->mList.empty())){
          sv.push_back(i->mSolution);
          i = mPool.erase(i);
          n --;
        } else {
          i ++;
        }        
      }
    }

    
    virtual bool findBestMatch(std::deque<int> &freeSlaves , std::vector<Solution> &sv, int &s)
    {
      std::deque<int>::iterator i;
      bool flag = false;
      typename std::vector<SolutionRecord>::iterator best;
      best = mPool.end();
      for(i = freeSlaves.begin(); i != freeSlaves.end(); i ++)  {
        typename std::vector<SolutionRecord>::iterator j;
        for(j = mPool.begin(); j != mPool.end();) {
          std::vector<int>::iterator k;
          for(k = j->mList.begin(); k != j->mList.end(); k ++) {
            if((*k == -1) || (*k == *i)) {
              if(best == mPool.end())
                best = j;
              else if(((ProblemFactory::getProblemType() == BNB_MINIMIZE)&&
                        (j->mSolution.getValue() < best->mSolution.getValue()))
                      ||
                        ((ProblemFactory::getProblemType() == BNB_MAXIMIZE) &&
                        (j->mSolution.getValue() > best->mSolution.getValue()))) 
              {
                 best = j;
              }              
            }
          }
          j ++;
        }
        if(best != mPool.end()) {
          s = *i;
          std::vector<int>::iterator k;
          for(k = best->mList.begin(); k != best->mList.end(); k ++) 
            if((*k == -1) || (*k == *i)) 
              break;
          if(!mDoPersist)
            best->mList.erase(k);              
          sv.push_back(best->mSolution);
          freeSlaves.erase(i);
          flag = true;
          break;
        }
      }
      return flag;
    }
    
    /**
     * Setup persitence flag
     * @param value flag value
     */
    void setDoPersist(bool value = true)
    {
      mDoPersist = value;
    }
    
  private:
    
    const bool same(const SolutionRecord& r1, const SolutionRecord& r2)
    {
      bool rv;
      if(mDivers) 
        rv = mDivers->same(r1.mSolution, r2.mSolution);
      else 
        rv = false;
      
/*      printf("%lf vs %lf is %s\n", r1.mSolution.getValue(), r2.mSolution.getValue(), rv ? "same": "differ");*/
      return rv;
    }
    
    const void transform(SolutionRecord& sr)
    {
      if(mTransformer)
        mTransformer->transform(sr.mSolution);
    }
    
    void cleanPool()
    {
      if(mMaxPoolSize == -1)
        return;
      while(mPool.size() > mMaxPoolSize) {
        typename std::vector<SolutionRecord>::iterator j, i;
        VT antirec;
        antirec = mPool.at(0).mSolution.getValue();
        j = mPool.begin();
        for(i = mPool.begin(); i != mPool.end(); i ++) {
          if(((ProblemFactory::getProblemType() == BNB_MAXIMIZE) && (antirec > i->mSolution.getValue())) ||
               ((ProblemFactory::getProblemType() == BNB_MINIMIZE) && (antirec < i->mSolution.getValue())))
          {
            j = i;
            antirec = i->mSolution.getValue();
          }
        }
        mPool.erase(j);
      }

    }
    /**
     * Solution pool
     */
    std::vector<SolutionRecord> mPool;
    
    /**
     * Maximal pool size
     */
    int mMaxPoolSize;
    
    /**
     * Diversity measure
     */
    BNBDivers<Solution>* mDivers;
    
    /**
     * The transformer
     */
    BNBTransformer<Solution>* mTransformer;
    
    /**
     * The solution surface
     */
    BNBSolutionSurface<ProblemFactory>* mSolutionSurface;
    
    /**
     * True if solutions persist
     */
    bool mDoPersist;
    
    /**
     * True is solution is distributed to all nodes
     */
    bool mSendToAll;
    
    
    /**
     * Update mode
     */
    unsigned int mUpdateMode;
    
    /**
     * Solution surface update mode
     */    
    unsigned int mSolSurUpdateMode;
    
};

#endif
