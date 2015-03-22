/**
 * Auxilary module for static variables of bnblog
 *
 * @author Mikhail Posypkin, ISA RAS, posypkin@isa.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file bnblog.cpp
 */

#include "bnblog.hpp"

BNBLog<BNB_DO_LOGGING>* BNBLog<BNB_DO_LOGGING>::mUniLog = NULL;

int BNBLog<BNB_DO_LOGGING>::mProcessNumber = 0;