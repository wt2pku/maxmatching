#ifndef _OBJECTS_H
#define _OBJECTS_H
#include <cstdlib>
#include <vector>

class vertex {
public:
	vertex(const int xx, const int yy, const int numV)
		: x(xx), y(yy), id(numV) {}
	~vertex() {}
	std::vector<vertex*> neighbors; 
	
	//x coordinate
	int x;
	//y coordinate
	int y;
	//id of the vertex
	int id;
};

class edge {
public:
	edge(const int lv, const int rv)
		: left(lv), right(rv) {}
	~edge() {}
	//left endpoint of the edge
	int left;
	//right endpoint of the edge
	int right;
};

#endif
		
		
		
		