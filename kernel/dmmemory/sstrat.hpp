/* 
 * File:   sstrat.hpp
 * Author: posypkin
 *
 * Search strategies for branch-and-bound
 *
 * Created on April 13, 2012, 10:51 AM
 */

#ifndef SSTRAT_HPP
#define	SSTRAT_HPP

/**
 * Different search strategies for branch-and-bound
 */
struct SearchStrategies {
    /**
     * No search strategy (used with heuristic)
     */
    static const int NOS = 0;
    /**
     * Depth-first search. Always split one of the deepest subproblems .
     */
    static const int DFS = 1;
    /**
     * Width-first search. Always split one of the subproblems with the minimal depth.
     */
    static const int WFS = 2;
    /**
     * Best-first search. Always split one of the subproblems with the best bound.
     */
    static const int BFS = 3;
};

#endif	/* SSTRAT_HPP */

