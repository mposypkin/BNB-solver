#ifndef __SMARTPTR_HPP__
#define __SMARTPTR_HPP__

#include "mmregistry.hpp"
#include "memmanager.hpp"
#include "bnberrcheck.hpp"

template < class T > struct SmartArrayPtr
{
  public:

    /**
     * The constructor
     */
    SmartArrayPtr (unsigned int num = 0)
    {
      mMemManager = NULL;
      setDim(0);
      alloc(num);
    }

    /**
     * Destructor
     *
     */
    ~SmartArrayPtr ()
    {
      dismiss ();
    }

    /**
     * Copy constructor
     *
     * @param sp pointer to copy
     */
    SmartArrayPtr (const SmartArrayPtr & sp)
    {
      mMemManager = NULL;
      mDim = 0;
      assign(sp);
    }

    /**
     * Assignment operator
     * @param sp
     * @return
     */
    SmartArrayPtr & operator = (const SmartArrayPtr & sp)
    {
      dismiss ();
      assign(sp);
      return *this;
    }


    /**
     * Copies the content of the array pointer
     * @param sp array to clone
     */
    void copy(const SmartArrayPtr & sp) {
        dismiss();
        alloc(sp.mDim);
        for(int i = 0; i < mDim; i ++)
           ((T*)mXBlock.mP)[i] = sp[i];
    }


    /**
     * Allocate memory for this pointer
     *
     * @param num number of items
     *
     */
    void alloc (unsigned int num)
    {
      if(mMemManager == NULL) {
	mMemManager = MMRegistry::getMemManager();
      }
      //BNB_ASSERT(mMemManager == MMRegistry::getMemManager());
      if(mDim != 0) {
        BNB_ERROR_REPORT("Trying to allocate non-null pointer");
      }	
      if(num > 0) {
	setDim(num);
	mCounterBlock = mMemManager->alloc(sizeof(int));
	*((int*)mCounterBlock.mP) = 1;
	mXBlock = mMemManager->alloc(sizeof(T) * num);
      }
    }

    /**
     * Allocate memory if not already done
     * @param num number of items
     */
    void lazyAlloc(int num)
    {
      if(mDim == 0)
	alloc(num);
    }
    
    /**
      * Reallocate memory if the storage is not sufficient
      * @param num number of items 
      */
    void realloc(int num)
    {
      if(mDim < num) {
	if(mDim > 0)
	  dismiss();
	alloc(num);
      }
    }

    /**
     * Element access
     *
     * @param i element index
     *
     * @return
     *
    T& operator [ ] (int i) const
    {
      if((i < 0) || (i >= mDim)) {
	BNB_ERROR_REPORT("Access element violates pointer bounds!\n");
      }
      return ((T*)mXBlock.mP)[i];
    }
     */

    /**
      * Pointer value access
      */
    operator T* () const
    {
      return (T*)mXBlock.mP;
    }

    /**
      *Check nullity
      */
    bool isNil() const
    {
      return (mDim == 0);
    }

    /**
      * Get size
      */
    unsigned int size() const
    {
      return mDim;
    }
  private:

    void assign(const SmartArrayPtr& sp)
    {
      if(sp.mDim > 0) {
	if(mMemManager != sp.mMemManager) {
	  alloc(sp.mDim);
	  for(int i = 0; i < sp.mDim; i ++) 
	    ((T*)mXBlock.mP)[i] = sp[i];
	} else {
	  mCounterBlock = sp.mCounterBlock;
	  mXBlock = sp.mXBlock;
	  (*(int*)(mCounterBlock.mP))++;
	}
      }
      setDim(sp.mDim);
    }

    void dismiss ()
    {
//      BNB_ASSERT(mMemManager == MMRegistry::getMemManager());
      if(mDim > 0) {
	(*(int*)(mCounterBlock.mP))--;
	if ((*(int*)(mCounterBlock.mP)) == 0){
	  mMemManager->free(mXBlock);
	  mMemManager->free(mCounterBlock);
	}
	setDim(0);
      }
    }

    void setDim(unsigned int dim)
    {
      mDim = dim;
    }

    MemManager::Block mCounterBlock;
    MemManager::Block mXBlock;
    unsigned int mDim;
    MemManager* mMemManager;
};

#endif
