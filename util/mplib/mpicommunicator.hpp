/* 
 * File:   mpicommunicator.hpp
 * Author: posypkin
 *
 * Created on April 16, 2011, 9:18 PM
 * This class implements the communicator interface for the MPI environment
 */

#ifndef MPICOMMUNICATOR_HPP
#define	MPICOMMUNICATOR_HPP

#include <mpi.h>
#include "communicator.hpp"
#include "util/common/bnberrcheck.hpp"

class MpiCommunicator : public Communicator {
public:

    /**
     * Constructs the MPI_Communicator. Should be performed on all processes
     * @param comm
     */
    MpiCommunicator(MPI_Comm comm = MPI_COMM_WORLD) {
        mComm = comm;
        mRecvBytes = 0;
        mSentBytes = 0;
    }

    /**
     * Returns the rank of the calling process
     * @return rank
     */
    int rank() const {
        int rk;
        MPI_Comm_rank(mComm, &rk);
        return rk;
    }

    /**
     * Returns the number of processes in the communicator
     * @return number of processes
     */
    int size() const {
        int sz;
        MPI_Comm_size(mComm, &sz);
        return sz;
    }

    /**
     * Sends the buffer to the destination process.
     * @param buf buffer to send
     * @param dest the destination process number
     * @return 0 if no error detected, the error code otherwise
     */
    int send(const Buffer& buf, int dest) {
        int rc = MPI_Send(buf.data(), buf.size(), MPI_BYTE, dest, 0, mComm);
        mSentBytes += buf.size();
        if(rc == MPI_SUCCESS)
            return BNB_COMM_SUCCESS;
        else
            return BNB_COMM_ERROR;
    }

    /**
     * Checks the message from the process with the number source
     * @param source the number of process that is checked for the message
     * @return true if the message is available, false otherwise
     */
    bool probe(int source = -1) {
        int flag = 0;
        MPI_Status st;
        for (int i = 0; i < MPI_PROBE_MAGIC_NUMBER; i++) {
            MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, mComm, &flag, &st);
            if (flag)
                break;
        }
        return (flag != 0);
    }

    /**
     * Recieves message from the process and stores it in a buffer
     * @param buf buffer to store the message
     * @param source the source process (-1 for template)
     * @return rank (positive) if no error detected, the error code (negative) otherwise
     */
    int recv(Buffer& buf, int source = -1) {
        int size;
        int rank;
        MPI_Status st;

        if (source == -1)
            rank = MPI_ANY_SOURCE;
        else
            rank = source;

        if (source == -1) {
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, mComm, &st);
            rank = st.MPI_SOURCE;
        } else
            MPI_Probe(source, MPI_ANY_TAG, mComm, &st);

        MPI_Get_count(&st, MPI_BYTE, &size);

        mRecvBytes += size;
        if (buf.resize(size)) {
            BNB_ERROR_REPORT("Too large buffer requested")
        }
        int rc = MPI_Recv(buf.data(), size, MPI_BYTE, rank, MPI_ANY_TAG, mComm, &st);
        if (rc == MPI_SUCCESS)
            return st.MPI_SOURCE;
        else
            return BNB_COMM_ERROR;
    }

    /**
     * Broadcasts the data from the current process to all other processes
     * Should be called on all processes in the communicator.     
     * @param buf buffer to send
     * @return 0 if no error detected, the error code otherwise
     */
    int bcast(Buffer& buf) {
        int rc = MPI_Bcast(buf.data(), buf.size(), MPI_BYTE, rank(), mComm);
    }

    /**
     * @return recieved bytes
     */
    long long int getRecvBytes() const {
        return mRecvBytes;
    }

    /**
     * @return sent bytes
     */
    long long int getSentBytes() const {
        return mSentBytes;
    }

private:

    MPI_Comm mComm;

    //Introduced because MPI_Probe usually is not able to get the message if calling once
    const static int MPI_PROBE_MAGIC_NUMBER = 8;

    long long int mRecvBytes;

    long long int mSentBytes;

};




#endif	/* MPICOMMUNICATOR_HPP */

