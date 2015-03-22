/* 
 * File:   knaputils.hpp
 * Author: posypkin
 *
 * Created on August 18, 2012, 5:55 PM
 */

#ifndef KNAPUTILS_HPP
#define	KNAPUTILS_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <libjson/libjson.h>
#include "util/common/bnberrcheck.hpp"
#include "knapdata.hpp"

#define JSON_KNAP_N "N"

#define JSON_KNAP_C "C"

#define JSON_KNAP_W "W"

#define JSON_KNAP_P "P"

template <class T> class KnapUtils {
public:

    /**
     * Writes JSON description to the string
     * @param kd knapsack data
     * @return resuling string
     */
    static std::string writeJson(KnapData< T >& kd) {
        std::ostringstream os;
        os << "{\n";
        os << "\"N\" : " << kd.mN << ",\n";
        os << "\"C\" : " << kd.mC << ",\n";
        os << "\"W\" : [";
        for (int i = 0; i < kd.mN; i++) {
            os << kd.mCoe[i].mW;
            if (i != (kd.mN - 1))
                os << ", ";
        }
        os << "],\n";
        os << "\"P\" : [";
        for (int i = 0; i < kd.mN; i++) {
            os << kd.mCoe[i].mP;
            if (i != (kd.mN - 1))
                os << ", ";
        }
        os << "]\n";
        os << "}";
        return os.str();
    }

    /**
     * Reads the description from the string
     * @param json the json description as a string
     * @param kd the resulting knapsack data
     */
    static void readJson(const std::string& json, KnapData< T >& kd) {

        struct {
            bool mNSet;
            bool mCSet;
            bool mWSet;
            bool mPSet;
        } checklist = {false, false, false, false};

        JSONNode nd = libjson::parse(json);
        JSONNode::const_iterator i = nd.begin();
        kd.mN = 0;
        kd.mTP = 0;
        kd.mTW = 0;
        kd.mCoe = NULL;


        while (i != nd.end()) {
            std::string name = i->name();
            //std::cout << "node \"" << name << "\"\n";
            if (name == JSON_KNAP_N) {
                kd.mN = i->as_int();
                kd.mCoe = new KnapRecord<T>[kd.mN];
                BNB_ASSERT(kd.mCoe);
                checklist.mNSet = true;
            } else if (name == JSON_KNAP_C) {
                kd.mC = i->as_int();
                checklist.mCSet = true;
            } else if (name == JSON_KNAP_W) {
                BNB_ASSERT(kd.mCoe);
                parseW(*i, kd, checklist.mPSet);
                checklist.mWSet = true;
            } else if (name == JSON_KNAP_P) {
                BNB_ASSERT(kd.mCoe);
                parseP(*i, kd, checklist.mWSet);
                checklist.mPSet = true;
            }
            i++;
        }
    }

    /**
     * Reads JSON description from file
     * @param fname file name to read from
     * @param kd knapsack data to fill
     */
    static void readJSONfromFile(char* fname, KnapData<T>& kd) {
        std::string json;
        std::ifstream is(fname);
        if (is.is_open()) {
            while (!is.eof()) {
                std::string tmp;
                is >> tmp;
                json += tmp;
            }
            is.close();
            //        printf("%s\n", json.c_str());
            KnapUtils<T>::readJson(json, kd);
        } else {
            BNB_ERROR_REPORT("Unable to open the data file\n");
        }

    }

    /**
     * Sorts knapsack coefficeints according to decreasing efficiency
     * @param kd knapsack data
     */
    static void sort(KnapData<T> &kd) {
        qsort(kd.mCoe, kd.mN, sizeof (KnapRecord<T>), compare);

    }

private:

    static void parseW(const JSONNode& nd, KnapData < T > & kd, bool pset) {
        JSONNode::const_iterator i = nd.begin();
        int j = 0;
        while (i != nd.end()) {
            BNB_ASSERT(j < kd.mN);
            kd.mCoe[j].mW = i->as_int();
            kd.mCoe[j].mI = j;
            kd.mTW += kd.mCoe[j].mW;
            if (pset)
                kd.mCoe[j].mE = (double) kd.mCoe[j].mP / (double) kd.mCoe[j].mW;
            j++;
            i++;
        }
    }

    static void parseP(const JSONNode& nd, KnapData < T > & kd, bool wset) {
        JSONNode::const_iterator i = nd.begin();
        int j = 0;
        while (i != nd.end()) {
            BNB_ASSERT(j < kd.mN);
            kd.mCoe[j].mP = i->as_int();
            kd.mTP += kd.mCoe[j].mP;
            if (wset)
                kd.mCoe[j].mE = (double) kd.mCoe[j].mP / (double) kd.mCoe[j].mW;
            j++;
            i++;
        }
    }

    static int compare(const void* a, const void* b) {
        int rv;
        double ea = ((KnapRecord<T>*)a)->mE;
        double eb = ((KnapRecord<T>*)b)->mE;
        double C = ea - eb;
        if (C > 0)
            rv = -1;
        else if (C < 0)
            rv = 1;
        else
            rv = 0;
        return rv;
    }

};

#endif	/* KNAPUTILS_HPP */

