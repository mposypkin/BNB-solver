#ifndef __BOXFACTORY_HPP__
#define __BOXFACTORY_HPP__

/**
 * Box constraints
 */
#define BNB_BOX_TAG "box"

/**
 * Box dimension
 */
#define BNB_BOX_DIM_TAG "dim"

/**
 * Box lower bound
 */
#define BNB_BOX_LOWER_TAG "a"

/**
 * Box upper bound
 */
#define BNB_BOX_UPPER_TAG "b"

class BoxFactory {
  public:
    static void create(std::string description, GOInitialData<double> *id) {
      int dim;
      double a, b;
      XMLPars pars;
      std::map<std::string, std::string> tls;
      pars.parse(description, tls);            
      if(tls.find(BNB_BOX_DIM_TAG) == tls.end()) {
        BNB_ERROR_REPORT("Box dimension not set");
      } else{
        dim = atoi(tls[BNB_BOX_DIM_TAG].c_str());
      }
      if(tls.find(BNB_BOX_LOWER_TAG) == tls.end()) {
        BNB_ERROR_REPORT("Box lower bound not set");
      } else{
        a = atof(tls[BNB_BOX_LOWER_TAG].c_str());
      }
      if(tls.find(BNB_BOX_UPPER_TAG) == tls.end()) {
        BNB_ERROR_REPORT("Box upper bound not set");
      } else{
        a = atof(tls[BNB_BOX_UPPER_TAG].c_str());
      }
      id->mA = (double*) malloc(dim * sizeof(double));
      id->mB = (double*) malloc(dim * sizeof(double));
      for(int i = 0; i < dim; i ++) {
        id->mA[i] = a;
        id->mB[i] = b;
      }
    }
};

#endif
