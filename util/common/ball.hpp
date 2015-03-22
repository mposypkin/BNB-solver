/* 
 * File:   ball.hpp
 * Author: medved
 * Defines a ball 
 * Created on November 1, 2014, 2:50 PM
 */

#ifndef BALL_HPP
#define	BALL_HPP

#include "smartptr.hpp"
/**
  * Class for handling boxes
  */

template <class FT> struct Ball {
    
    struct Metric {
        static const unsigned int L2 = 1;
        static const unsigned int L1 = 2;
    };
    
  Ball(int dim = 0) : mX(dim), mR(0), mType(Metric::L2)
  {
    mDim = dim;
  }

 
  /**
    * Ball number of dimensions
    */
  int mDim;

  /**
   * Ball Center
   */
  SmartArrayPtr<FT> mX;

  /**
   * Ball Radius
   */
  FT mR;
  
  /**
   * Ball (metric) type
   */
  unsigned int mType;
};



#endif	/* BALL_HPP */

