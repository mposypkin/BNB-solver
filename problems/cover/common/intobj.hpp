#ifndef __INTOBJ_HPP__
#define __INTOBJ_HPP__

#include <problems/optlib/objective.hpp>

template <typename FT> class IntervalObjective : virtual public Objective <FT>
{
  public:
    
    IntervalObjective()
    {
      mDoCheck = false;
    }
    
    /**
     * Set Do Check flag. If set check for interval functions is performed.
     * @param docheck flag value
     */
    void setDoCheck(bool docheck)
    {
      mDoCheck = docheck;
    }
    
    /**
     * Interval version for the objective function
     * @param avec lower bound for argument
     * @param bvec upper bound for argument
     * @param lbound lower bound for gradient
     * @param ubound upper bound for gradient
     */
    void gradInt(FT* avec, FT* bvec, FT* lbound, FT* ubound)
    {
      gradI(avec, bvec, lbound, ubound);
      if(mDoCheck) {
        int n;
        FT *x, *g;
        n = Objective<FT>::getDim();
        x = (FT*)malloc(n *  sizeof(FT));
        g = (FT*)malloc(n *  sizeof(FT));
        for(int i = 0; i < n; i ++) {
          x[i] = 0.5 * (avec[i] + bvec[i]);
        }        
        this->grad(x, g);
        for(int i = 0; i < n; i ++) {
          //printf("%lf %lf %lf\n", lbound[i], g[i], ubound[i]);
          if(g[i] > ubound[i]){
            printf("Gradient %lf > %lf (upper bound)\n", g[i], ubound[i]);            
          } else if(g[i] < lbound[i]) {
            printf("Gradient %lf < %lf (lower bound)\n", g[i], lbound[i]);            
          }
        }
        free(x);
        free(g);
      }
    }

    
  /**
   * Interval version for the objective function (for overloading)
   * @param avec lower bound for argument
   * @param bvec upper bound for argument
   * @param minv the resulting minimal value
   */
    virtual void funcMin(FT* avec, FT* bvec, FT* minv) = 0;
    
    /**
     * Interval version for the gradient
     * @param avec lower bound for argument
     * @param bvec upper bound for argument
     * @param lbound lower of gradient components
     * @param ubound upper bounds for gradient components
     */
    virtual void gradI(FT* avec, FT* bvec, FT* lbound, FT* ubound)
    {
    }
    
    
    /**
     * Interval version of the first Taylor form
     * 
     * @param m point of expansion
     * @param avec argument lower bound
     * @param bvec argument upper bound
     * @param minv the minimal value
     */
    virtual void form1(FT* avec, FT* bvec, FT* minv)
    {
    }
    
    /**
     * Fills in rectangle bounds
     * 
     * @param dim the dimensionality
     * @param a lower bound
     * @param b upper bound
     */
    virtual void fillBounds(int dim, FT *a, FT* b)
    {
    };    
      
  private:
    
    bool mDoCheck;
};
#endif
