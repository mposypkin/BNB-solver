/* 
 * File:   commi.hpp
 * Author: posypkin
 *
 * Created on March 1, 2011, 8:56 PM
 *
 * This file contains the definition of an abstract communicator interface that
 * is used for passing messages among nodes.
 */

#ifndef COMMUNICATOR_HPP
#define	COMMUNICATOR_HPP

#include "buffer.hpp"


// Communication succeeded error code
#define BNB_COMM_SUCCESS 0

// Communication error code
#define BNB_COMM_ERROR -1

class Communicator {
public:

    /**
     * Returns the rank of the calling process
     * @return rank
     */
    virtual int rank() const = 0;

    /**
     * Returns the number of processes in the communicator
     * @return number of processes
     */
    virtual int size() const = 0;

    /**
     * Sends the buffer to the destination process.
     * @param buf buffer to send
     * @param dest the destination process number
     * @return 0 if no error detected, the error code otherwise
     */
    virtual int send(const Buffer& buf, int dest) = 0;

    /**
     * Checks the message from the process with the number source
     * @param source the number of process that is checked for the message
     * @return true if the message is available, false otherwise
     */
    virtual bool probe(int source = -1) = 0;

    /**
     * Recieves message from the process and stores it in a buffer
     * @param buf buffer to store the message
     * @param source the source process (-1 for template)
     * @return rank (positive) if no error detected, the error code otherwise
     */
    virtual int recv(Buffer& buf, int source = -1) = 0;

    /**
     * Broadcasts the data from the current process to all other processes
     * Should be called on all processes in the communicator.
     * @param buf buffer to send
     * @return 0 if no error detected, the error code otherwise
     */
    virtual int bcast(Buffer& buf) = 0;

    /**
     * @return recieved bytes
     */
    virtual long long int getRecvBytes() const = 0;

    /**
     * @return sent bytes
     */
    virtual long long int getSentBytes() const = 0;


};


#endif	/* COMMUNICATOR_HPP */

