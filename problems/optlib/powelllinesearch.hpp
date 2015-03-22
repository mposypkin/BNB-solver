#ifndef __POWELL_LINE_SEARCH__
#define __POWELL_LINE_SEARCH__

#include <util/common/vec.hpp>
#include "util/common/utilmacro.hpp"
#include "linesearch.hpp"
#include <iostream>


#define EPS 1e-8
#define M_EPS -1e-8
#define maxK 100

#define PLS_DEBUG_no
#define PLS_FULL_DEBUG_no

template <typename FT> class PowellLineSearch : public LineSearch <FT> {

public:

	PowellLineSearch(){
		//~ x1 = NULL;
		//~ x2 = NULL;
		//~ x3 = NULL;
		//~ xmin = NULL;
		mX = NULL;
		mEps1 = 1e-10;
		mEps2 = 1e-8;
		mDx = 1.0;
		
	}

	~PowellLineSearch(){
		if(mX)
			free(mX);
	}

	void setObjective(Objective<FT>* obj){
		mObj = obj;
		dim = mObj->getDim();
		//~ x1 = (FT*)realloc(mX, sizeof(FT) * mObj->getDim());
		//~ x2 = (FT*)realloc(mX, sizeof(FT) * mObj->getDim());
		//~ x3 = (FT*)realloc(mX, sizeof(FT) * mObj->getDim());
		//~ xmin = (FT*)realloc(mX, sizeof(FT) * mObj->getDim());
		mX = (FT*)realloc(mX, sizeof(FT) *dim);
	};

	FT search(FT* x, FT* dir){
		#ifdef PLS_DEBUG
			std::cout << "Powell line search initiated: " << "x: ";
			for(int j = 0; j < dim; ++j){
				std:: cout << x[j] << " ";
			};
			std::cout << "; dir: ";
			for(int j = 0; j < dim; ++j){
				std:: cout << dir[j] << " ";
			};
			
			std::cout << std::endl;
		#endif

		FT denom;
		//~ VecUtils::vecCopy<FT>(dim, x,x1);
		//Step 1
		x1 = 0.0;
		k = 0;
		for(;;){
			
			//Step2
			x2 = x1 + mDx;
			//Step3
			VecUtils::vecSaxpy<FT>(dim, x, dir, x1, mX);
			f1 = mObj->func(mX);
			#ifdef PLS_DEBUG
				std::cout << "Mark 1" << std::endl;
			#endif
			VecUtils::vecSaxpy<FT>(dim, x, dir, x2, mX);
			f2 = mObj->func(mX);
			#ifdef PLS_DEBUG
				std::cout << "Mark 2" << std::endl;
			#endif
			//Step4
			if (f1>f2){
				x3 = x2 + mDx;
			} else {
				x3 = x1 - mDx;
			};
			//Step5
			VecUtils::vecSaxpy<FT>(dim, x, dir, x3, mX);
			f3 = mObj->func(mX);
			#ifdef PLS_DEBUG
				std::cout << "Mark 3: "<< f3 << std::endl;
			
			#endif

			
			for (;;){
				k++;
				if ( k > maxK ) return xmin;
				//Step6
				if (f1<f2) {
					if (f1<f3) {
						xmin = x1;
						fmin = f1;
						imin = 1;
					} else {
						xmin = x3;
						fmin = f3;
						imin = 3;
					} 
				} else {
					if (f2<f3) {
						xmin = x2;
						fmin = f2;
						imin = 2;
					} else {
						xmin = x3;
						fmin = f3;
						imin = 3;
					} 
				}
				#ifdef PLS_DEBUG
					std::cout << "Mark 4" << std::endl;
				#endif

				//Step7
				denom = 2.0 * ((x2 - x3)*f1 + (x3 - x1)*f2 + (x1 - x2)*f3);
				#ifdef PLS_DEBUG
					std::cout << "Mark 5" << std::endl;
				#endif

				if (denom > M_EPS) {
				//~ if (denom >= 0.0) {
					#ifdef PLS_DEBUG
						std::cout << "Mark 6" << std::endl;
					#endif
					
					if (imin!=3) {
						return xmin;
					} else {
						x1 = x3;
						break; //Goto step2
					}
				#ifdef PLS_DEBUG
					std::cout << "Mark 7" << std::endl;
				#endif
				} else {
					xx = ((x2*x2 - x3*x3)*f1 + (x3*x3 - x1*x1)*f2 + (x1*x1 - x2*x2)*f3) / denom;
					VecUtils::vecSaxpy<FT>(dim, x, dir, xx, mX);
					#ifdef PLS_DEBUG
						std::cout << "Mark 8" << std::endl;
						for(int j = 0; j < dim; ++j){
							std::cout << mX[j] << " ";
						};
						std::cout << std::endl;
					#endif

					ff = mObj->func(mX);
					
					#ifdef PLS_DEBUG
						std::cout << "-------------------------------------------------------------------------------------" << std::endl;;
						std::cout << "pls: " << "x1: "<< x1 << "; f1: " << f1 << "; X1: ";
						#ifdef PLS_FULL_DEBUG
						for(int j = 0; j < dim; ++j){
							std:: cout << mX[j] << " ";
						};
						#endif
						std::cout << std::endl;

					
						std::cout << "pls: " << "x2: "<< x2 << "; f2: " << f2 << "; X2: ";
						#ifdef PLS_FULL_DEBUG
						for(int j = 0; j < dim; ++j){
							std:: cout << mX[j] << " ";
						};
						#endif
						std::cout << std::endl;
					
					
						std::cout << "pls: " << "x3: "<< x3 << "; f3: " << f3 << "; X3: ";
						#ifdef PLS_FULL_DEBUG
						for(int j = 0; j < dim; ++j){
							std:: cout << mX[j] << " ";
						};
						#endif
						std::cout << std::endl;

						std::cout << "pls: " << "xx: "<< xx << "; ff: " << ff << "; XX: ";
						#ifdef PLS_FULL_DEBUG
						for(int j = 0; j < dim; ++j){
							std:: cout << mX[j] << " ";
						};
						#endif
						std::cout << std::endl;
						std::cout << "pls: " << "xmin: "<< xmin << "; fmin: " << fmin << std::endl;					
					#endif


					//Step8
					
					
					if ((((BNBABS(ff) > EPS) &&(BNBABS((fmin - ff)/ff) < mEps1)) || (BNBABS(fmin - ff) < mEps1)) && (((BNBABS(xx)>EPS) && (BNBABS((xmin - xx)/xx) < mEps2)) || (BNBABS(xmin - xx) < mEps2)) ){
						//Step 8a
						if (ff < fmin) {
							return xx;
						} else {
							return xmin;
						};
						
					} else {
						//Step 8b
						if (x3>x1) {
							if ((x1<xx) and (xx < x2)) {
								if (ff < fmin) {
									x3 = x2;
									f3 = f2;
									x2 = xx;
									f2 = ff;
								} else {
									x1 = xx;
									f1 = ff;
								}
							} else 
							if ((x2<xx) and (xx < x3)){
								if ((ff>fmin) and (imin == 2)) {
									x3 = xx;
									f3 = ff;
								} else {
									x1 = x2;
									f1 = f2;
									x2 = xx;
									f2 = ff;
								}
							} else {
								//Step  8c
								if (ff < fmin) {
									x1 = xx;
									break; //goto 2
								} else {
									x1 = xmin;
									break; //goto 2
								}
							}
						} else {
							if ((x3 < xx) and(xx < x1)) {
								if ((ff>fmin) and (imin == 1)) {
									x3 = x2;
									f3 = f2;
									x2 = x1;
									f2 = f1;
									x1 = xx;
									f1 = ff;
								} else {
									x2 = x1;
									f2 = f1;
									x1 = xx;
									f1 = ff;
								}
							} else 
							if ((x1<xx) and (xx < x2)){
								if (ff < fmin){
									x3 = x2;
									f3 = f2;
									x2 = xx;
									f2 = ff;
								} else {
									x2 = xx;
									f2 = ff;
								}
							} else {
								//Step  8c
								if (ff < fmin) {
									x1 = xx;
									break; //goto 2
								} else {
									x1 = xmin;
									break; //goto 2
								}
							}
							
						}
						
					}
				}
			}
		}
	}

	void setDx(FT a){
		mDx = a;
	}

	void setEpss(FT e1, FT e2){
		mEps1 = e1;
		mEps2 = e2;
	}

private:
	Objective<FT> *mObj;
	//~ FT* x1,x2,x3,xmin;
	FT * mX;
	FT x1,x2,x3,xmin,xx,f1,f2,f3,fmin,ff;
	FT mEps1,mEps2, mDx;
	int imin, k;

	int dim;
};

#endif
