#ifndef __RASTR_HPP__
#define __RASTR_HPP__

#include <math.h>
#include <problems/optlib/objective.hpp>

/**
  * Rastringin function
  */
template <class FT> class Rastr: public Objective<FT> {
  public:
    
	Rastr(int n, FT a){
		Objective<FT>::mDim = n;
		mA = a;
		mBias = 0;
		
		mO = (FT*)malloc(n*sizeof(FT));
		VecUtils::vecSet<FT>(Objective<FT>::mDim, 0, mO);
		
		mM = (FT*)malloc(n*n*sizeof(FT));
		VecUtils::vecSet<FT>(Objective<FT>::mDim*Objective<FT>::mDim, 0, mM);
		
		for(int i = 0; i < n; ++i){
			mM[i*n+i] = 1;
		}

		mAux1 = (FT*)malloc(n*sizeof(FT));
		mAux2 = (FT*)malloc(n*sizeof(FT));
	}
	
	Rastr(int n, FT a, FT* o, FT* M, FT f_bias){
		Objective<FT>::mDim = n;
		mA = a;
		mBias = f_bias;
		
		mO = (FT*)malloc(n*sizeof(FT));
		VecUtils::vecCopy<FT>(Objective<FT>::mDim, o, mO);
		
		mM = (FT*)malloc(n*n*sizeof(FT));
		VecUtils::vecCopy<FT>(Objective<FT>::mDim*Objective<FT>::mDim, M, mM);
		
		mAux1 = (FT*)malloc(n*sizeof(FT));
		mAux2 = (FT*)malloc(n*sizeof(FT));
	}


	FT func(const FT* x){
		FT v;
		int n;
		n = Objective<FT>::mDim;
		
		VecUtils::vecSaxpy<FT>(n, x, mO, -1.0, mAux1);
		VecUtils::vecRotate<FT>(n, mM, mAux1, mAux2);

		v = n * mA;
		for(int i = 0; i < n; i ++) {
			v += mAux2[i] * mAux2[i] - mA * cos(2 * M_PI * mAux2[i]);
		}
		return v;
	}
        
	//~ void grad(const FT* x, FT* g){
		//~ int n;
		//~ n = Objective<FT>::mDim;
		//~ for(int i = 0; i < n; i ++) {
			//~ g[i] = 2 * x[i] + 2 * M_PI * mA * sin(2 * M_PI * x[i]);
		//~ }
	//~ }

	//~ FT gradAndFunc(const FT* x, FT* g){
		//~ FT v;
		//~ int n;
		//~ n = Objective<FT>::mDim;
		//~ v = n * mA;
		//~ for(int i = 0; i < n; i ++) {
			//~ v += x[i] * x[i] - mA * cos(2 * M_PI * x[i]);
			//~ g[i] = 2 * x[i] + 2 * M_PI * mA * sin(2 * M_PI * x[i]);
		//~ }
		//~ return v;
	//~ }
	~Rastr(){
		if(mM)
			free(mM);
		if(mO)
			free(mO);
		if(mAux1)
			free(mAux1);
		if(mAux2)
			free(mAux2);
		
	}
    

private:
	FT mA;
	FT mBias;
	FT* mCos;
	FT* mM;
	FT* mO;
	FT* mAux1;
	FT* mAux2;
};
#endif
