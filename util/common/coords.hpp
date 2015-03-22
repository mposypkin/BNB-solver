// -*- coding: utf-8 -*-
#include  <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

template <typename T, int n_dim> class coords{
	public:
		coords() {};
		coords (vector <T> init);
		coords(coords c);
		T& operator[] (int i);
		void operator+= (coords c);
		coords operator+ (coords c);
		void operator-= (coords c);
		coords operator- (coords c);
		coords operator- ();
		void operator*= (T l);
		coords coords::operator* (T l);
		T operator* (coords c);
		
		friend ostream& operator<< (ostream& o, coords const & c);
	private:
		vector<T> x;
};


coords::coords (){
	x.resize(n_dim);
	fill (x.begin(), x.end(), 0);
}

coords::coords (vector <T> init){
	x = init;
	x.resize(n_dim);
}

coords::coords (coords c){
	x = c.x;
}


T& coords::operator[] (int i){
	return x[i];
}

void coords::operator+= (coords c){
	for(int i = 0; i < n_dim; ++i){
		x[i]+=c.x[i];
	};
}

coords coords::operator+ (coords c){
	coords<T,n_dim> res(x);
	res+=c;
	return res;
}


void coords::operator-= (coords c){
	for(int i = 0; i < n_dim; ++i){
		x[i]-=c.x[i];
	};
	
}

coords coords::operator- (coords c){
	coords<T,n_dim> res(x);
	res-=c;
	return res;
}

coords coords::operator- (){
	vector<T> res;
	res.reserve(n_dim)
	for(int i = 0; i< n_dim; ++i){
		res.push_back( - x[i]);
	};
	return coords(res);
}

void coords::operator*= (T l){
	for(int i = 0; i < n_dim; ++i){
		x[i]*=l;
	};
}
coords coords::operator* (T l){
	coords<T,n_dim> res(x);
	res*=l;
	return res;
}



T coords::operator* (coords c){
	T res = 0;
	for(int i = 0; i< n_dim; ++i){
		res += x[i] * c[i];
	};
	return res;
}


ostream& operator<< (ostream& o, coords const & c){
	for (int i = 0; i<n_dim; i++){
	o<< c.x[i] << " ";
	};
	return o;
}


