/* 
 * File:   testjson.cpp
 * Author: posypkin
 *
 * Test JSON parsers and writers
 * Created on August 20, 2012, 10:49 AM
 */

#include "knaputils.hpp"

#define JSON_ISTR "{\n"\
"\"N\" : 4,\n"\
"\"C\" : 9,\n"\
"\"W\" : [5, 3, 2, 7],\n"\
"\"P\" : [6, 7, 5, 3]\n"\
"}"

#define JSON_OSTR "{\n"\
"\"N\" : 4,\n"\
"\"C\" : 9,\n"\
"\"W\" : [2, 3, 5, 7],\n"\
"\"P\" : [5, 7, 6, 3]\n"\
"}"


/*
 * 
 */
int main(int argc, char** argv) {

    std::string json = JSON_ISTR;
    KnapData<int> kd;
    KnapUtils<int>::readJson(json, kd);
    KnapUtils<int>::sort(kd);
    std::string s = KnapUtils<int>::writeJson(kd).c_str();
    //printf("%s\n", s.c_str());
    if(s != JSON_OSTR) {
        BNB_ERROR_REPORT("ERROR");
    } else {
        return 0;
    }
}

