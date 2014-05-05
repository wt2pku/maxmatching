#ifndef _OBJECTS_H
#define _OBJECTS_H
#include <cstdlib>
#include <vector>
#include <deque>
#include <map>

#define INFTY 2147483647

class blossom;
class vertex {
public:
	vertex(const int xx, const int yy, const int numV) {
			x = xx;
			y = yy;
			id = numV;
			degree = 0;
			evenlevel = INFTY;
			oddlevel = INFTY;
			visited = 0;
			matched = 0;
			blos = NULL;
			markLeft = 0;
			fv = NULL;
			erase = 0;
			matchNeighbor = NULL;
			noUnused = 0;
		}
	~vertex() { neighbors.clear(); }
	int level() {
		if(oddlevel > evenlevel) {
			return evenlevel;
		} else {
			return oddlevel;
		}
	}
	
	std::vector<vertex*> neighbors; 
	//std::vector<int> edgeMatch;
	std::vector<int> edgeVisit;
	std::vector<int> edgeUsed;
	std::map<int, int> neighborMap;
	
	//f(v)
	vertex *fv;
	
	//matchNeighbor
	vertex *matchNeighbor;
	
	//x coordinate
	int x;
	//y coordinate
	int y;
	//id of the vertex
	int id;
	//degree of the vertex
	int degree;
	//even level
	int evenlevel;
	//odd level
	int oddlevel;
	//visited
	int visited; //0 unvisited; 1 visited
	//matched
	int matched; //0 unmatched; 1 matched
	//markLeft
	int markLeft;
	//markRight
	int markRight;
	//erase
	int erase; //0 not erase; 1 erase
	//noUnused
	int noUnused;
	
	//blossom
	blossom *blos;
	//predecessors
	std::vector<vertex*> predecessors;
	//anomalies
	std::vector<vertex*> anomalies;
	
};

class blossom {
public:
	//TODO: find base and peaks of the blossom
	blossom(vertex* bs, vertex* pl, vertex * pr) { 
		base = bs;
		peakL = pl;
		peakR = pr;
	}
	
	~blossom();
	vertex* base;
	vertex* peakL;
	vertex* peakR;
	//std::vector<vertex*> VVs;
};


class path {
public:
	path() {}
	~path() {}
	std::deque<vertex*> pvertices;
};

class edge {
public:
	edge(vertex* lv, vertex* rv)
		: left(lv), right(rv), visited(0), used(0) {}
	~edge() {}
	//left endpoint of the edge
	vertex* left;
	//right endpoint of the edge
	vertex* right;
	//matched
	int matched; //0 unmatched, 1 matched
	//visited
	int visited; //0 unvisited; 1 visited
	//used
	int used; //0 unused; 1 used
};

#endif
		
		
		
		
