#ifndef __INFINI__
#define __INFINI__

#include <vector>
#include <string>
#include <sstream>
#include "fixedvector.hpp"


class InfiniInt {
  
  public:
    
  InfiniInt() : mData(mDataArray, mMaxDigits)
  {
    mBase = 10;
  }

  InfiniInt(const InfiniInt& a) : mData(mDataArray, mMaxDigits)
  {
    mBase = a.mBase;
    mData = a.mData;
  }

  
  
  InfiniInt(unsigned int num) : mData(mDataArray, mMaxDigits)
  {
    mBase  = 10;
    operator=(num);
  }
  
  
  /**
   * Assignement from the "normal" integer
   * @param number 
   * @return 
   */
  InfiniInt& operator = (unsigned int number)
  {
    unsigned int i;
    
    i = 0;
    do {
      unsigned int rem;
      
      rem = number % mBase;
      if(i < mData.size())
        mData[i] = rem;
      else
        mData.push_back(rem);
      i ++;
      number /= mBase;
    } while(number != 0);
    mData.resize(i);
    return *this;
  }
  
  /**
   * Summation
   * @param operand operand to sum
   * @return the result
   */
  InfiniInt operator + (InfiniInt& operand) const
  {
    unsigned int m, i, res;
    InfiniInt n;
    m = 0;
    i = 0;
    if((!isInfty()) && (!operand.isInfty())) {
      do {
        res = m;
        if(i < mData.size())
          res += mData[i];
        if(i < operand.getData().size())
          res += operand.getData()[i];
        if(res < mBase) {
          n.getData().push_back(res);
          m = 0;
        } else {
          unsigned int rem;
          rem = res % mBase;
          m = res / mBase;
          n.getData().push_back(rem);
        }
        i ++; 
      } while ((i < mData.size()) || (i  < operand.getData().size()) || (m != 0));
    }
    return n;
  }
  
  InfiniInt& operator ++ ()
  {
    int i;
    
    i = 0;
    if(!isInfty()) {
      do {
        if(i >= mData.size()) {
          mData.push_back(1);
          break;
        } else if(mData[i] < mBase - 1) {
          mData[i] ++;
          break;
        } else {
          mData[i] = 0;
          i ++;
        }
      } while(1);
    }
    return *this;
  }
  
  InfiniInt operator ++ (int a)
  {
    int i;
    InfiniInt tmp;
    tmp = *this;  
    i = 0;
    if(!isInfty()) {
      do {
        if(i >= mData.size()) {
          mData.push_back(1);
          break;
        } else if(mData[i] < mBase - 1) {
          mData[i] ++;
          break;
        } else {
          mData[i] = 0;
          i ++;
        }
      } while(1);
    }
    return tmp;
  }

  bool operator == (InfiniInt operand)
  {
    if(mData.size() != operand.getData().size())
      return false;
    else {
      int i;
      for(int i = 0; i < mData.size(); i ++)
        if(mData[i] != operand.getData()[i])
          return false;
      return true;
    }
  }

  bool operator != (InfiniInt operand)
  {
    return ! operator==(operand);
  }
  
  bool operator < (InfiniInt operand)
  {
    if(isInfty())
      return false;
    else if(operand.isInfty())
      return true;
    else if(operand.getData().size() < mData.size())
      return false;
    else if(operand.getData().size() > mData.size())
      return true;
    else {
      int i;
      i = mData.size() - 1;
      do {
        if(mData[i] < operand.getData()[i])
          return true;
        else if(mData[i] > operand.getData()[i])
          return false;
        else
          i --;
      } while(i >= 0);
      return false;
    }
  }
  
  bool operator > (InfiniInt operand)
  {
    return operand.operator<(*this);
  }
  
  
  
  bool operator <= (InfiniInt operand)
  {
    if(operand.isInfty())
      return true;
    else if(isInfty())
      return false;
    else if(operand.getData().size() < mData.size())
      return false;
    else if(operand.getData().size() > mData.size())
      return true;
    else {
      int i;
      i = mData.size() - 1;
      do {
        if(mData[i] < operand.getData()[i])
          return true;
        else if(mData[i] > operand.getData()[i])
          return false;
        else
          i --;
      } while(i >= 0);
      return true;
    }
  }
  
  bool operator >= (InfiniInt operand)
  {
    return operand.operator<=(*this);
  }
  
  /**
   * Check the value against infinity
   * @return true if the value is infinity false otherwise
   */
  bool isInfty() const
  {
    return (mData.size() == 0);
  }
  
  /**
   * Coner to string
   * @return conversion
   */
  std::string toString() const
  {
    std::ostringstream os;
    if(isInfty())
      os << "INF";
    else {
      for(int i = (mData.size() - 1); i >= 0; i --) {
        os <<  mData[i];
      }
    }
    return os.str();
  }
  
  /**
   * Initialize from string
   * @param str string ot initialize from
   */
  void fromString(std::string str)
  {
    mData.clear();
    for(int i = str.size() - 1; i >= 0 ; i --) {
      unsigned char c;
      c = str[i] - '0';
      mData.push_back((int)c);
    }
  }
  
  /**
   * Retrieve data  vector
   * @return data vector
   */
  FixedVector<int>& getData() 
  {
    return mData;
  }
  
  /*
  friend void AbstractCommunicator::pack <InfiniInt> (const InfiniInt * num);
  
  friend void AbstractCommunicator::unpack <InfiniInt> (InfiniInt * num);
  */   
  
    static const unsigned int mMaxDigits = 100;
    int mDataArray[mMaxDigits];
    FixedVector<int> mData;
    unsigned int mBase;
};
#endif
