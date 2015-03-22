#ifndef __BNBDIVERS_HPP__
#define __BNBDIVERS_HPP__

template <class Solution> class BNBDivers 
{
  public:
  /**
   * Checks whether two solution are really different (belongs to different domains)
   * @param s1 first solution
   * @param s2 second solution
   * @return true if same
   */
  virtual bool same(const Solution s1, Solution s2) = 0;
};

#endif
