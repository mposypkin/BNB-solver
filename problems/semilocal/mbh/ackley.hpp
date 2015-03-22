#ifndef __ACKLEY_HPP__
#define __ACKLEY_HPP__

#include <math.h>
#include <problems/optlib/objective.hpp>
#include <problems/optlib/powell.hpp>
//~ #include <problems/optlib/lmbfgs.hpp>
#include <problems/optlib/mbfgs.hpp>
#include <util/common/vec.hpp>
#include <util/common/gabriel.hpp>
#include <iostream>

#define Pi 3.14159265
/**
  * Ackley function
  */
template <class FT> class Ackley: public Objective<FT> {
public:
    
	Ackley(int n, FT cc){
		Objective<FT>::mDim = n;
		c = cc;
		local_min_121 = (FT*)malloc(121*n*sizeof(FT));
		local_min_calculated = false;
		gg_calculated = false;
				
	}

	Ackley(int n){
		Objective<FT>::mDim = n;
		c = 2*Pi;
		local_min_121 = (FT*)malloc(121*n*sizeof(FT));
		local_min_calculated = false;
		gg_calculated = false;
	}

	FT func(const FT* x){
		int n;
		n = Objective<FT>::mDim;
		FT sum1 = 0.0;
		FT sum2 = 0.0;
		for(int i = 0; i < n; ++i){
			sum1+=x[i]*x[i];
			sum2+=cos(c*x[i]);
		};
		return 20.0+exp(1.0)-20.0*exp(-0.2*(sqrt((1.0/n)*sum1)))-exp((1.0/n)*sum2);
	}
	
	void grad(const FT* x, FT* g){
		int n = Objective<FT>::mDim;
		FT sum1 = 0.0;
		FT sum2 = 0.0;
		for(int i = 0; i < n; ++i){
			sum1+=x[i]*x[i];
			sum2+=cos(c*x[i]);
		};
		for(int j = 0; j < Objective<FT>::mDim; ++j){
			g[j] = 1.0/n*(4*exp(-0.2*(sqrt((1.0/n)*sum1)))*x[j]/sqrt((1.0/n)*sum1)+c*exp((1.0/n)*sum2)*sin(c*x[j]));
		};
	}
		
	
	void CalculateLocalMin(){
		if (!local_min_calculated){
			Powell<double> p;
			p.setObjective(this);
			p.setMaxk(100);
			p.setLineSearchStep(0.01*c);
			
			//~ MBFGS bfgs;
			//~ bfgs.setObjective(this);
			//~ bfgs.setEps(0.001);
			
			FT x[Objective<FT>::mDim];
			FT temp_min;
			for(int i = -5; i <=5; ++i){
				for(int j = -5; j <=5; ++j){
					x[0] = i*2*Pi/c;
					x[1] = j*2*Pi/c;
					for(int k = 2; k <Objective<FT>::mDim; ++k)
						x[i] = 0.0;
					if (p.search(x,&temp_min)){
					//~ if (bfgs.search(x,&temp_min)){
						VecUtils::vecCopy<FT>(Objective<FT>::mDim, x, local_min_121+(11*(i+5)+j+5)*Objective<FT>::mDim);
					} else {
						std::cout << "Ackley: Couldn't find local minimum: "<< i << " " << j << std::endl;
					}
				}
			}
			local_min_calculated = true;
		}
	}
	
	FT* GetLocalMinima(){
		if (!local_min_calculated)
			CalculateLocalMin();
		return local_min_121;
	}
	
	void CalculateGabriel(){
		if (!gg_calculated){
			CalculateLocalMin();
			gg = new Gabriel<double>;
			gg->Calculate(Objective<FT>::mDim,121,local_min_121);
			gg_calculated = true;
		}
	}
	
	Gabriel<FT>* getGabriel(){
		CalculateGabriel();
		return  gg;
	};
	
private:
	FT c;
	FT* local_min_121;
	bool local_min_calculated;

	Gabriel<FT>* gg;
	bool gg_calculated;


};
#endif
