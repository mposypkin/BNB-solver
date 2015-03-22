#include "conchelper.hpp"

int ConcHelper::mTid = 0;

int ConcHelper::mTids[BNB_MAX_THREADS];

int ConcHelper::mNumberOfThreads = 1;

pthread_mutex_t ConcHelper::mMutConc;

pthread_key_t ConcHelper::mTidKey;


void* plauncher(void* x)
{
  ConcHelper* ch = (ConcHelper*)x;
  ch->assignTid();
  ch->cmethod();
}


