#ifndef __POLYNOMFMT_HPP__
#define __POLYNOMFMT_HPP__

#include <string>

/**
  * Abstract class for polynomial formatting
  */


class PolynomFmt {
  public:
    /**
      * Returns plus sign
      */
    virtual std::string getPlus() const = 0;

    /**
      * Returns minus sign
      */
    virtual std::string getMinus() const = 0;

    /**
      * Returns multiplication sign
      */
    virtual std::string getMult() const = 0;

    /**
      * Returns degree sign
      */
    virtual std::string getDegree() const = 0;

    /**
      * Returns index sign
      */
    virtual std::string getIndex() const = 0;

    /**
      * Returns left brace
      */
    virtual std::string getLbrace() const = 0;

    /**
      * Returns right brace
      */
    virtual std::string getRbrace() const = 0;
};
#endif
