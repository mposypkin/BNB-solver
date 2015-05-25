/* 
 * File:   runmisqsum.cpp
 * Author: medved
 *
 * Created on May 5, 2015, 1:38 PM
 */

#include "misqsumfact.hpp"


/*
 * 
 */
int main(int argc, char** argv) {

    MISqSumFactory fact(2, 100, 4);
    NlpProblem<double>* nlp = fact.getProb();
    return 0;
}

