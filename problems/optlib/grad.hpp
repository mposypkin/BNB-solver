#ifndef _GRAD_HPP_
#define _GRAD_HPP_
/**
 * Local optimization methods based on gradient
 * @author Mikhail Posypkin, ISA RAS, posypkin@isa.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file grad.hpp
 */

#include <util/common/vec.hpp>
#include "objective.hpp"


/**
 * The class for optimization of multidimensional function F
 * in a given direction D from the starting point X, i.e. 
 * find 'a' such that 
 *
 *  F(X + a * D) is minimal for all a >= 0
 *
 * @author Mikhail Posypkin, ISA RAS, posypkin@isa.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 */
template <class T> class Uniopt {
public:

  typedef T FloatType;

/**
 * The constructor
 *
 * @param n dimensionality
 *
 * @param xstart the starting point
 *
 * @param dir optimization direction
 *
 * @param obj objective
 *
 */
  Uniopt(T* xstart, T* dir, Objective < T > * obj)
  {
    mCurr = new T[obj->getDim()];
    mXStart = new T[obj->getDim()];
    mDir = new T[obj->getDim()];
    for(int i = 0; i < obj->getDim(); i ++) {
      mXStart[i] = xstart[i];
      mDir[i] = dir[i];
    }
    mObj = obj;
    mStep = 0.1;
    mEps = 0.00000001;
  }

  ~Uniopt()
  {
    delete [] mDir;
    delete [] mCurr;
    delete [] mXStart;
  }

/**
 * Find minimum
 *
 */
  void search()
  {
    T v, vfin, vn, vo, a, b;
    
    b = a = 0.;
    v = uni(a);
    vo = vn = v;
    do {
      vo = vn;
      b = b + mStep;
      vn = uni(b);
    } while(vo > vn);

    vfin = findGold(b - mStep, b, &mAlpha);
    mFMin =  vfin;
  }

/**
 * @return objective minimal value  value
 *
 */
  T getMinimum()
  {
    return mFMin;
  }

/**
 * @return the alpha parameter
 *
 */
  T getAlpha()
  {
    return mAlpha;
  }

/**
 * Sets the initial step (ovverrides the default)
 *
 * @param step step value
 *
 */
  void setStep(T step)
  {
    mStep = step;
  }

/**
 * Sets the tolerance value (minimal segment length)
 *
 * @param eps
 *
 */
  void setEps(T eps)
  {
    mEps = eps;
  }

private:

/**
 * Find minimum by "zolotoe sechenie"
 *
 * @param a left bound
 *
 * @param b right bound
 *
 * @param alpha solution point
 *
 * @return minimum value
 *
 */
  T findGold(T a, T b, T* alpha)
  {
    T p, q, an, bn, va, vb, vp, vq, v;
    const static T tau = 0.6180;

    if((b - a) < mEps) {
      *alpha = a;
      return uni(a);
    }
    p = a + (b - a) * (1. - tau);
    q = a + (b - a) * tau;
    va = uni(a);
    vb = uni(b);
    vp = uni(p);
    vq = uni(q);
    if(vp >= va) {
      an = a;
      bn = p;
    } else if(vp > vq) {
      an = p;
      bn = b;
    } else {
      an = a;
      bn = q;
    }    
    v = findGold(an, bn, alpha);
    return v;
  }

/**
 * Function evaluation
 *
 * @param alpha shift
 *
 * @return function value
 *
 */
  T uni(T alpha)
  {
    T v;
    VecUtils::vecSaxpy(mObj->getDim(), mXStart, mDir, alpha, mCurr);
    v = mObj->func(mCurr);
    return v;
  }

  Objective < T > * mObj;

  T mFMin;

  T mAlpha;
  
  T mStep;

  T* mDir;

  T* mXStart;

  T* mCurr;

  T mEps;
};

/**
 * Class for local optimization based on going to the opposite direction
 * to the gradient. From the starting point search goes in the direction
 * -DF, where DF is the first direvation (gradient).
 *
 * @author Mikhail Posypkin, ISA RAS, posypkin@isa.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 */
template <class Uni> class GradLocalOpt {
 public:

/**
 * The constructor
 *
 * @param xstart starting point
 * @param eps tolerance (for the gradient norm)
 * @param obj objective function
 */
  GradLocalOpt(typename Uni::FloatType * xstart, 
	       typename Uni::FloatType eps, 
	       Objective <typename Uni::FloatType>* obj)
  {
    mXStart = new typename Uni::FloatType [obj->getDim()];
    mXFinal = new typename Uni::FloatType [obj->getDim()];
    for(int i = 0; i < obj->getDim(); i ++)
      mXStart[i] = xstart[i];
    mEps = eps;
    mObj = obj;
    mMaxIters = 100;
  }

  ~GradLocalOpt()
   {
     delete [] mXStart;
     delete [] mXFinal;
   }

/**
 * Find minimum *
 */
  void search()    
  {
    typename Uni::FloatType *xnew, *grad;
    typename Uni::FloatType norm;

    xnew = new typename Uni::FloatType[mObj->getDim()];
    grad = new typename Uni::FloatType[mObj->getDim()];
    for(int i = 0; i < mObj->getDim(); i ++)
      xnew[i] = mXStart[i];
    for(int i = 0; i < mMaxIters; i ++) {
      mObj->grad(xnew, grad);
      norm = VecUtils::vecNormTwo(mObj->getDim(), grad);
      if(norm < mEps)
	break;
      VecUtils::vecMult(mObj->getDim(), grad, -1./norm, grad);
      Uni uo(xnew, grad, mObj);
      uo.search();
      VecUtils::vecSaxpy(mObj->getDim(), xnew, grad, uo.getAlpha(), xnew);
    }
    for(int i = 0; i < mObj->getDim(); i ++)  
      mXFinal[i] = xnew[i];
    mFMin = mObj->func(mXFinal);
    delete [] xnew;
    delete [] grad;
  }

/**
 * @return minimum value
 *
 */
  typename Uni::FloatType getMinimum()
  {
    return mFMin;
  }
  
/**
 * @return The solution point
 *
 */
  typename Uni::FloatType* getFinal()
  {
    return mXFinal;
  }

 private:
  
  Objective <typename Uni::FloatType> *mObj;

  typename Uni::FloatType *mXStart;

  typename Uni::FloatType *mXFinal;

  typename Uni::FloatType mFMin;

  typename Uni::FloatType mEps;

  int mMaxIters;

};

#endif
