#ifndef __BNBACTIONS_HPP__
#define __BNBACTIONS_HPP__

struct BNBActions {
  
  static const int NOACTION = 0;
  
  static const int FORWARD = 1;

  static const int ERASE = 1 << 1;
  
  static const int REPEAT = 1 << 2;
  
  static const int SKIP = 1 << 3;
  
  static const int DROP = 1 << 4;

  static const int EXIT = 1 << 5;
};

#endif
