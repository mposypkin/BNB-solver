#ifndef __SEGMENT_HPP__
#define __SEGMENT_HPP__

/**
  * Uni-dimensional segment (interval)
  */
template <class FT> struct Segment {
  /**
    * Left end
    */
  FT mA;
  /**
    * Right end
    */
  FT mB;
};
#endif

