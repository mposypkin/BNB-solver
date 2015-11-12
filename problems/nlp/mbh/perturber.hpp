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
    virtual void perturb(const FT * x, FT * y) = 0;
    
    /**
     * Resets perturber's parameters. To be called at the search beginning.
     */
    virtual void reset() {
        
    }
    /**
     * Answers continue or not iterations
     * @return true if continue false if stop
     */
    virtual bool cont() const {
        // Should be overwritten by user for complex pathologies
        return false;
    }
    
    /**
     * Called when algorithm failed to improve the value after given number of tries
     * @param ntries number of unsuccessful tries
     */
    virtual void fail(int ntries) {
        // User provides implementation if needed
    }
    
    /**
     * Called when the objective was improved
     * @param ntries number of tries before improve
     * @param pv previous value
     * @param nv improved value
     */
    virtual void success(int ntries, FT pv, FT nv) {
        // User provides implementation if needed        
    }
};

#endif	/* PERTUBER_HPP */

