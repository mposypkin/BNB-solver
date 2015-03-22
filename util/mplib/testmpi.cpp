/* 
 * File:   testmpi.cpp
 * Author: posypkin
 *
 * Created on April 17, 2011, 3:21 AM
 */

#include <cassert>
#include <string>
#include <iostream>

#include <mpi.h>

#include <util/common/bnberrcheck.hpp>

#include "mpicommunicator.hpp"
#include "binary_serializer.hpp"

#define MAGIC_NUMBER 1974

struct Data {
    int key;
    std::vector<int> array;
};

template <> void BinarySerializer::pack<Data>(const Data &data) {
    pack(data.key);
    pack(data.array);
}

template <> void BinarySerializer::unpack(Data &data) {
    data.array.clear();
    unpack(data.key);
    unpack(data.array);
}

void checkEqualData(const Data &lhs, const Data &rhs) {
    assert(lhs.key == rhs.key);
    assert(lhs.array.size() == rhs.array.size());
    for (size_t i = 0; i < lhs.array.size(); ++i) {
        assert(lhs.array[i] == rhs.array[i]);
    }
}

void testBinarySerializer() {
    Data originalData;
    originalData.key = 100;
    for (int i = 11; i < 20; ++i) {
        originalData.array.push_back(i);
    }

    BinarySerializer serializer;
    serializer << originalData;

    BinarySerializer deserializer((const char *) serializer.data(), serializer.size());
    Data targetData;
    deserializer >> targetData;

    checkEqualData(originalData, targetData);
}

void testCommunicator(Communicator* comm) {
    BinarySerializer ser;
    if (comm->rank() == 0) {
        int message = MAGIC_NUMBER;
        ser << message;
        comm->send(ser, 1);
    } else if(comm->rank() == 1) {
        int message;
        comm->recv(ser, 0);
        ser >> message;
        BNB_ASSERT(message == MAGIC_NUMBER);
    }
}

/**
 * Test for the MpiCommunicator
 */
int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);
    MpiCommunicator com;
    testBinarySerializer();
    testCommunicator(&com);
    MPI_Finalize();
}

