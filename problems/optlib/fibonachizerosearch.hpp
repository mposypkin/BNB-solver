#ifndef __FIBONACHI_ZERO_SEARCH__
#define __FIBONACHI_ZERO_SEARCH__

#include <util/common/vec.hpp>
#include "util/common/utilmacro.hpp"
#include <util/common/bnblog.hpp>
#include "linesearch.hpp"
#include <iostream>


template <typename FT> class FibonachiZeroSearch : public LineSearch <FT> {

public:

	FibonachiZeroSearch(){
		mX = NULL;
		mX0 = NULL;
		mEps = 1e-8;
		mL = 1e-6;
	}

	~FibonachiZeroSearch(){
		if(mX)
			free(mX);
		if(mX0)
			free(mX0);
	}

	void setObjective(Objective<FT>* obj){
		mObj = obj;
		dim = mObj->getDim();
		mX = (FT*)realloc(mX, sizeof(FT) *dim);
		mX0 = (FT*)realloc(mX0, sizeof(FT) *dim);
	};

	FT search(FT* x, FT* dir){
		//Step 1
		FT alpha, sgn, fx0;
			
		VecUtils::vecCopy<FT>(dim,x,mX0);
		fx0 = mObj->func(mX0);
		mLog << "FZS: Starting Fibonachi zero search. Initial function value = " << fx0 << "\n";
		mLog << "FZS: Initial coordinates:  ";
		for(int j = 0; j < dim; ++j){
			mLog << mX0[j] << " ";
		};
		mLog << "\n";
		mLog << "FZS: Direction:  ";
		for(int j = 0; j < dim; ++j){
			mLog << dir[j] << " ";
		};
		mLog << "\n";
		mLog.write();
		alpha = BNBABS(mX0[0]/dir[0]);
		sgn = BNBSGN(-mX0[0]/dir[0]);
		for(int j = 1; j < dim; ++j){
			if (BNBABS(mX0[j]/dir[j]) < alpha){
				alpha = BNBABS(mX0[j]/dir[j]);
				sgn = BNBSGN(-mX0[j]/dir[j]);
			}
		};
		mLog << "FZS: alpha = " << alpha << ", sgn = " << sgn << "\n";
		mLog << "FZS: Starting exponential descent:  \n";
		mLog.write();		
		if (fx0 < 0){
			a = 0;
			VecUtils::vecSaxpy<FT>(dim, mX0, dir, sgn*alpha/2, mX);
			fa = mObj->func(mX);
			while (fx0*fa > 0){
				mLog << "FZS: x = " << a <<", f(x) = " << fa << "\n";
				mLog.write();						
				VecUtils::vecCopy<FT>(dim,mX,mX0);
				alpha = alpha/2;
				a += sgn*alpha;
				if (BNBABS(a-alpha) < mL){
					return a;
				}
				VecUtils::vecSaxpy<FT>(dim, mX0, dir, sgn*alpha/2, mX);
				fa = mObj->func(mX);
			}
			if (sgn > 0){
				b = a + sgn*alpha/2;
			}
			else{
				b = a;
				a = b + sgn*alpha/2;
			}
		} else {
			a = 0;
			VecUtils::vecSaxpy<FT>(dim, mX0, dir, -sgn*alpha, mX);
			while (fx0*mObj->func(mX) > 0){
				VecUtils::vecCopy<FT>(dim,mX,mX0);
				a -= sgn*alpha;
				VecUtils::vecSaxpy<FT>(dim, mX0, dir, -sgn*alpha, mX);
			}
			if (sgn < 0){
				b = a - sgn*alpha;
			}
			else{
				b = a;
				a = b - sgn*alpha;
			}			
		}	
		mLog << "FZS: Starting Fibonachi search:  \n";		
		mLog.write();		
		VecUtils::vecSaxpy<FT>(dim, x, dir, a, mX);
		fa = mObj->func(mX);
		VecUtils::vecSaxpy<FT>(dim, x, dir, b, mX);
		fb = mObj->func(mX);
				
		//Step 2
		mMaxFib = (b-a)/mL;
		
		fib1 = 1.0;
		fib2 = 1.0;
		fib3 = 2.0;
		N=2;
		while (fib3< mMaxFib){
			fib1 = fib2;
			fib2 = fib3;
			fib3 = fib1+fib2;
			N++;
		};
		int k =0;
		//~ for (int k =0; k<N-3;++k){
		while(k<N-3){
			mLog << "FZS: a = " << a << ", b = " << b << ", fa = " << fa << ", fb = " << fb <<"  \n";		
			mLog.write();		
			y = a + fib1/fib3*(b-a);
			VecUtils::vecSaxpy<FT>(dim, x, dir, y, mX);
			fy = mObj->func(mX);
			if (fa*fy < 0){
				k+=2;
				fib3 = fib1;
				fib2 = fib2 - fib1;
				fib1 = fib1 - fib2;
				b = y;
				fb = fy;
			} else {
				z = a + fib2/fib3*(b-a);
				VecUtils::vecSaxpy<FT>(dim, x, dir, z, mX);
				fz = mObj->func(mX);
				if (fb*fz < 0){
					k+=2;
					fib3 = fib1;
					fib2 = fib2 - fib1;
					fib1 = fib1 - fib2;
					a = z;
					fa = fz;
				} else {
					k+=3;
					fib3 = fib2 - fib1;
					fib2 = fib1 - fib3;
					fib1 = fib3 - fib2;
					a = y;
					fa = fy;
					b = z;
					fb = fz;
				}
			}

		}

		VecUtils::vecSaxpy<FT>(dim, x, dir,(a+b)/2.0, mX);
		fz = mObj->func(mX);
		
		mLog << "FZS: Done Fibonachi zero search. Result function value = " << fz << "\n";
		mLog.write();
		return (a+b)/2.0;
		
		
	}

	void setL(FT L){
		mL = L;
	}

	void setEps(FT e){
		mEps = e;
	}

private:
	Objective<FT> *mObj;
	//~ FT* x1,x2,x3,xmin;
	FT * mX;
	FT * mX0;
	FT a,b,y,z, fa,fb,fy,fz;
	FT mEps,mL, mMaxFib;
	FT fib1,fib2,fib3;

	int dim,N;
	BNBLog <BNB_NO_LOGGING> mLog;	
};

#endif
