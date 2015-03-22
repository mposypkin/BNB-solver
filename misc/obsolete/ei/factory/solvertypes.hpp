#ifndef _SOLVERTYPES_HPP_
#define _SOLVERTYPES_HPP_
/**
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file solvertypes.hpp
 *
 * Supported solver types
 *
 */


/**
  * Parallel knapsack solver
 */
#define BNB_PARALLEL_KNAP_SOLVER "bnbparknap"

/**
 * Parallel Basin Hopping Solver
 */
#define BNB_PARALLEL_MBH_SOLVER "parmbh"

/**
  * Parallel Multistart Solver
  */
#define BNB_PARALLEL_MULTISTART_SOLVER "parmulti"

/**
  * Crypto solver
  */
#define CRYPTO_SOLVER "crypto"

/**
  * Crypto solver
  */
#define KNAPDP_SOLVER "knapdp"

/**
  * Serial local search solver
  */
#define BNB_SLOC_SOLVER "sloc"

/**
 * Just emulate solver
 */
#define BNB_DUMMY_SOLVER "dummy"

#endif
