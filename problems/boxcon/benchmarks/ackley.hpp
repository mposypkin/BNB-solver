#ifndef __ACKLEY_HPP__
#define __ACKLEY_HPP__

#include <math.h>
#include <problems/optlib/objective.hpp>
#include <util/common/vec.hpp>
#include <iostream>

#define Pi 3.14159265
/**
  * Ackley function
  */
template <class FT> class Ackley: public Objective<FT> {
public:
    

	Ackley(int n){
		Objective<FT>::mDim = n;
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

	Ackley(int n, FT* o, FT* M, FT f_bias){
		Objective<FT>::mDim = n;
		mBias = f_bias;
		
		mO = (FT*)malloc(n*sizeof(FT));
		VecUtils::vecCopy<FT>(Objective<FT>::mDim, o, mO);
		
		mM = (FT*)malloc(n*n*sizeof(FT));
		VecUtils::vecCopy<FT>(Objective<FT>::mDim*Objective<FT>::mDim, M, mM);
		
		mAux1 = (FT*)malloc(n*sizeof(FT));
		mAux2 = (FT*)malloc(n*sizeof(FT));
	}


	FT func(const FT* x){
		int n;
		n = Objective<FT>::mDim;

		VecUtils::vecSaxpy<FT>(n, x, mO, -1.0, mAux1);
		VecUtils::vecRotate<FT>(n, mM, mAux1, mAux2);

		FT sum1 = 0.0;
		FT sum2 = 0.0;
		for(int i = 0; i < n; ++i){
			sum1+=mAux2[i]*mAux2[i];
			sum2+=cos(c*mAux2[i]);
		};
		return 20.0+exp(1.0)-20.0*exp(-0.2*(sqrt((1.0/n)*sum1)))-exp((1.0/n)*sum2);
	}
	
	//~ void grad(const FT* x, FT* g){
		//~ int n = Objective<FT>::mDim;
		//~ FT sum1 = 0.0;
		//~ FT sum2 = 0.0;
		//~ for(int i = 0; i < n; ++i){
			//~ sum1+=x[i]*x[i];
			//~ sum2+=cos(c*x[i]);
		//~ };
		//~ for(int j = 0; j < Objective<FT>::mDim; ++j){
			//~ g[j] = 1.0/n*(4*exp(-0.2*(sqrt((1.0/n)*sum1)))*x[j]/sqrt((1.0/n)*sum1)+c*exp((1.0/n)*sum2)*sin(c*x[j]));
		//~ };
	//~ }
		
	~Ackley(){
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
	FT c, mBias;
	FT* mCos;
	FT* mM;
	FT* mO;
	FT* mAux1;
	FT* mAux2;

};
#endif
