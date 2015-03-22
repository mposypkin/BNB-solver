#pragma once

#include <mpi.h>

namespace dmc
{
class Environment
{
public:
	Environment(int &argc, char **& argv)
	{
		MPI_Init(&argc, &argv);
	}

	~Environment()
	{
		MPI_Finalize();
	}

};
}
