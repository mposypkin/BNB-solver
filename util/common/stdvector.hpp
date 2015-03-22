#ifndef __STDVECTOR_HPP__
#define __STDVECTOR_HPP__

#include <vector>
#include "gvector.hpp"

template <class T> class StdVector : public GVector <T>
{
  public:
    void push_back(const T& a) {
      mBase.push_back(a);
    }

    void pop_back() {
      mBase.pop_back();
    }

    const T& back() const 
    {
      return mBase.back();
    }

    bool empty() const 
    {
      return mBase.empty();
    }

    int size() const
    {
      return mBase.size();
    }

    T& operator [ ] (int i) 
    {
      return mBase[i];
    }
    
    const T& operator [ ] (int i) const 
    {
      return mBase[i];
    }

    void clear()
    {
      mBase.clear();
    }


  private:
    std::vector<T> mBase;
};

#endif
