#ifndef __MEMMANAGER_HPP__
#define __MEMMANAGER_HPP__

#include <string>
/**
 * Abstract interface to memory manager
 */
class MemManager {
  public:
    
    struct Block {
      Block()
      {
        mP = NULL;
        mKey = 0;
      }
      
      /**
       * Pointer to allocated memory
       */
      void* mP;
      /**
       * Key of the allocated block
       */
      unsigned int mKey;
    };
    
    /**
     * Allocates memory and returns pointer to the allocated memory
     * @param n number of bytes to allocate
     * @return Block of the allocated memory
     */
    virtual Block alloc(unsigned int n) = 0;
    
    /**
     * Frees allocated memory
     * @param  
     */
    virtual void free(Block block) = 0; 
    
    virtual std::string statistics() {
      return "No statistic available";
    }
};

extern MemManager* MMDefault;

#pragma omp threadprivate(MMDefault)

#endif

