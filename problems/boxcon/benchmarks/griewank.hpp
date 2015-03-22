#ifndef __GRIEWANK_HPP__
#define __GRIEWANK_HPP__

#include <math.h>
#include <problems/optlib/objective.hpp>

/**
  * Griewankingin function
  */
template <class FT> class Griewank: public Objective<FT> {
  public:
    
	Griewank(int n){
		Objective<FT>::mDim = n;
		mCos = (FT*)malloc(n * sizeof(FT));
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

	Griewank(int n, FT* o, FT* M, FT f_bias){
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

		FT v1,v2;		
		v1 = 0.0;
		v2 = 1.0;
		for(int i = 0; i < n; i ++) {
			v1 += mAux2[i] * mAux2[i];
			v2 *= cos(mAux2[i]/sqrt(i+1));
		};
		return 1.0+1.0/4000.0*v1-v2 + mBias;
	}
        
	void grad(const FT* x, FT* g){
		int n;
		FT temp,temp2;
		n = Objective<FT>::mDim;

		VecUtils::vecSaxpy<FT>(n, x, mO, -1.0, mAux1);
		VecUtils::vecRotate<FT>(n, mM, mAux1, mAux2);
	
		for(int i = 0; i < n; i ++) {
			mCos[i] = cos(mAux2[i]/sqrt(i+1));
		};
		
		for(int j = 0; j < n; ++j){
			g[j] = 0;
			for(int i = 0; i < n; i ++) {
				temp2 = 1.0/2000.0*mAux2[i];
				temp = 1.0/sqrt(i);
				for(int k = 0; k < i; ++k){
					temp*=mCos[k];
				};
				temp*=sin(mAux2[i]/sqrt(i+1));
				for(int k = i+1; k < n; ++k){
					temp*=mCos[k];
				};
				temp2+=temp;
				g[j]+=mM[i*n+j];
			};
		};
	};

	FT gradAndFunc(const FT* x, FT* g){
		int n;
		FT temp,temp2,v1,v2;
		n = Objective<FT>::mDim;

		VecUtils::vecSaxpy<FT>(n, x, mO, -1.0, mAux1);
		VecUtils::vecRotate<FT>(n, mM, mAux1, mAux2);
	
		for(int i = 0; i < n; i ++) {
			mCos[i] = cos(mAux2[i]/sqrt(i+1));
		};

		v1 = 0.0;
		v2 = 1.0;
		
		for(int j = 0; j < n; ++j){
			
			v1 += mAux2[j] * mAux2[j];
			v2 *= cos(mAux2[j]/sqrt(j+1));

			g[j] = 0;			
			for(int i = 0; i < n; i ++) {
				temp2 = 1.0/2000.0*mAux2[i];
				temp = 1.0/sqrt(i);
				for(int k = 0; k < i; ++k){
					temp*=mCos[k];
				};
				temp*=sin(mAux2[i]/sqrt(i+1));
				for(int k = i+1; k < n; ++k){
					temp*=mCos[k];
				};
				temp2+=temp;
				g[j]+=mM[i*n+j];
			};
		};
		return 1.0+1.0/4000.0*v1-v2 + mBias;
	};
    
		
		
	~Griewank(){
		if (mCos){
			free(mCos);
		}
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
	FT mBias;
	FT* mCos;
	FT* mM;
	FT* mO;
	FT* mAux1;
	FT* mAux2;
};
#endif
