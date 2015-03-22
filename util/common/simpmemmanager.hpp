#ifndef __SIMPMEMMANAGER_HPP__
#define __SIMPMEMMANAGER_HPP__

/**
 * Simple memory manager
 */
#include <stdlib.h>
#include <map>
#include <vector>
#include <sstream>
#include "bnberrcheck.hpp"
#include "memmanager.hpp"



class SimpMemManager: public MemManager {
  public:
    typedef MemManager::Block Block;
    struct Chunk {
      void* mP;
      unsigned int mOffset;
      unsigned int mSize;
      int mRef;
    };
    
    class ChunkCollection {
      public:
        struct SizeRecord {
          int mSize;
          int mRef;
        };
        
        ChunkCollection(int n, int sr) {
          mMaxChunks = n;
          mNChunks = 0;
          mLastFreeChunk = -1;
          mBuffer = (Chunk*)malloc(n * sizeof(Chunk));
          if(mBuffer == NULL)
            BNB_ERROR_REPORT("Too many chunks requested");
          mMaxSizeRecords = sr;
          mCurrSizeRecords = 0;
          if(mMaxSizeRecords > 0) {
            mSizeRecords = (SizeRecord*)malloc(mMaxSizeRecords * sizeof(SizeRecord));
            if(mSizeRecords == NULL)
              BNB_ERROR_REPORT("Too many size records requested");
          }
        }
        
        int put(Chunk chunk) {
          int n;
          if(mLastFreeChunk != -1) {
            n = mLastFreeChunk;
            mLastFreeChunk = mBuffer[n].mRef;
          } else {
            if(mNChunks >= mMaxChunks)
              BNB_ERROR_REPORT("Number of chunks exceeded limit\n");
            n = mNChunks ++;
          }
          mBuffer[n] = chunk;                  
          return n;
        }
        
        Chunk remove(int key, bool flag) {
          if(key >= mNChunks) {
            BNB_ERROR_REPORT("Key exceeds the number of chunks\n");
          } else {
            if((mCurrSizeRecords == mMaxSizeRecords) || flag) {
              mBuffer[key].mRef = mLastFreeChunk;
              mLastFreeChunk = key; 
            } else {
              int sz = mBuffer[key].mSize;
              bool found = false;
              for(int i = 0; i < mCurrSizeRecords; i ++) {
                if(mSizeRecords[i].mSize == sz) {
                  mBuffer[key].mRef = mSizeRecords[i].mRef;
                  mSizeRecords[i].mRef = key;
                  found = true;
                  break;
                }                
              }
              if(!found) {              
                mSizeRecords[mCurrSizeRecords].mSize = sz;
                mSizeRecords[mCurrSizeRecords].mRef = key;                                
                mBuffer[key].mRef = -1;
                mCurrSizeRecords ++;
              }              
            }
          }
          return mBuffer[key];
        }
        
        int hireFreeChunk(int sz) {
          int n = -1, chunk = -1;
          for(int i = 0; i < mCurrSizeRecords; i ++) {
            if(mSizeRecords[i].mSize == sz) {
              n = i;
              break;
            }             
          }
          if(n == -1) {
            chunk = -1;
          } else {
            chunk = mSizeRecords[n].mRef;
            if(chunk != -1)
              mSizeRecords[n].mRef = mBuffer[chunk].mRef;
          }
//           printf("hire: %d bytes found %d, %d, %d\n", sz, chunk, mCurrSizeRecords, mNChunks);
          return chunk;
        }
        
        Chunk get(int i) {
          return mBuffer[i];
        }
      private:
        Chunk* mBuffer;
        int mNChunks;
        int mMaxChunks;
        int mLastFreeChunk;
        int mMaxSizeRecords;
        int mCurrSizeRecords;
        SizeRecord* mSizeRecords;
    };
    
    SimpMemManager(int size, int nchunks, int nsizerecs) :
    mAllocated(nchunks, nsizerecs){
      mOffset = 0;
      mSize = size;
      mAlign = 8;
      mBuffer = (unsigned char*)malloc(size); 
      if(mBuffer == NULL)
        BNB_ERROR_REPORT("Too large buffer requested");
    }
    
    
    Block alloc(unsigned int n)
    {
      Block block;
      unsigned int m = (n / mAlign + ((n % mAlign) ? 1 : 0)) * mAlign;
      if(mOffset + m > mSize){
        BNB_ERROR_REPORT("Too large size requested in allocation\n");        
      } else {
        int cn;
        cn = mAllocated.hireFreeChunk(m);
        if(cn != -1) {
          Chunk chunk = mAllocated.get(cn);
          block.mP = chunk.mP;
          block.mKey = cn;
        } else {
          Chunk chunk;
          chunk.mP = mBuffer + mOffset;
          chunk.mSize = m;
          chunk.mOffset = mOffset;
          int k = mAllocated.put(chunk);        
          mOffset += m;
          mMaxOffset = mOffset += m;
          block.mP = chunk.mP;
          block.mKey = k;
        }
      }
      return block;
    }
    
  
    void free(Block block)
    {
      bool flag;
      Chunk chunk = mAllocated.get(block.mKey);
      if(chunk.mOffset + chunk.mSize == mOffset) {
          mOffset = chunk.mOffset;
          flag = true;
      } else {
          flag = false;
      }
      mAllocated.remove(block.mKey, flag);      
    }
    
    
    std::string statistics() {
      std::ostringstream os;
      os << "Maximal offset is " << mMaxOffset << "\n";
      return os.str();
    }
  private:
    
    
    unsigned char* mBuffer;    
    unsigned int mOffset;    
    unsigned int mSize;    
    unsigned int mAlign;
    ChunkCollection mAllocated;    
    unsigned int mMaxOffset;
    
};
#endif
