/* 
 * File:   locsearch.hpp
 * Author: medved
 *
 * Created on February 9, 2015, 8:35 PM
 */

#ifndef LOCSEARCH_HPP
#define	LOCSEARCH_HPP

/**
 * Generic local search to use with cuts
 */
template <class FT> class LocSearch {
    public:
        /**
         * Search for minimum
         * @param x the initial (input) and resulting (output) vectors
         * @return resulting value
         */
        virtual FT search(FT* x) = 0;
};

#endif	/* LOCSEARCH_HPP */

