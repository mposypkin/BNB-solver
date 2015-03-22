#ifndef __PERTRUBER_HPP__
#define __PERTRUBER_HPP__


template <typename FT> class Pertruber {
  public:
    
    /**
     * Perform pertrubation of vector x
     *
     * @param i number of iteration
     * @param x source vector
     * @param y destination vector
     */
    virtual void pertrub(int i, FT *x, FT *y) = 0;
};
#endif

