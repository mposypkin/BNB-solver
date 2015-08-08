/* 
 * File:   pertuber.hpp
 * Author: medved
 *
 * Created on August 4, 2015, 3:35 PM
 */

#ifndef PERTUBER_HPP
#define	PERTUBER_HPP

/**
 * Abstract class to generate perturbations for Monotonic Basin Hopping Algorithms
 */
template <class FT> class Perturber {
public:

    /**
     * Generates perturbed point
     * @param x source point
     * @param y resulting (perturbed) point
     */
    virtual void perturb(const FT * x, FT * y) const = 0;
};

#endif	/* PERTUBER_HPP */

