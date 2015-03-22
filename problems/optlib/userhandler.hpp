#ifndef __USER_HANDLER__
#define __USER_HANDLER__

template <typename FT> class UserHandler {
  public:
    
    /**
     * Back call
     * @param x point
     * @param g grad
     */
    virtual void debug(FT* x, FT* g) = 0;
};
#endif
