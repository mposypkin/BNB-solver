/**
 * MPI implementation for AbstractCommunicator
 *
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file mpiaclib.cpp
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aclib.hpp"

#define BNBMAX(a,b) (((a)>(b))?(a):(b))

#define BNBMIN(a,b) (((a)<(b))?(a):(b))

#define PROBE_MAGIC_NUMBER 8

#define INIT_BUFFER_SIZE 4 * 1024 * 1024

#define INIT_CHUNK_SIZE 1024 * 1024

AbstractCommunicator::AbstractCommunicator (int *pargc, char **pargv[])
  : mSendBuffer(INIT_BUFFER_SIZE, INIT_CHUNK_SIZE), 
    mRecvBuffer(INIT_BUFFER_SIZE, INIT_CHUNK_SIZE)
{
  MPI_Init (pargc, pargv);
  MPI_Comm_rank (MPI_COMM_WORLD, &mMyId);
  MPI_Comm_size (MPI_COMM_WORLD, &mProcessNumber);
  mAuxData = (void*)(new MPI_Comm);
  *(MPI_Comm*) mAuxData = MPI_COMM_WORLD;
  mSendBufferPosition = 0;
  mRecvBufferPosition = 0;
  mTime = MPI_Wtime ();
}

AbstractCommunicator::AbstractCommunicator (AbstractCommunicator* old, bool flag)
{
  int color;
  MPI_Comm com;
  
  if(flag)
    color = 1;
  else
    color = MPI_UNDEFINED;
  
  mAuxData = (void*)(new MPI_Comm);
  MPI_Comm_split(*(MPI_Comm*) old->mAuxData, color, old->getMyId(), (MPI_Comm*)mAuxData);
  if(flag) {
    MPI_Comm_rank (*(MPI_Comm*)mAuxData, &mMyId);
    MPI_Comm_size (*(MPI_Comm*)mAuxData, &mProcessNumber);
    mSendBufferPosition = 0;
    mRecvBufferPosition = 0;
    mTime = MPI_Wtime ();
  }
}


AbstractCommunicator::~AbstractCommunicator ()
{
  int res;

  if(*(MPI_Comm*)mAuxData != MPI_COMM_NULL) {
    MPI_Comm_compare(*(MPI_Comm*)mAuxData, MPI_COMM_WORLD, &res);

    if(res == MPI_IDENT) 
      MPI_Finalize ();
    else 
      MPI_Comm_free((MPI_Comm*)mAuxData);  
    *((MPI_Comm*)mAuxData) = MPI_COMM_NULL;  
  }
}


bool AbstractCommunicator::isIn()
{
  return (*(MPI_Comm*)mAuxData != MPI_COMM_NULL);
}


void
AbstractCommunicator::transfer (const int procid, const int mode)
{
  if (procid != mMyId)
    {
      if (mode == async)
	MPI_Send (mSendBuffer.getBuf(), mSendBufferPosition, MPI_PACKED, procid, 0,
		  *(MPI_Comm*)mAuxData);
      else
	MPI_Ssend (mSendBuffer.getBuf(), mSendBufferPosition, MPI_PACKED, procid, 0,
		   *(MPI_Comm*)mAuxData);

    }
}

int
AbstractCommunicator::probe (int *size, bool block)
{
  MPI_Status st;
  int flag;

  if (block)
    {
      MPI_Probe (MPI_ANY_SOURCE, MPI_ANY_TAG, *(MPI_Comm*)mAuxData, &st);
      flag = 1;
    }
  else {
    for(int i = 0; i < PROBE_MAGIC_NUMBER; i ++) {
      MPI_Iprobe (MPI_ANY_SOURCE, MPI_ANY_TAG, *(MPI_Comm*)mAuxData, &flag, &st);
      if(flag)
	break;
    }
  }
  if (flag)
    {
      MPI_Get_count (&st, MPI_PACKED, size);
      return st.MPI_SOURCE;
    }
  else
    return -1;
}

int
AbstractCommunicator::recv (const int procid, const int msize)
{
  MPI_Status st;
  unsigned char *old;

  if (procid == mMyId)
    {
      mRecvBuffer.resize(mSendBuffer.getSize());
      mRecvBufferPosition = 0;
      memcpy (mRecvBuffer.getBuf(), mSendBuffer.getBuf(), mRecvBuffer.getSize());
      return procid;
    }
  else
    {
      int size;
      int rank;

      if (procid == -1)
	rank = MPI_ANY_SOURCE;
      else
	rank = procid;

      if (msize < 0)
	{
	  throw
	    AbstractCommunicatorException
	    ("Negative value as a message size");
	}
      else if (msize == 0)
	{
	  if (procid == -1)
	    {
	      MPI_Probe (MPI_ANY_SOURCE, MPI_ANY_TAG, *(MPI_Comm*)mAuxData, &st);
	      rank = st.MPI_SOURCE;
	    }
	  else
	    MPI_Probe (procid, MPI_ANY_TAG, *(MPI_Comm*)mAuxData, &st);

	  MPI_Get_count (&st, MPI_PACKED, &size);
	}
      else
	{
	  size = msize;
	}

      mRecvBuffer.resize(size);
      mRecvBufferPosition = 0;
      MPI_Recv (mRecvBuffer.getBuf(), mRecvBuffer.getSize(), MPI_PACKED, rank,
		MPI_ANY_TAG, *(MPI_Comm*)mAuxData, &st);
      return st.MPI_SOURCE;
    }
}

void
AbstractCommunicator::barrier ()
{
  MPI_Barrier (*(MPI_Comm*)mAuxData);
}

void
AbstractCommunicator::abort (int rc)
{
  MPI_Abort (*(MPI_Comm*)mAuxData, rc);
}

double
AbstractCommunicator::time ()
{
  return MPI_Wtime ()/* - mTime*/;
}
