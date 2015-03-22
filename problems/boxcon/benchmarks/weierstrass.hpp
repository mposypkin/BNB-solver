#ifndef __WEIERSTRASS_HPP__
#define __WEIERSTRASS_HPP__

#include <math.h>
#include <problems/optlib/objective.hpp>
#include <util/common/vec.hpp>

/**
	Shifted Rotated Weierstrass Function
 **/
 
 #define PI 3.1415926
template <class FT> class Weierstrass: public Objective<FT> {
public:
    
	Weierstrass(int n, FT* o, FT* M, FT f_bias, FT a, FT b, int kmax){
		Objective<FT>::mDim = n;
		mBias = f_bias;
		
		mO = (FT*)malloc(n*sizeof(FT));
		VecUtils::vecCopy<FT>(Objective<FT>::mDim, o, mO);
		
		mM = (FT*)malloc(n*n*sizeof(FT));
		VecUtils::vecCopy<FT>(Objective<FT>::mDim*Objective<FT>::mDim, M, mM);
		
		mAux1 = (FT*)malloc(n*sizeof(FT));
		mAux2 = (FT*)malloc(n*sizeof(FT));
		mKmax = kmax;
		mA = (FT*)malloc(kmax*sizeof(FT));
		mB = (FT*)malloc(kmax*sizeof(FT));
		for(int i = 0; i < kmax; ++i){
			mA[i] = pow(a,i);
			mB[i] = 2*PI*pow(b,i);
		};		
		
		
	}

	Weierstrass(int n){
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
		
		mKmax = 20;
		mA = (FT*)malloc(mKmax*sizeof(FT));
		mB = (FT*)malloc(mKmax*sizeof(FT));
		for(int i = 0; i < mKmax; ++i){
			mA[i] = pow(0.5,i);
			mB[i] = 2*PI*pow(3.0,i);
		};		
	}
	
	~Weierstrass(){
		if(mM)
			free(mM);
		if(mO)
			free(mO);
		if(mAux1)
			free(mAux1);
		if(mAux2)
			free(mAux2);
		if(mA)
			free(mA);
		if(mB)
			free(mB);
	}

	FT func(const FT* x){
		int n;
		n = Objective<FT>::mDim;
		
		VecUtils::vecSaxpy<FT>(n, x, mO, -1.0, mAux1);
		VecUtils::vecRotate<FT>(n, mM, mAux1, mAux2);
		
		FT sum1 = 0.0;
		for(int i = 0; i < n; ++i){
			for(int k = 0; k < mKmax; ++k){
				sum1+=floor(mA[k]*cos(mB[k]*(mAux2[i]+0.5)));
			};
		}

		FT sum2 = 0.0;
		for(int k = 0; k < mKmax; ++k){
			sum2+=floor(mA[k]*cos(mB[k]*0.5));
		};
		return sum1-n*sum2+mBias;
	}
	
	
private:
	int mKmax;
	FT mBias;
	FT* mM;
	FT* mO;
	FT* mAux1;
	FT* mAux2;
	FT* mA;
	FT* mB;
	

};
#endif
