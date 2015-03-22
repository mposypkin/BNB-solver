#pragma once

#include <mpi.h>

namespace dmc
{

template<typename T> MPI_Datatype MpiDatatype();

template<> MPI_Datatype MpiDatatype<char>()
{
	return MPI_CHAR;
}

template<> MPI_Datatype MpiDatatype<short>()
{
	return MPI_SHORT;
}

template<> MPI_Datatype MpiDatatype<int>()
{
	return MPI_INT;
};

template<> MPI_Datatype MpiDatatype<long>()
{
	return MPI_LONG;
};

template<> MPI_Datatype MpiDatatype<unsigned char>()
{
	return MPI_UNSIGNED_CHAR;
};

template<> MPI_Datatype MpiDatatype<unsigned short>()
{
	return MPI_UNSIGNED_SHORT;
};

template<> MPI_Datatype MpiDatatype<unsigned>()
{
	return MPI_UNSIGNED;
};

template<> MPI_Datatype MpiDatatype<unsigned long>()
{
	return MPI_UNSIGNED_LONG;
};

template<> MPI_Datatype MpiDatatype<float>()
{
	return MPI_FLOAT;
};

template<> MPI_Datatype MpiDatatype<double>()
{
	return MPI_DOUBLE;
};

template<> MPI_Datatype MpiDatatype<long double>()
{
	return MPI_LONG_DOUBLE;
};

}
