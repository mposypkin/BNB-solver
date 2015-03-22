#ifndef __GVECTOR_HPP__
#define __GVECTOR_HPP__

template <class T> class GVector {
  public:
    virtual void push_back(const T& a) = 0;

    virtual void pop_back() = 0;

    virtual const T& back() const = 0;

    virtual bool empty() const = 0;
    
    virtual int size() const = 0;

    virtual T& operator [ ] (int i) = 0;

    virtual const T& operator [ ] (int i) const = 0;

    virtual void clear() = 0;

};

#endif
