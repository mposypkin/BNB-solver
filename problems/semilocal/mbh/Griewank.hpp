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
		local_min_121 = (FT*)malloc(121*n*sizeof(FT));
		local_min_calculated = false;
		gg_calculated = false;
		scale = 1.0;
	}

	FT func(const FT* x){

		FT v1,v2;
		int n;
		n = Objective<FT>::mDim;
		
		//Scaling
		FT x1[n];
		for(int i = 0; i < n; i ++) {
			x1[i] = x[i] / scale;
		}
		//End of Scaling
		
		v1 = 0.0;
		v2 = 1.0;
		for(int i = 0; i < n; i ++) {
			v1 += x1[i] * x1[i];
			v2 *= cos(x1[i]/sqrt(i+1));
		};
		return 1.0+1.0/4000.0*v1-v2;
	}
        
	void grad(const FT* x, FT* g){
		int n;
		FT temp;
		n = Objective<FT>::mDim;
		
		//Scaling
		FT x1[n];
		for(int i = 0; i < n; i ++) {
			x1[i] = x[i] / scale;
		}
		//End of Scaling
	
		for(int i = 0; i < n; i ++) {
			mCos[i] = cos(x1[i]/sqrt(i+1));
		};

		for(int i = 0; i < n; i ++) {
			g[i] = 1.0/2000.0*x1[i];
			temp = 1.0/sqrt(i);
			for(int j = 0; j < i; ++j){
				temp*=mCos[j];
			};
			temp*=sin(x1[i]/sqrt(i+1));
			for(int j = i+1; j < n; ++j){
				temp*=mCos[j];
			};
			g[i]+=temp;
		};
	};

	FT gradAndFunc(const FT* x, FT* g){
		int n;
		FT temp, v1,v2;;
		n = Objective<FT>::mDim;
		
		//Scaling
		FT x1[n];
		for(int i = 0; i < n; i ++) {
			x1[i] = x[i] / scale;
		}
		//End of Scaling
		
		for(int i = 0; i < n; i ++) {
			mCos[i] = cos(x1[i]/sqrt(i+1));
		};
		v1 = 0.0;
		v2 = 1.0;

		for(int i = 0; i < n; i ++) {
			v1 += x1[i] * x1[i];
			v2 *= mCos[i];
			g[i] = 1.0/2000.0*x1[i];
			temp = 1.0/sqrt(i);
			for(int j = 0; j < i; ++j){
				temp*=mCos[j];
			};
			temp*=sin(x1[i]/sqrt(i+1));
			for(int j = i+1; j < n; ++j){
				temp*=mCos[j];
			};
			g[i]+=temp;
		};
		return 1.0+1.0/4000.0*v1-v2;
	};
    
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
					x[0] = i*2*Pi * scale;
					x[1] = j*2*Pi*sqrt(2) * scale;
					for(int k = 2; k <Objective<FT>::mDim; ++k)
						x[i] = 0.0;
					if (p.search(x,&temp_min)){
					//~ if (bfgs.search(x,&temp_min)){
						VecUtils::vecCopy<FT>(Objective<FT>::mDim, x, local_min_121+(11*(i+5)+j+5)*Objective<FT>::mDim);
					} else {
						std::cout << "Griewank: Couldn't find local minimum: "<< i << " " << j << std::endl;
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
	
	void SetScale(FT s){
		scale = s;
	}
	
	Gabriel<FT>* getGabriel(){
		CalculateGabriel();
		return  gg;
	};
		
		
	~Griewank(){
		if (mCos){
			free(mCos);
		}
	}

private:
	FT* mCos;
	FT* local_min_121;
	FT scale;
	bool local_min_calculated;

	Gabriel<FT>* gg;
	bool gg_calculated;

};
#endif
