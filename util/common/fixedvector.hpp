#ifndef __FIXEDVECTOR_HPP__
#define __FIXEDVECTOR_HPP__

#include "utilmacro.hpp"
#include "bnberrcheck.hpp"
#include "gvector.hpp"
/**
  * A vector of fixed length
  */
template <class T> class FixedVector: public GVector < T >{
  public:
    FixedVector(const FixedVector& v)
    {
      BNB_ERROR_REPORT("Forbidden to clone fixed vector!\n");
    }

    FixedVector() 
    {
      mP = NULL;
      mMaxSize = 0;
      mSize = 0;
    }


    FixedVector(T* vec, unsigned int msize)
    {
      mP = vec;
      mMaxSize = msize;
      mSize = 0;
    }

    FixedVector operator = (const FixedVector& v)
    {
      unsigned int sz = v.size();
      resize(sz);
      for(int i = 0; i < sz; i ++) 
	mP[i] = v[i];
    }

    void setCarrier(T* vec, unsigned int msize)
    {
      mP = vec;
      mMaxSize = msize;
      mSize = 0;
    }


    void resize(unsigned int nsz)
    {
      BNB_ASSERT(nsz <=mMaxSize);
      mSize = nsz;
    }

    void push_back(const T& a)
    {
      BNB_ASSERT(mSize < mMaxSize);
      mP[mSize] = a;
      mSize ++;
    }

    void pop_back() 
    {
      mSize = BNBMAX(0, mSize - 1);
    }

    const T& back() const 
    {
      BNB_ASSERT(mSize > 0);
      return mP[mSize - 1];
    }

    bool empty() const 
    {
      return (mSize == 0);
    }

    int size() const 
    {
      return mSize;
    }

    T& operator [ ] (int i) 
    {
      return mP[i];
    }

    const T& operator [ ] (int i) const 
    {
      return mP[i];
    }

    void clear() 
    {
      mSize = 0;
    }

    void insert(int pos, const T& a)
    {
      for(int i = mSize; i > pos; i --) 
	mP[i] = mP[i - 1];
      mP[pos] = a;
      mSize ++;
    }

    void erase(int pos)
    {
      for(int i = pos; i < (mSize - 1); i ++)
	mP[i] = mP[i + 1];
      mSize --;
    }

  private:

    unsigned int mSize;
    
    unsigned int mMaxSize;

    T* mP;

};


#endif

