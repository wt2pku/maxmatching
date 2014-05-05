//////////////////////////////////////////////////////////////////////
/// @file getcloserV.cpp
/// @author Tao Wang@CS325 2014 Spring
/// @Load the input data of the testcases and
/// @Get the closer vetices for each vetex
//////////////////////////////////////////////////////////////////////
using namespace std;
#include<stdio.h>
#include<vector>
#include <map>
#include <algorithm> 
#include "objects.hpp"

/*********************subroutines****************************/
void merge_sort(int low, int high, vector<vertex*> &a);
void merge(int low, int mid, int high, vector<vertex*> &a);

/**********************global variables***********************/
char input_file[100];
int Radius = 5000;
//the vector vertices are to store the vertices in the graph
vector<vertex*> vertices;
//the map edges are to sotre the edges in the graph
std::map<std::pair<int, int>, edge*> edges;

/**********************getcloserV ****************************/
bool myfunction (vertex* i, vertex* j) {
  return (i->x==j->x && i->y==j->y);
}
///////////////////////////////////////////////////////////////////////
//@Load data from testcases and decide which two vertices has an 
//@edge between them
///////////////////////////////////////////////////////////////////////
void getcloserV( ) {
	FILE* fp;
	
	FILE* fpout;
	
	fpout = fopen("vertices.log", "w");
	
	if ( fopen(input_file, "r") == NULL) {
		printf("\nError: cannot open input file %s for read!\n", input_file);
		exit(1);
	}

	/******* read the input ***************************/

	printf("\nLoading input file ... \n");
	
	
	fp = fopen(input_file, "r");
	
	int nLines;
	char line[50];

	fgets(line, 50, fp);
	
	sscanf(line, "%d", &nLines);
	
	int i, j;
	int xx, yy;
	
	for(i = 0; i < nLines; i++) {
		fgets(line, 50, fp);
		sscanf(line, "%d\t%d", &xx, &yy);
		
		vertex *tmpV = new vertex(xx, yy, i);
		vertices.push_back(tmpV);
	}
	
	
	printf("\nFinish Loading input file.\n");
	printf("\nStart Merge Sort...\n");
	//sorting the vertices according to the x coordinate
	merge_sort(0, vertices.size() - 1, vertices);
	printf("\nFinish Merge Sort.\n");
	
	
	
	int sizeOfVertices = vertices.size();
	printf("\nStart construct edges...\n");
	for(i = 0; i < sizeOfVertices - 1; i++) {
		int leftLine = vertices[i]->x;
		int yLine = vertices[i]->y;
		//int leftID = vertices[i]->id;
		for( j = i + 1; j < sizeOfVertices; j++) {
			//if in x-axis is closer than h
			if(vertices[j]->x - leftLine <= Radius) {
				//and in y-axis is closer than h
				if(abs(vertices[j]->y - yLine) <= Radius) {
					//edge *tmpE = new edge(vertices[i], vertices[j]);
					//if found a vertices, update vertices[i]'s information
					vertices[i]->degree++;
					vertices[i]->neighbors.push_back(vertices[j]);
					//vertices[i]->edgeMatch.push_back(0);
					vertices[i]->edgeVisit.push_back(0);
					vertices[i]->edgeUsed.push_back(0);
					//vertices[i]->neighborMap.insert( std::pair<int, int>(j, vertices[i]->degree - 1) );
					vertices[i]->neighborMap[vertices[j]->id] = vertices[i]->degree - 1;
					//printf("i->id = %d, j->id = %d\n", vertices[i]->id, vertices[j]->id);
					//printf("i->degree = %d\n", vertices[i]->degree);
					//printf("iInj = %d\n", vertices[i]->neighborMap[vertices[j]->id]);
					//int iInj = vertices[i]->neighborMap[vertices[j]->id];
					//printf("%d th neighbor: %d\n", iInj, vertices[i]->neighbors[iInj]->id);
					//update vertices[j]'s information
					vertices[j]->degree++;
					vertices[j]->neighbors.push_back(vertices[i]);
					//vertices[j]->edgeMatch.push_back(0);
					vertices[j]->edgeVisit.push_back(0);
					vertices[j]->edgeUsed.push_back(0);
					//vertices[j]->neighborMap.insert( std::pair<int, int>(i,vertices[j]->degree - 1) );
					vertices[j]->neighborMap[vertices[i]->id] = vertices[j]->degree - 1;

					//insert the edge to the map
					//edges.insert(std::pair<std::pair<int, int>, edge*>(std::pair<int, int>(leftID, vertices[j]->id), tmpE));
					fprintf(fpout, "edge left endpoint: %d, right endpoint: %d\n", vertices[i]->id, vertices[j]->id);
				}
			} else { // if the x-axis is further than h, the sweep line will move
				break;
			}
		}
	}
	printf("Finish construct edges.\n");
	
	fclose(fp);
	fclose(fpout);
	
}

/*****************************merge_sort************************************/				
void merge_sort(int low, int high, vector<vertex*> &a)
{
	int mid;
	if(low<high)
	{
		mid = (low+high)/2;
		merge_sort(low, mid, a);
		merge_sort(mid+1, high, a);
		merge(low, mid, high, a);
	}
}

/******************************merge****************************************/
void merge(int low, int mid, int high, vector<vertex*> &a)
{
	int sz = a.size();
	vector<vertex*> b(sz);
	int h,i,j,k;
	h=low;
	i=low;
	j=mid+1;

	while((h<=mid)&&(j<=high))
	{
		if(a[h]->x <= a[j]->x )
		{
			b[i] = a[h];
			h++;
		} else {
			b[i] = a[j];
			j++;
		}
		i++;
	}
	
	if(h > mid) {
		for(k = j; k <= high; k++) {
			b[i] = a[k];
			i++;
		}
	} else {
		for(k = h;k <= mid; k++) {
			b[i] = a[k];
			i++;
		}
	}
	
	for(k = low; k <= high; k++) {
		a[k] = b[k];
	}
}
