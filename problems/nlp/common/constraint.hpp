/* 
 * File:   constraint.hpp
 * Author: medved
 *
 * Generic class for constraints
 * 
 * Created on August 24, 2014, 1:03 PM
 */

#ifndef CONSTRAINT_HPP
#define	CONSTRAINT_HPP

#include <problems/optlib/objective.hpp>

/**
 * Class defining constraints
 */
template <class FT> struct Constraint  {
    
    /**
     * Constructor 
     * @param obj objective
     * @param typ constraint type
     */
    Constraint(Objective<FT>* obj, unsigned int typ) : 
    mObjective(obj), mType(typ)
    {
        
    }
    struct Types {
        /**
         * If set the constraint in inequality g(x) <= 0 
         */
        static const unsigned int INEQUALITY_CONSTRAINT = 1;
        /**
         * If set the constraint is equality h(x) = 0
         */
        static const unsigned int EQUALITY_CONSTRAINT = 1 << 1;
        /**
         * If set the constraint is linear and generic otherwise
         */
        static const unsigned int LINEAR = 1 << 2;
    };
    
    /**
     * Constraint function
     */
    Objective <FT>* mObjective;
    
    /**
     * Constaint type
     */
    unsigned int mType;
};

#endif	/* CONSTRAINT_HPP */

