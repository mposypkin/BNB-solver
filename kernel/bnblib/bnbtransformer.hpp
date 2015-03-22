#ifndef __BNBTRANSFORMER_HPP__
#define __BNBTRANSFORMER_HPP__


template <class Solution> class BNBTransformer {
  public:
  /**
   * TRansform solution
   * @param sol solution to transform
   */
  virtual void transform(Solution& sol) = 0;
};
#endif
