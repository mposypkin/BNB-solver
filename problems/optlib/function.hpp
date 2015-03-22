#ifndef __FUNCTION_HPP__
#define __FUNCTION_HPP__

template <typename FT> class Function  {
  public:
      
  /**
   * The objective function itself
   *
   * @param x argument
   *
   * @return function value
   *
   */
  virtual FT func(const FT* x) = 0;

};

#endif
