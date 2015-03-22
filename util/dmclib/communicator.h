#pragma once

#include <mpi.h>

#include "datatype.h"
#include "binary_serializer.hpp"

namespace dmc
{
class Communicator
{
	MPI_Comm comm_;
public:
	Communicator(MPI_Comm comm = MPI_COMM_WORLD) :
		comm_(comm)
	{
	}

	int rank() const
	{
		int rank;
		MPI_Comm_rank(comm_, &rank);
		return rank;
	}

	int size() const
	{
		int size;
		MPI_Comm_size(comm_, &size);
		return size;
	}

	int bcast(int root, void *data, int count, MPI_Datatype type) const
	{
		return MPI_Bcast(data, count, type, root, comm_);
	}

	template<typename T> int bcast(int root, T *data, int count) const
	{
		return bcast(root, data, count, MpiDatatype<T>());
	}

	template<typename T> int bcast(int root, T &data) const
	{
		return bcast(root, &data, 1, MpiDatatype<T>());
	}

	int bcast(int root, BinarySerializer &binser) const
	{
		size_t size = 0;
		if (rank() == root)
		{
			size = binser.size();
		}
		int res = bcast(root, size);
		if (res != MPI_SUCCESS)
			return res;
		binser.resize(size);
		return bcast(root, binser.get(), size);
	}

	int send(int dest, void *data, int count, MPI_Datatype type)
	{
		return MPI_Send(data, count, type, dest, 0, comm_);
	}

	template<typename T> int send(int dest, T *data, int count)
	{
		return send(dest, data, count, MpiDatatype<T>());
	}

	template<typename T> int send(int dest, T &data)
	{
		return send(dest, &data, 1, MpiDatatype<T>());
	}

	int send(int dest, BinarySerializer &binser)
	{
		size_t size = binser.size();
		int res = send(dest, size);
		if (res != MPI_SUCCESS)
			return res;
		return send(dest, binser.get(), size);
	}

	int recv(int source, void *data, int count, MPI_Datatype type)
	{
		MPI_Status st;
		return MPI_Recv(data, count, type, source, 0, comm_, &st);
	}

	template<typename T> int recv(int source, T *data, int count)
	{
		return recv(source, data, count, MpiDatatype<T>());
	}

	template<typename T> int recv(int source, T &data)
	{
		return recv(source, &data, 1, MpiDatatype<T>());
	}

	int recv(int source, BinarySerializer &binser)
	{
		size_t size;
		int res = recv(source, size);
		if (res != MPI_SUCCESS)
			return res;
		binser.resize(size);
		return recv(source, binser.get(), size);
	}

	int reduce(int root, void *sendbuf, void *recvbuf, int count,
			MPI_Datatype type, MPI_Op operation)
	{
		return MPI_Reduce(sendbuf, recvbuf, count, type, operation, root, comm_);
	}

	template<typename T> int reduce(int root, T *senddata, T*recvdata,
			int count, MPI_Op operation)
	{
		return reduce(root, senddata, recvdata, count, MpiDatatype<T>(),
				operation);
	}

	template<typename T> int reduce(int root, T &senddata, T &recvdata,
			MPI_Op operation)
	{
		return reduce(root, &senddata, &recvdata, 1, MpiDatatype<T>(),
				operation);
	}

};
}
