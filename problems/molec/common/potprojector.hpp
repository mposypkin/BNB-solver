#ifndef __POTPROJECTOR_HPP__
#define __POTPROJECTOR_HPP__

#include <problems/optlib/projector.hpp>

/**
 * Projector for molecular potential 
 */
class PotProjector : public Projector <double> 
{
  public:
    PotProjector(int sd)
    {
      mSpaceD = sd;
    }
  
    void project(int n, double* x)
    {
      if(mSpaceD == 1)
        x[0] = 0.;
      else if(mSpaceD == 2) {
        x[0] = 0.;
        x[1] = 0.;
        x[2] = 0.;
      } else if(mSpaceD == 3) {
        x[0] = 0.;
        x[1] = 0.;
        x[2] = 0.;
        x[3] = 0.;
        x[4] = 0.;
        x[6] = 0.;
      }
    }
  
  private:
    int mSpaceD;
};
#endif
