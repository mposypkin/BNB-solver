/* 
 * File:   rndfill.hpp
 * Author: Mikhail Posypkin
 *
 * Created on June 10, 2014, 9:49 PM
 */

#ifndef RNDFILL_HPP
#define	RNDFILL_HPP

#include <cstdlib>
#include "boxfill.hpp"


template <class FT> class RndFill : public BoxFill <FT> {
    public:
        typedef BoxFill <FT> BASE;
        /**
         * Constructor
         * @param box box to generate points in
         */
        RndFill(const Box<FT>& box) : BoxFill <FT> (box) 
        {           
        }
        
        void getPoint(FT* x)
        {
            int n = BASE::mBox.mDim;
            for(int i = 0; i < n; i ++) {
                FT a = BASE::mBox.mA[i];
                FT b = BASE::mBox.mB[i];
                FT l = b - a;
                x[i] = a + l * ((FT)rand() / (FT) RAND_MAX);                
            }
        }
};


#endif	/* RNDFILL_HPP */

