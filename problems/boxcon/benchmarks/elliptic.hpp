#ifndef __ELLIPTIC_HPP__
#define __ELLIPTIC_HPP__

#include <math.h>
#include <problems/optlib/objective.hpp>
#include <util/common/vec.hpp>

/**
	Shifted Rotated High Conditioned Elliptic Function
 **/
template <class FT> class Elliptic: public Objective<FT> {
public:
    
	Elliptic(int n, FT* o, FT* M, FT f_bias){
		Objective<FT>::mDim = n;
		mBias = f_bias;
		
		mO = (FT*)malloc(n*sizeof(FT));
		VecUtils::vecCopy<FT>(Objective<FT>::mDim, o, mO);
		
		mM = (FT*)malloc(n*n*sizeof(FT));
		VecUtils::vecCopy<FT>(Objective<FT>::mDim*Objective<FT>::mDim, M, mM);
		
		mAux1 = (FT*)malloc(n*sizeof(FT));
		mAux2 = (FT*)malloc(n*sizeof(FT));
		mCoef = (FT*)malloc(n*sizeof(FT));
		for(int i = 0; i < n; ++i){
			mCoef[i] = pow(1e6, (1.0*i/(n-1)));
		};		
		
		
	}

	Elliptic(int n){
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
		
		mCoef = (FT*)malloc(n*sizeof(FT));
		for(int i = 0; i < n; ++i){
			mCoef[i] = pow(1e6, (1.0*i/(n-1)));
		};		
		
	}
	
	~Elliptic(){
		if(mM)
			free(mM);
		if(mO)
			free(mO);
		if(mAux1)
			free(mAux1);
		if(mAux2)
			free(mAux2);
		if(mCoef)
			free(mCoef);
	}

	FT func(const FT* x){
		int n;
		n = Objective<FT>::mDim;
		
		VecUtils::vecSaxpy<FT>(n, x, mO, -1.0, mAux1);
		VecUtils::vecRotate<FT>(n, mM, mAux1, mAux2);
		
		FT sum = 0.0;
		for(int i = 0; i < n; ++i){
			sum+=mCoef[i]*mAux2[i]*mAux2[i];
		};
		return sum+mBias;
	}
	
	void grad(const FT* x, FT* g){
		int n;
		n = Objective<FT>::mDim;

		FT sum;
		FT coef;
		for(int j = 0; j < n; ++j){
			sum = 0.0;
			for(int i = 0; i < n; ++i){
				sum+=2*mCoef[i]*mM[i*n+j];
			}
			g[j] = (x[j]-mO[j])*sum;
		}
	}
	
private:
	FT mBias;
	FT* mM;
	FT* mO;
	FT* mAux1;
	FT* mAux2;
	FT* mCoef;

};
#endif
