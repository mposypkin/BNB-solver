#ifndef __POLYNOMTXT_HPP__
#define __POLYNOMTXT_HPP__

#include "polynomfmt.hpp"

class PolynomTxt : public PolynomFmt {
  public:
      std::string getPlus() const {
	return "+";
      }

      std::string getMinus() const {
	return "-";
      }

      std::string getMult() const {
	return " ";
      }

      std::string getDegree() const {
	return "^";
      }

      std::string getIndex() const {
	return "";
      }

      std::string getLbrace() const
      {
	return "";
      }

      std::string getRbrace() const 
      {
	return "";
      }

};

#endif
