#ifndef __LINEAR_OBJECTIVE__
#define __LINEAR_OBJECTIVE__

#include <problems/optlib/objective.hpp>
#include <util/common/vec.hpp>

template <class FT> class LinearObjective: public Objective<FT> {
public:
    
	LinearObjective(int n, FT* g){
		Objective<FT>::mDim = n;
		mG = (FT*) malloc (n * sizeof(FT));
		VecUtils::vecCopy<FT>(n,g,mG);
	}

	FT func(const FT* x){
		return VecUtils::vecDotProd<FT>(Objective<FT>::mDim, x,mG);
	}
        
	void grad(const FT* x, FT* g){
		VecUtils::vecCopy<FT>(Objective<FT>::mDim,mG,g);
	}

	FT gradAndFunc(const FT* x, FT* g){
		VecUtils::vecCopy<FT>(Objective<FT>::mDim,mG,g);
		return VecUtils::vecDotProd<FT>(Objective<FT>::mDim, x,mG);
	}
	
	~LinearObjective(){
		if (mG){
			free(mG);
		}
	}
	
private:
	FT* mG;
};
#endif
