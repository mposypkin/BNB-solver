#ifndef __PINTER_HPP__
#define __PINTER_HPP__

#include <string>
#include <sstream>


/**
 * Pinter objective
 */
class Pinter : public LipzObjective<double>{
  public:
    
  Pinter(int n, double sigma, double* coe)
  {
    mN = n;
    mSigma = sigma;
    mCoe = coe;    
    setDim(1);
  }
  
  std::string toString()
  {
    std::ostringstream os;
    os << mSigma;
    os << "*x**2";
    for(int i = 0; i < 2 * mN;) {      
      os << "+";
      os << mCoe[i++] << "*(sin(";
      os << mCoe[i++] << "*x))**2";      
    }
    return os.str();
  }
  
  double func(const double* x)
  {
    double y = x[0];
    double v = mSigma * y * y;    
    for(int i = 0; i < mN; i ++) {
      double a = mCoe[2 * i];
      double b = mCoe[2 * i + 1];            
      double w = sin (b * y);
      v += a * w * w;
    }
    return v;
  }

  void grad(const double* x, double* g)
  {
    double y = x[0];
    double v = 2 * mSigma * y;
    for(int i = 0; i < mN; i ++) {
      double a = mCoe[2 * i];
      double b = mCoe[2 * i + 1];            
      double w = a * b * sin(2. * b * y);      
      v +=  w;
    }    
    g[0] = v;
  }
  
  int hessn(double* x, double* H)
  {
    double y = x[0];
    double v = 2. * mSigma;
    for(int i = 0; i < mN; i ++) {
      double a = mCoe[2 * i];
      double b = mCoe[2 * i + 1];            
      double c = 2. * a * b * b;
      double w = c * cos(2. * b * y);      
      v +=  w;
    }        
    H[0] = v;
    return 1;
  }

  double getFuncLipzConstant(double* a, double* b)
  {
    double A = a[0], B = b[0], l;
    l = 2. * mSigma * BNBMAX(BNBABS(A), BNBABS(B));
    for(int i = 0; i < mN; i ++) {
      double a = mCoe[2 * i];
      double b = mCoe[2 * i + 1];    
      l += BNBABS(a * b);
    }
    printf("A = %lf, B = %lf, l = %lf\n", A, B, l);
    return l;        
  }
   
  double getGradLipzConstant(double* a, double* b)
  {
    double A = a[0], B = b[0], l;
    l = 2. * mSigma;
    for(int i = 0; i < mN; i ++) {
      double a = mCoe[2 * i];
      double b = mCoe[2 * i + 1];    
      l += 2. * BNBABS(a * b * b);
    }
    return l;        
  }
    
  double getGradCompLipzConstant(double* a, double* b, int i)
  {
    return getGradLipzConstant(a, b);
  }
    
  double getHessLipzConstant(double* a, double* b)
  {
    double A = a[0], B = b[0], l;
    l = 0.;
    for(int i = 0; i < mN; i ++) {
      double a = mCoe[2 * i];
      double b = mCoe[2 * i + 1];    
      l += 4. * BNBABS(a * b * b * b);
    }
    return l;        
  }
    
  private:
    
    int mN;
    double *mCoe;
    double mSigma;
  
};

#endif
