#ifndef __RNABASE_HPP__
#define __RNABASE_HPP__


/**
 * Base class for RNA computations
 */

template <typename FT> class RNABase 
{
  public:
    
  RNABase()
  {
    mDiag = NULL;
    mSDiag = NULL;
    mKap = NULL;
    mDim = 0;
  }
  
  /**
   * The objective function itself 
   *
   * @param x argument
   *
   * @return function value
   *
   */
  double func(double* x)
  {
    int n;
    double v, y;
      
    BNB_ASSERT(mDiag);
    BNB_ASSERT(mSDiag);
    BNB_ASSERT(mKap);      
    BNB_ASSERT(mDim);
    n = mDim;
    v = 0.;
    for(int i = 0; i < n; i ++) {
      y = mDiag[i] * x[i];
      y *= x[i];
      v += y;
      y = 0.5 * mKap[i];
      y *= x[i];
      y *= x[i];
      y *= x[i];
      y *= x[i];
      v -= y;
      if(i != (n - 1)) {
        y = mSDiag[i] * x[i];
        v +=  y * x[i + 1];
      }
      if(i != 0) {
        y = mSDiag[i-1] * x[i - 1];
        v += y * x[i];
      }
    }      
    return  v;
  }
  
  /**
   * Gradient 
   * 
   * @param x argument
   * @param g gradient
   */
  void grad(double* x, double* g)
  {
    int n;
    double v;
    double y;
      
    v = 0.;
    BNB_ASSERT(mDiag);
    BNB_ASSERT(mSDiag);
    BNB_ASSERT(mKap);
    BNB_ASSERT(mDim);
    n = mDim;
      
    for(int i = 0; i < n; i ++) {
      v = mDiag[i] * x[i];
      y =  mKap[i] * x[i];
      y *= x[i];
      y *= x[i];
      v -=  y;
      if(i != (n - 1)) {
        v += mSDiag[i] * x[i + 1];
      }
      if(i != 0) {
        v += mSDiag[i - 1] * x[i - 1];
      }
      g[i] =  2. * v;
    }
  }

  double* getDiag()
  {
    return mDiag;
  }
  
  void setDim(int dim)
  {
    mDim = dim;
  }
      
  int getDim()
  {
    return mDim;
  }
    
  double* getSDiag()
  {
    return mSDiag;
  }
        
  void setDiag(double* h)
  {
    mDiag = h;
  }
    
  void setSDiag(double* h)
  {
    mSDiag = h;
  }
    
  double* getKap() 
  {
    return mKap;
  }
    
  void setKap(double* kap)
  {
    mKap = kap;
  }
 
  private:
    
    double* mDiag;    
    double* mSDiag;    
    double* mKap;
    int mDim;
};

#endif
