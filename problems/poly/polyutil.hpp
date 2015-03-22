/* 
 * File:   polyutil.hpp
 * Author: posypkin
 *
 * Different utils for polynomials
 *
 * Created on April 21, 2011, 2:56 AM
 */

#ifndef POLYUTIL_HPP
#define	POLYUTIL_HPP

#include <string>
#include <map>
#include "polypars.hpp"
#include "polynomfmt.hpp"

class PolyUtil {
public:

    /**
     * Constructs polynome from a string in the following format:
     *
     *  +5 x1^6 x2^7 - 3 x2^3 x3 + 4
     *
     * @param str string representing a plynomial
     * @param idents represents constructed idents
     * @param poly plynom
     */
    template <typename FT> static void fromString(const char* str, std::map<std::string, int>& idents, Polynom<FT>& poly) {
        //std::map<std::string, int> idents;
        PolyPars::parsePolynom(str, idents, poly);
    }

    /**
     * Produces the string representation for the polynomial
     *
     * @param fmt formatter
     * @param poly polynomial
     * @return
     */
    template <typename FT> static std::string toString(const PolynomFmt& fmt, const Polynom<FT>& poly) {
        std::ostringstream os;
        typename std::vector< Monom<FT> >::const_iterator i;
        int ms = poly.getMonoms().size();
        for (int i = 0; i < ms; i++) {
            if (i != 0)
                os << " ";
            if (poly.getMonoms()[i].getCoe() < 0)
                os << fmt.getMinus();
            else if (i != 0)
                os << fmt.getPlus();
            if (i != 0)
                os << " ";
            os << toString(fmt, poly.getMonoms()[i]);
        }
        return os.str();

    }

    /**
     * Produces the string representation for the monomial
     *
     * @param fmt formatter
     * @param poly monomial
     * @return
     */
    template <typename FT> static std::string toString(const PolynomFmt& fmt, const Monom<FT>& mon) {
        std::ostringstream os;
        bool notfirst = false;
        int deg = 0;
        if (mon.getCoe() != 0) {
            FT acoe = BNBABS(mon.getCoe());
            if (acoe != 1) {
                os << acoe;
                notfirst = true;
            }
            for (int i = 0; i < mon.getNvars(); i++) {
                if (mon.getDeg()[i] != 0) {
                    deg += mon.getDeg()[i];
                    if (notfirst)
                        os << fmt.getMult();
                    os << fmt.getLbrace();
                    os << "x" << fmt.getIndex() << i;
                    if (mon.getDeg()[i] != 1)
                        os << fmt.getDegree() << mon.getDeg()[i];
                    os << fmt.getRbrace();
                    notfirst = true;
                }
            }
            if((deg == 0) && (acoe == 1)) {
                os << acoe;
            }
        }
        return os.str();
    }

};

#endif	/* POLYUTIL_HPP */

