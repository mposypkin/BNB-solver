#ifndef __FIBONACHI_LINE_SEARCH__
#define __FIBONACHI_LINE_SEARCH__

#include <util/common/vec.hpp>
#include "util/common/utilmacro.hpp"
#include "linesearch.hpp"
#include <iostream>


#define EPS 1e-8
template <typename FT> class FibonachiLineSearch : public LineSearch <FT> {

public:

	FibonachiLineSearch(){
		mX = NULL;
		mEps = 1e-8;
		mL = 1e-3;
	}

	~FibonachiLineSearch(){
		if(mX)
			free(mX);
	}

	void setObjective(Objective<FT>* obj){
		mObj = obj;
		dim = mObj->getDim();
		mX = (FT*)realloc(mX, sizeof(FT) *dim);
	};

	FT search(FT* x, FT* dir){
		//Step 1
		FT delta = 1.0;
		FT x0 = 0.0;
		FT fx0;
		a = -1.0*delta;
		b = delta;
		VecUtils::vecSaxpy<FT>(dim, x, dir, x0, mX);
		fx0 = mObj->func(mX);
		VecUtils::vecSaxpy<FT>(dim, x, dir, a, mX);
		fa = mObj->func(mX);
		VecUtils::vecSaxpy<FT>(dim, x, dir, b, mX);
		fb = mObj->func(mX);
		if (!((fa > fx0) and (fx0 < fb))){
			if (fa > fb){
				a = x0;
				fa = fx0;
				x0 = b;
				fx0 = fb;
				while (fx0 < fa) {
					a = x0;
					fa = fx0;
					delta*=2;
					x0 = a+delta;
					VecUtils::vecSaxpy<FT>(dim, x, dir, x0, mX);
					fx0 = mObj->func(mX);
				}
				b = x0;
				fb = fx0; 
			} else {
				b = x0;
				fb = fx0;
				x0 = a;
				fx0 = fa;
				while (fx0 < fb) {
					b = x0;
					fb = fx0;
					delta*=2;
					x0 = b - delta;
					VecUtils::vecSaxpy<FT>(dim, x, dir, x0, mX);
					fx0 = mObj->func(mX);
				}
				a = x0;
				fa = fx0; 
		}
		
		
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
		//Step 3
		//~ k = 0;
		//Step 4
		y = a + fib1/fib3*(b-a);
		z = a + fib2/fib3*(b-a);
		//Step 5

		VecUtils::vecSaxpy<FT>(dim, x, dir, y, mX);
		fy = mObj->func(mX);
		VecUtils::vecSaxpy<FT>(dim, x, dir, z, mX);
		fz = mObj->func(mX);
		
		for (int k =0; k<N-3;++k){
			fib3 = fib2;
			fib2 = fib1;
			fib1 = fib3-fib2;
			//Step6
			if (fy <= fz){
				//Step 6a
				b = z;
				fb = fz;
				z = y;
				fz = fy;
				y = a + fib1/fib3*(b-a);
				VecUtils::vecSaxpy<FT>(dim, x, dir, y, mX);
				fy = mObj->func(mX);
			} else {
				a = y;
				fa = fy;
				y = z;
				fy = fz;
				z = a + fib2/fib3*(b-a);
				VecUtils::vecSaxpy<FT>(dim, x, dir, z, mX);
				fz = mObj->func(mX);
			}
		}
		y = (a+b)/2.0;
		z = y+mEps;

		VecUtils::vecSaxpy<FT>(dim, x, dir, y, mX);
		fy = mObj->func(mX);
		VecUtils::vecSaxpy<FT>(dim, x, dir, z, mX);
		fz = mObj->func(mX);

		
		if (fy<=fz){
			return (a+z)/2.0;
		}else {
			return (y+b)/2.0;
		}

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
	FT a,b,y,z, fa,fb,fy,fz;
	FT mEps,mL, mMaxFib;
	FT fib1,fib2,fib3;

	int dim,N;
};

#endif
