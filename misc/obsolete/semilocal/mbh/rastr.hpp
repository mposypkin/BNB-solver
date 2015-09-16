#ifndef __RASTR_HPP__
#define __RASTR_HPP__

#include <math.h>
#include <problems/optlib/objective.hpp>
#include <problems/optlib/powell.hpp>
#include <util/common/gabriel.hpp>


/**
  * Rastringin function
  */
template <class FT> class Rastr: public Objective<FT> {
  public:
    
	Rastr(int n, FT a){
		Objective<FT>::mDim = n;
		mA = a;
		local_min_121 = (FT*)malloc(121*n*sizeof(FT));
		local_min_calculated = false;
		gg_calculated = false;	    
	}

	FT func(const FT* x){
		FT v;
		int n;
		n = Objective<FT>::mDim;
		v = n * mA;
		for(int i = 0; i < n; i ++) {
			v += x[i] * x[i] - mA * cos(2 * M_PI * x[i]);
		}
		return v;
	}
        
	void grad(const FT* x, FT* g){
		int n;
		n = Objective<FT>::mDim;
		for(int i = 0; i < n; i ++) {
			g[i] = 2 * x[i] + 2 * M_PI * mA * sin(2 * M_PI * x[i]);
		}
	}

	FT gradAndFunc(const FT* x, FT* g){
		FT v;
		int n;
		n = Objective<FT>::mDim;
		v = n * mA;
		for(int i = 0; i < n; i ++) {
			v += x[i] * x[i] - mA * cos(2 * M_PI * x[i]);
			g[i] = 2 * x[i] + 2 * M_PI * mA * sin(2 * M_PI * x[i]);
		}
		return v;
	}
    
    	void CalculateLocalMin(){
		if (!local_min_calculated){
			Powell<double> p;
			p.setObjective(this);
			p.setMaxk(100);
			p.setLineSearchStep(0.01);
			
			//~ MBFGS bfgs;
			//~ bfgs.setObjective(this);
			//~ bfgs.setEps(0.001);
			
			FT x[Objective<FT>::mDim];
			FT temp_min;
			for(int i = -5; i <=5; ++i){
				for(int j = -5; j <=5; ++j){
					x[0] = 1.0*i;
					x[1] = 1.0*j;
					for(int k = 2; k <Objective<FT>::mDim; ++k)
						x[i] = 0.0;
					if (p.search(x,&temp_min)){
					//~ if (bfgs.search(x,&temp_min)){
						VecUtils::vecCopy<FT>(Objective<FT>::mDim, x, local_min_121+(11*(i+5)+j+5)*Objective<FT>::mDim);
					} else {
						std::cout << "Rastr: Couldn't find local minimum: "<< i << " " << j << std::endl;
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

    FT mA;
	FT* local_min_121;
	bool local_min_calculated;

	Gabriel<FT>* gg;
	bool gg_calculated;
};
#endif
