#ifndef __MPIACLIB__HPP__
#define __MPIACLIB__HPP__

#include <mpi.h>

template < class T > MPI_Datatype getMPIType ();

template <> MPI_Datatype getMPIType < int >()
{
  return MPI_INT;
}

template <> MPI_Datatype getMPIType < double >()
{
  return MPI_DOUBLE;
}

template <> MPI_Datatype getMPIType < char >()
{
  return MPI_CHAR;
}

template <> MPI_Datatype getMPIType < char const >()
{
  return MPI_CHAR;
}

template <> MPI_Datatype getMPIType < long >()
{
  return MPI_LONG;
}

template <> MPI_Datatype getMPIType < unsigned long >()
{
  return MPI_UNSIGNED_LONG;
}

template <> MPI_Datatype getMPIType < long long >()
{
  return MPI_LONG_LONG_INT;
}


template < class T > void
AbstractCommunicator::pack (const T * p)
{
  packArray (p, 1);
}

template < class T > void
AbstractCommunicator::unpack (T * p)
{
  unpackArray (p, 1);
}

template < class T > void
AbstractCommunicator::packArray (const T * p, int n)
{
  int size;
  int newsize;
  unsigned char *old;
  MPI_Datatype type;

  if(n > 0) {
    type = getMPIType < T > ();
    MPI_Pack_size (n, type, *(MPI_Comm*)mAuxData, &size);
    newsize = mSendBufferPosition + size;
    mSendBuffer.resize(newsize);
    MPI_Pack ((void *) p, n, type, mSendBuffer.getBuf(), mSendBuffer.getSize(),
	      &(mSendBufferPosition), *(MPI_Comm*)mAuxData);
  }

}


template < class T > void
AbstractCommunicator::unpackArray (T * p, int n)
{
  if(n > 0)
    MPI_Unpack (mRecvBuffer.getBuf(), mRecvBuffer.getSize(), &mRecvBufferPosition,
		(void *) p, n, getMPIType < T > (), *(MPI_Comm*)mAuxData);
}


#endif
