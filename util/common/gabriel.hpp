#ifndef _GABRIEL_
#define _GABRIEL_

#include "vec.hpp"

#include <iostream>
#include <fstream>
template <typename FT> class Gabriel{
public:
	Gabriel(){
		points = NULL;  
		edges1 = NULL;
		edges2 = NULL;
		centers = NULL;
		radii = NULL;
		temp_center = NULL;
		same_points_check = false;
		same_points_epsilon = 0.0;
	};
	
	Gabriel(int d){
		points = NULL;  
		edges1 = NULL;
		edges2 = NULL;
		centers = NULL;
		radii = NULL;
		temp_center = NULL;
		dim = d;
		same_points_check = false;
		same_points_epsilon = 0.0;
	};
	
	void Reserve(int n){
		n_points = 0;
		n_edges = 0;
		points = (FT*) realloc(points, dim * n * sizeof(FT));
		edges1 = (int*) realloc(edges1,  3*n * sizeof(int));
		edges2 = (int*) realloc(edges2,  3*n * sizeof(int));
		centers = (FT*) realloc(centers, dim * 3*n * sizeof(FT));
		radii = (FT*) realloc(radii,  3*n * sizeof(FT));
		temp_center = (FT*) realloc(temp_center, dim * sizeof(FT));
	};
	
	void SetSamePointsCheck(FT eps){
		same_points_check = true;
		same_points_epsilon = eps;
	}
	void AddEdge (int i,int j, FT* temp_center, FT temp_radius){
		edges1[n_edges] = i;
		edges2[n_edges] = j;
		VecUtils::vecCopy(dim, temp_center, centers+dim*n_edges);
		radii[n_edges] = temp_radius;
		n_edges++;
	};
	void RemoveEdge(int j){
		edges1[j] = edges1[n_edges-1];
		edges2[j] = edges2[n_edges-1];
		VecUtils::vecCopy(dim, centers+dim*(n_edges-1), centers+dim*j);
		radii[j] = radii[n_edges-1];
		n_edges--;
		//~ std::cout<< "Edge" << j<<"removed"<< std::endl;
	};
	
	
	void AddPoint(FT* point){
		check = true;
		if (same_points_check){
			for(int j = 0; j < n_points; ++j){
				if (VecUtils::vecDist<FT> (dim, point, points+j*dim) < same_points_check){
					check = false;
				}
			};
		}
		if (check){
			for(int j = 0; j < n_edges; ++j){
				if (VecUtils::vecDist<FT>(dim,centers+j*dim, point) < radii[j])
					RemoveEdge(j);
			};
			
			for(int j = 0; j < n_points; ++j){
				VecUtils::vecCenter<FT>(dim, point, points+j*dim, temp_center);
				temp_radius = VecUtils::vecDist<FT>(dim, point, points+j*dim) / 2.0;
				check = true;
				for(int k = 0; k < n_points; ++k){
					if ((k != j) &&  (VecUtils::vecDist<FT> (dim, temp_center, points+k*dim) < temp_radius)){
						check = false;
					};
				};
				if (check){
					AddEdge(j,n_points, temp_center, temp_radius);
				}
			};
			VecUtils::vecCopy(dim, point, points+dim*n_points);
			n_points++;
		}
		//~ std::cout<< n_points <<":"<< n_edges << std::endl;
	};
	
	
	
	void Calculate(int d, int n, FT* p){
		dim = d;
		n_points = 0;
		n_edges = 0;
		points = (FT*) realloc(points, dim * n * sizeof(FT));
		edges1 = (int*) realloc(edges1,  3*n * sizeof(int));
		edges2 = (int*) realloc(edges2,  3*n * sizeof(int));
		centers = (FT*) realloc(centers, dim * 3*n * sizeof(FT));
		radii = (FT*) realloc(radii,  3*n * sizeof(FT));
		temp_center = (FT*) realloc(temp_center, dim * sizeof(FT));
		for(int i = 0; i < n; ++i ){
			AddPoint(p+i*dim);
			
		}
	};
	
	
	
	//~ void WriteVect(char* filename){
		//~ if ((dim==2) or (dim == 3)) {
			//~ std::ofstream of(filename);
			
			//~ of << "VECT" << std::endl;
			//~ of << n_edges << " " << 2*n_edges << " " << 1 << std::endl;
			//~ for(int j = 0; j < n_edges-1; ++j){
				//~ of << "2 ";
			//~ };
			//~ of << "2" << std::endl;
			//~ of << "1 ";
			//~ for(int j = 0; j < n_edges-2; ++j){
				//~ of << "0 ";
			//~ };
			//~ of << "0" << std::endl;
			//~ for(int j = 0; j < n_edges; ++j){
				//~ if (dim == 3) {
					//~ of << points[edges1[j]*dim] << " " << points[edges1[j]*dim+1] << " " << points[edges1[j]*dim+2] << " " ;
					//~ of << points[edges2[j]*dim] << " " << points[edges2[j]*dim+1] << " " << points[edges2[j]*dim+2] << " " ;
				//~ } else {
					//~ of << points[edges1[j]*dim] << " " << points[edges1[j]*dim+1] << " " << 0 << std::endl;
					//~ of << points[edges2[j]*dim] << " " << points[edges2[j]*dim+1] << " " << 0 << std::endl;
					
				//~ }
				
			//~ };
			//~ of << "0 0 0 1" << std::endl;
			//~ of.close();
		//~ };
	//~ };
	
	//~ void WriteDot(char* filename){
		//~ if ((dim==2) or (dim == 3)) {
			//~ std::ofstream of(filename);
			//~ of << "Graph G{" << std::endl;
			//~ of << "node [shape = point];" << std::endl;
			//~ if (dim == 3)
				//~ of << "graph [dim = 3];" << std::endl;
			//~ for(int j = 0; j < n_points; ++j){
				//~ of << j << " [pos = \"";
				//~ if (dim == 2) {
					//~ of << points[j*dim] << "," << points[j*dim+1] << "!\"];"<<std::endl;
				//~ };
				//~ if (dim == 3) {
					//~ of << points[j*dim] << "," << points[j*dim+1] << "," << points[j*dim] << "!\"];" << std::endl;
				//~ };
			//~ };
			//~ for(int j = 0; j < n_edges; ++j){
				//~ of << edges1[j] << " -- " << edges2[j] << ";" << std::endl;
			//~ };
			
			//~ of << "}";
			//~ of.close();
		//~ }
	//~ };
	
	//~ void WriteDot(char* filename, FT scale){
		//~ if ((dim==2) or (dim == 3)) {
			//~ std::ofstream of(filename);
			//~ of << "Graph G{" << std::endl;
			//~ of << "node [shape = point];" << std::endl;
			//~ if (dim == 3)
				//~ of << "graph [dim = 3];" << std::endl;
			//~ for(int j = 0; j < n_points; ++j){
				//~ of << j << " [pos = \"";
				//~ if (dim == 2) {
					//~ of << points[j*dim]*scale << "," << points[j*dim+1]*scale << "!\"];"<<std::endl;
				//~ };
				//~ if (dim == 3) {
					//~ of << points[j*dim]*scale << "," << points[j*dim+1]*scale << "," << points[j*dim]*scale << "!\"];" << std::endl;
				//~ };
			//~ };
			//~ for(int j = 0; j < n_edges; ++j){
				//~ of << edges1[j] << " -- " << edges2[j] << ";" << std::endl;
			//~ };
			
			//~ of << "}";
			//~ of.close();
		//~ }
	//~ };
	
	//~ void WriteDot(char* filename, FT scale, FT bound){
		//~ if ((dim==2) or (dim == 3)) {
			//~ std::ofstream of(filename);
			//~ of << "Graph G{" << std::endl;
			//~ of << "node [shape = point];" << std::endl;
			//~ if (dim == 3)
				//~ of << "graph [dim = 3];" << std::endl;
			
			//~ of << "ll [pos = \"-" << bound << ",-" << bound << "!\", color=red];"<<std::endl;
			//~ of << "lu [pos = \"-" << bound << "," << bound << "!\", color=red];"<<std::endl;
			//~ of << "rl [pos = \"" << bound << ",-" << bound << "!\", color=red];"<<std::endl;
			//~ of << "ru [pos = \"" << bound << "," << bound << "!\", color=red];"<<std::endl;
			//~ for(int j = 0; j < n_points; ++j){
				//~ of << j << " [pos = \"";
				//~ if (dim == 2) {
					//~ of << points[j*dim]*scale << "," << points[j*dim+1]*scale << "!\"];"<<std::endl;
				//~ };
				//~ if (dim == 3) {
					//~ of << points[j*dim]*scale << "," << points[j*dim+1]*scale << "," << points[j*dim]*scale << "!\"];" << std::endl;
				//~ };
			//~ };
			//~ for(int j = 0; j < n_edges; ++j){
				//~ of << edges1[j] << " -- " << edges2[j] << ";" << std::endl;
			//~ };
			
			//~ of << "}";
			//~ of.close();
		//~ }
	//~ };

	
	~Gabriel(){
		if (points)
			free(points);
		if (edges1)
			free(edges1);
		if (edges2)
			free(edges2);
		if (centers)
			free(centers);
		if (radii)
			free(radii);
		if (temp_center)
			free(temp_center);
	};
	

	int* edges1;
	int* edges2;
	FT* centers;
	FT* radii;
	int n_edges,n_points,dim;
private:
	bool check, same_points_check;
	FT same_points_epsilon;
	FT* points; 
	FT* temp_center;
	FT temp_radius;
	

};

#endif
