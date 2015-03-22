#ifndef __PROJECTOR_HPP__
#define __PROJECTOR_HPP__

/**
 * Perform projection on a given set
 */
template <typename FT> class Projector {
  public:
  /**
   * Perform projection
   * @param n dimesionality
   * @param x vector to project
   */
  virtual void project(int n, FT* x) = 0;
};

#endif
