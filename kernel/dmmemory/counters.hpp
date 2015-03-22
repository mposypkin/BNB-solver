/* 
 * File:   counters.hpp
 * Author: posypkin
 *
 * Statistic counters
 *
 * Created on February 19, 2012, 4:02 PM
 */

#ifndef COUNTERS_HPP
#define	COUNTERS_HPP

#include <string>
#include <sstream>

struct DmCounters {

    DmCounters() {
        init();
    }
    /**
     * Initialization of counters     
     */
    void init() {
        sent_parcels = 0;
        sent_bytes = 0;
        sent_subs = 0;
        sent_records = 0;
        sent_commands = 0;
        recv_parcels = 0;
        recv_bytes = 0;
        recv_subs = 0;
        recv_records = 0;
        recv_commands = 0;
        steps_done = 0;
    }

    /**
     * Produces string output
     * @return resulting string representation
     */
    std::string toString() const {
        std::ostringstream os;
        os << sent_parcels << " " << sent_bytes << " " << sent_subs << " " << sent_records << " " << sent_commands << " " << recv_parcels
                << " " << recv_bytes << " " << recv_subs << " " << recv_records << " " << recv_commands << " " << steps_done;
        return os.str();
    }


    /**
     * Number of sent parcels
     */
    long long int sent_parcels;
    /**
     * Number of bytes sent
     */
    long long int sent_bytes;
    /**
     * Number of sent subproblems
     */
    long long int sent_subs;
    /**
     * Number of sent records
     */
    long long int sent_records;
    /**
     * Number of sent commands
     */
    long long int sent_commands;
    /**
     * Number of recieved parcels
     */
    long long int recv_parcels;
    /**
     * Number of received bytes
     */
    long long int recv_bytes;
    /**
     * Number of received subproblems
     */
    long long int recv_subs;
    /**
     * Number of received records
     */
    long long int recv_records;
    /**
     * Number of received commands
     */
    long long int recv_commands;
    /**
     * Number of steps performed
     */
    long long int steps_done;

};

#endif	/* COUNTERS_HPP */

