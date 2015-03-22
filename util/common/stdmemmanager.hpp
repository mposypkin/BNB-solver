#ifndef __STDMEMMANAGER_HPP__
#define __STDMEMMANAGER_HPP__

#include <stdlib.h>
#include "bnberrcheck.hpp"
#include "memmanager.hpp"

class StdMemManager: public MemManager {
  public:
  
    typedef MemManager::Block Block;
    
    Block alloc(unsigned int n)
    {
      Block block;
      block.mP = (void*)malloc(n);
      block.mKey = 0;
      if(block.mP == NULL)
        BNB_ERROR_REPORT("No more memory\n");
      return block;
    }
    
  
    void free(Block block)
    {
        ::free(block.mP);
    }
    
};
#endif

