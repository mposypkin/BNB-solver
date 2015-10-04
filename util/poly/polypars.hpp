/* 
 * File:   polypars.hpp
 * Author: posypkin
 * Parsing Polynomial
 *
 * Created on April 21, 2011, 5:37 AM
 */

#ifndef POLYPARS_HPP
#define	POLYPARS_HPP

#include <ctype.h>
#include <math.h>
#include <map>
#include <iostream>
#include "monom.hpp"
#include "polynom.hpp"
#include "util/common/bnberrcheck.hpp"

/**
 * Parsing Polynomial values 
 */
class PolyPars {
public:

    /**
     * Parses the string and returns the monomial
     *
     * @param str the string representation
     * @param idents the ident table
     * @param sign (1 or -1)
     * @param p the displacement for the next char
     * @return parsed monomial
     */
    template <typename FT> static Monom<FT> parseMonom(const char* str, std::map<std::string, int>& idents, int sign, int *p) {
        char c = str[*p];
        FT coe;
        int nvars = 0;
        bool atstart = true;
        unsigned int deg[MAX_POLYNOM_VARS];

        for (int i = 0; i < MAX_POLYNOM_VARS; i++)
            deg[i] = 0;

        coe = sign;
        
        while (true) {
            c = str[*p];
            if (issign(c)) {
                break;
            } else if (isspace(c)) {
                (*p)++;
            } else if (isalpha(c)) {
                int i;
                i = parseIdent(str, idents, p);
                deg[i] = parseDegree(str, p);
                atstart = false;
            } else if (atstart && isdigit(c)) {
                coe *= parseConst(str, p);
                atstart = false;
            } else
                break;
        }
        nvars = idents.size();
        Monom<FT> mon(nvars, coe, deg);
              
        return mon;
    }

    /**
     * Parses the polynomial from string
     * @param str the string representation of the polynomial
     * @param idents the table of identifiers
     * @param poly the polynomial to fill
     */
    template <typename FT> static void parsePolynom(const char* str, std::map<std::string, int>& idents, Polynom<FT>& poly) {
        int p = 0;
        int sign = 1;
        bool atstart = true;

        while (true) {
            char c = str[p];
            if (isspace(c)) {
                p++;
            } else if (issign(c)) {
                if (c == '+')
                    sign = 1;
                else
                    sign = -1;
                p++;
                Monom<FT> mon = parseMonom<FT > (str, idents, sign, &p);
                poly.addMonom(mon);
                atstart = false;
            } else if (atstart && isalnum(c)) {
                Monom<FT> mon = parseMonom<FT > (str, idents, sign, &p);
                poly.addMonom(mon);
                atstart = false;
            } else if (c == 0) {
                break;
            } else {
                BNB_ERROR_REPORT("Unrecognized symbol in polynomial");
            }
        }
        poly.alignMonoms();
    }

private:

    static bool issign(char c) {
        if ((c == '+') || (c == '-'))
            return true;
        else
            return false;
    }

    static int char2digit(char c) {
        int v;
        switch (c) {
            case '0': v = 0;
                break;
            case '1': v = 1;
                break;
            case '2': v = 2;
                break;
            case '3': v = 3;
                break;
            case '4': v = 4;
                break;
            case '5': v = 5;
                break;
            case '6': v = 6;
                break;
            case '7': v = 7;
                break;
            case '8': v = 8;
                break;
            case '9': v = 9;
                break;
            default:
                BNB_ERROR_REPORT("Non-digit");
        }
        return v;
    }

    static double parseConst(const char* str, int* p) {
        char c;
        double v = 0.;
        int afterdot = 0;
        while (true) {
            c = str[*p];
            if (isdigit(c)) {
                if (!afterdot) {
                    v = v * 10 + char2digit(c);
                } else {
                    v += char2digit(c) * pow(10., -afterdot);
                    afterdot++;
                }
                (*p)++;
            } else if (c == '.') {
                (*p)++;
                afterdot = 1;
            } else {
                break;
            }
        }
        return v;
    }

    static int parseIdent(const char* str, std::map<std::string, int>& idents, int *p) {
        char c;
        std::string name;
        do {
            c = str[*p];
            if (isalnum(c))
                name.append(1, c);
            else
                break;
            (*p)++;
        } while (true);
        std::map<std::string, int>::iterator i = idents.find(name);
        if (i != idents.end())
            return i->second;
        else {
            int n = idents.size();
            idents[name] = n;
            return n;
        }
    }

    static int parseDegree(const char* str, int* p) {
        char c;
        int d = 1;
        bool atstart = false;
        while (true) {
            c = str[*p];
            if (isspace(c)) {
                (*p)++;
            } else if (c == '^') {
                (*p)++;
                atstart = true;
            } else if (isdigit(c) && atstart) {
                d = parseConst(str, p);
                break;
            } else {
                break;
            }
        }
        return d;
    }

};

#endif	/* POLYPARS_HPP */

