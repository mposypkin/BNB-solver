#ifndef __C2_HPP__
#define __C2_HPP__

class C2 :  public LipzObjective<double> {
  
  public:
    
    C2()
    {
      Objective<double>::setDim(4);
      Objective<double>::mKind = Objective<double>::INEQCONSTRAINT;
    }
    
    double func(const double* x)
    {
      double v;
      v = -x[1] + 0.00954 * x[2];
      return v;
    }
        
    void grad(const double* x, double* g)
    {
      g[0] = 0.;
      g[1] = -1.;
      g[2] = 0.00954;
      g[3] = 0.;
    }
    
    double getFuncLipzConstant(double* a, double* b)
    {
      BNB_ERROR_REPORT("NOT_IMPLEMENTED");
    }
    
    double getGradLipzConstant(double* a, double* b)
    {
      return 0.;
    }
};


#endif

