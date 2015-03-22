#ifndef __SPHEREGENERATOR_HPP_
#define __SPHEREGENERATOR_HPP_

#include <kernel/bnblib/asolgenerator.hpp>
#include "go.hpp"
#include <util/common/vec.hpp>
#include <util/common/random.hpp>

/**
 * Generate data for shpere constarints
 */
template <typename FloatType> class SphereSolutionGenerator : 
        public AbstractSolutionGenerator < GOFactory < FloatType > > 
{
  public:
    typedef typename GOFactory < FloatType >::Solution Solution;
    
    
    SphereSolutionGenerator() {
      mRadius = 1.;
      mId = NULL;
      mCenter = NULL;
    }
    
    /**
     * Set sphere radius
     * @param radius radius
     */
    void setRadius(FloatType radius) 
    {
      mRadius = radius;
    }
    
    /**
     * Set center of sphere
     * @param center new center
     */    
    void setCenter(FloatType * center)
    {
      mCenter = center;
    }
    
    /**
     * Set initial data
     * @param id Initial data 
     */
    void setInitialData(GOInitialData <FloatType> * id)
    {
      mId = id;
    }
    
    void generate(int n, GVector < Solution > &solv)
    {
      int dim = mId->mObj->getDim();
      FloatType* x;
      BNB_ASSERT(mId);
      x = (FloatType*) malloc(dim * sizeof(FloatType));
      for(int i = 0; i < n; ) {
                
        Solution sol (dim);
        UniformSphereDistribution::nextPoint(dim, mRadius, mCenter, x);
        sol.mFval = mId->mObj->func(x);
        VecUtils::vecCopy(dim, x, (double*)sol.mX);
//        printf("SOL: %s\n", sol.toString().c_str());
        solv.push_back(sol);
        i ++;
        
      }
      free(x);
    }

    
            
  private:
    
    GOInitialData < FloatType > * mId;
    FloatType mRadius;
    FloatType *mCenter;
        
  
};


#endif
