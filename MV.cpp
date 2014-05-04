//////////////////////////////////////////////////////////////////////
/// @file MV.cpp
/// @author Tao Wang@CS325 2014 Spring
/// @Implement the MV algorithm
//////////////////////////////////////////////////////////////////////
using namespace std;
#include<stdio.h>
#include<vector>
#include<map>
#include<algorithm> 
#include "objects.hpp"

/*********************subroutines****************************/
void blossAug();
void findpath();
void open();
edge* findEdge(int left, int right);

/**********************extern variables**********************/
//the vector vertices are to store the vertices in the graph
vector<vertex*> vertices;
//the map edges are to sotre the edges in the graph
std::map<std::pair<int, int>, edge*> edges;

/***********************global variables**********************/


//////////////////////////////////////////////////////////////
/**************main routine search***************************/
void search() {
	/**************first define a matching****************/
	//std::map<std::pair<int, int>, edge*> matchings;
	int i, j;
	for(i = 0; i < vertices.size(); i++) {
		if(vertices[i]->matched == 0) {
			if(!vertices[i]->neighbors.empty()) {
				for(j = 0; j < vertices[i]->neighbors.size(); j++) {
					vertex* u = vertices[i]->neighbors[j];
					if(u->matched == 0) {
						vertices[i]->matched = 1;
						vertices[i]->matchNeighbor = u;
						
						u->matched = 1;
						u->matchNeighbor = v;
						break;
					}
				}
			}
		}
	}
	
	int searchLevel = -1;
	
	/************Step 1: initialize the free vertex, evenlevel(v):= 0******/
	vector<vertex*> currentSearchSet;
	vector<vertex*> nextSearchSet;
	vector<vector<edge*>> bridges;
	
	for(i = 0; i < vertices.size(); i++) {
		if(vertices[i]->matched == 0) {
			vertices[i]->evenlevel = 0;
			currentSearchSet.push_back(vertices[i]);
		}
		vector<edge*> tmpBridge;
		bridges.push_back(tmpBridge);
	}
	
	int index;
	int neighborIndex;
	int largestLevel = vertices.size();
	while(searchLevel <= largestLevel) {
		searchLevel = searchLevel+1;
		
		//if searchLevel is even
		if(searchLevel%2 == 0) {
			//for each v with evenlevel(v) = searchLevel
			for(index = 0; index < currentSearchSet.size(); index++) {
				vertex* v = currentSearchSet[index];
				for(neighborIndex = 0; neighborIndex < v->neighbors.size(); neighborIndex++) {
					//for each v with evenlevel(v) = searchLevel, find its unmatched, unused neighbors
					if(v->matchNeighbor != v->neighbors[neighborIndex] && v->edgeUsed[neighborIndex] == 0) {
						//such neighbor is u
						vertex* u = v->neighbors[neighborIndex];
						//if evenlevel(u) is finit
						if(u->evenlevel < INFTY) {
							//then temp := (evenlevel(u) + evenlevel(v))/2
							int temp = (u->evenlevel + v->evenlevel) / 2;
							//TODO: bridges(temp) := bridges(temp) union (u,v)
							bridges[temp].push_back(edge(u,v));
						} else { 
							//(a) handle oddlevel
							if( u->oddlevel < INFTY) {
								u->oddlevel = searchLevel + 1;
								//u will be in next search level
								nextSearchSet.push_back(u);
							}
							//(b) handle predecessors
							if( u->oddlevel == searchLevel + 1) {
								u->predecessors.push_back(v);
							}
							
							//(c) handle anomalies
							if( u->oddlevel < searchLevel) {
								u->anomalies.push_back(v);
							}
						}
					}
				}
			}
		} else { //if search level is odd
			//for each v with oddlevel(v) = searchLevel and blossom(v) is undefined
			for(index = 0; index < currentSearchSet.size(); index++) {
				vertex *v = currentSearchSet[index];
				if(v->oddlevel == searchLevel && v->blos== NULL) {
					//take v's matched neighbor u
					u = v->matchNeighbor;
					if(u != NULL) {
						//(a) handle bridge
						if(u->oddlevel == searchLevel) {
							int temp = (u->oddlevel + v->oddlevel) / 2;
							//bridges(temp) := bridges(temp) union (u,v)
							bridges[temp].push_back(edge(u,v));
						}
						//(b) handle predecessors
						if(u->oddlevel == INFTY) {
							u->evenlevel = searchLevel + 1;
							//u will be in next search level
							nextSearchSet.push_back(u);
							u->predecessors.push_back(v);
						}
					}
						
				}
			}
		}
		
		int edgeIndex;
		for(edgeIndex = 0; edgeIndex < bridges[searchLevel].size(); edgeIndex++) {
			edge theEdge = bridges[searchLevel][edgeIndex];
			
			bloss-aug(theEdge->v1, theEdge->v2, searchLevel);
		}
	}
	
	
}

/*******************subroutine bloss-aug*****************************************************/
void bloss-aug(vertex *w1, vertex *w2, int searchLevel ) {
	//if w1 and w2 belong to the same blossom then go to step(5), that is return
	if(w1->blos != NULL && w2->blos != NULL && w1->blos == w2->blos) {
		return;
	}
	
	vertex* vl;
	vertex* vr;
	
	//(0) initialization: assign vl, vr
	if(w1->blos != NULL) {
		vl = w1->blos->base;
	} else {
		vl = w1;
	}
	
	if(w2->blos != NULL) {
		vr = w2->blos->base;
	} else {
		vr = w2;
	}
	
	//Mark vl left and vr right
	vl->markLeft = 1;
	vr->markRight = 1;
	
	vertex* barrier = vr;
	vertex* DCV = NULL;
	vl->fv = NULL;
	//TODO: create vertices that has been marked left and right in this call
	std::vector<vertex*> Bvertices;
	Bvertices.push_back(vl);
	Bvertices.push_back(vr);
	
	bool stop = false;
	bool newBlos = false;
	while(!stop) {
		//(1.1) if vl and vr are free vertices then
		if(v1->matched == 0 && vr->matched == 0) {
		//P:= (FINDPATH(w1, vl, undefined))-1 con FINDPATH(w2, vr, undefined)
			path* p1 = findpath(w1, vl, NULL);
			inv(p1);
			path* p2 = findpath(w2, vr, NULL);
			contraction(p1, p2);
			//augment the matching along P, do a topological erase, and go to step(5) return
			augmentanderase(p1);
			
			return;
		} else {
			//(1.2) if vl and vr are not both free vertices
			//(2.1) if level(vl) <= level(vr)
			if(vl->level() <= vr->level()) {
				
				bool noUsed = true;
				vertex* u = NULL;
				int uIndex;
				if(vl->predessors.size() != 0) {
}					for(int i = 0; i < vl->predecessors.size(); i++) {
						int mapi = vl->neighborMap[v->predecessors[i]->id];
						if(vl->edgeUsed[mapi] == 1) {
							noUsed = false;
							break;
						} else {
							//u is an unused edge
							u = vl->neighbos[mapi];
							uIndex = mapi;
						}
					}
				}
				//(2.1) If vl has no more "unused" ancestor edges then
				if(noUsed) {
					//if fvl is undefined
					if(vl->fv == NULL) {
						//then go to step (4) (create a new blossom)
						stop = true;
						newBlos = true;
						continue;
					} else {
						// vl := f(vl) and go to step (1.1)
						vl->fv;
						continue;
					}
				} else { //(2.2) vl has "unused" ancestor edges
					//mark vl-u used
					vl->edgeUsed[uIndex] = 1;
					//if u in B, u:= base(B)
					if(u->blos != NULL) {
						u = u->blos->base;
					}
					//(a) if u is unmarked, then mark u "left", f(u) := vl, vl :=u and go to (1.1)
					if(u->markLeft== 0 && u->markRight == 0) {
						u->markLeft = 1;
						Bvertices.push_back(u);
						u->fv = vl;
						vl = u;
						continue;
					} else {//(b) Otherwise(u is marked)
						//If u = barries or u!= vr, go to step(1.1)
						if( u == barrier || u!= vr) {
							continue;
						} else {//mark u "left", vr := f(vr), vl := u, DCV := u, and go to step(1.1)
							u->markLeft = 1;
							Bvertices.push_back(u);
							vr = vr->fv;
							vl = u;
							DCV = u;
							continue;
						}
					}
				}
			} else { //(3.1) level(vl) > level(vr) 
				bool noUsed = true;
				vertex* u = NULL;
				int uIndex;
				if(vr->predessors.size() != 0) {
					for(int i = 0; i < vr->predecessors.size(); i++) {
						int mapi = vr->neighborMap[v->predecessors[i]->id];
						if(vr->edgeUsed[mapi] == 1) {
							noUsed = false;
							break;
						} else {
							//u is an unused edge
							u = vr->neighbos[mapi];
							uIndex = mapi;
						}
					}
				}
				//(3.1) If vr has no more "unused" ancestor edges then
				if(noUsed) {
					// if vr = barrier
					if(vr = =barrier) {
						// vr := DCV, barrier := DCV, mark vr "right"
						vr = DCV;
						barrier = DCV;
						vr->markRight = 1;
						Bvertices.push_back(vr);
						//vl:= f(vl) and go to step (1.1)
						vl = vl->fv;
						continue;
				} else { //(3.2) vr has "unused" ancestor edges
					//mark vr-u used
					vr->edgeUsed[uIndex] = 1;
					//if u in B, u:= base(B)
					if(u->blos != NULL) {
						u = u->blos->base;
					}
					//(a) if u is unmarked, then mark u "right", f(u) := vr, vr :=u and go to (1.1)
					if(u->markLeft == 0 && u->markRight == 0) {
						u->markRight = 1;
						Bvertices.push_back(u);
						u->fv = vr;
						vr = u;
						continue;
					} else {//(b) Otherwise(u is marked)
						// if(u = vl) then DCV =:= u
						if(u == vl) {
							DCV = u;
						}
						continue;
					}
				}
			}
		}
	}
	
	//(4) (creation of a newblossom)
	if(newblos) {
		blossom *b = new blossom(DCV, w1, w2);
		//remove the right mark from DCV
		DCV->markRight = 0;
		int uIndex;
		for(uIndex = 0; uIndex < Bvertices.size(); uIndex++) {
			if(Bvertices[uIndex] != DCV) {
				u = Bvertices[uIndex];
				u->blos = b;
				//if u is outer then
				if(u->level()%2 == 0) {
					u->oddlevel = 2*searchLevel + 1 - u->evenlevel;
				} else {
					u->evenlevel = 2*searchLevel + 1 - u->oddlevel;
					//for each v in anomalies(u)
					int vIndex;
					for(vIndex = 0; vIndex < u->anomalies.size(); vIndex++) {
						vertex* v = u->anomalies[vIndex];
						int temp := (u->evenlevel + v->evenlevel)/2;
						edge* tmpE = new edge(u, v);
						//TODO: make bridges global variable
						bridges[temp].push_back(tmpE);
						//mark(u, v) "used"
						int uInv = v->neighborMap[u->id];
						v->edgeUsed[uInv] = 1;
						int vInu = u->neighborMap[v->id];
						u->edgeUsed[vInu] = 1;
					}
				}
			}
		}
	}
	return;
}
	
					
					
					

/******************subroutine findpath***************************************************/
path* findpath(vertex* high, vertex* low, blossom* B) {
	//0.0 (boundary condition) if high = low, then Path := high and got to step (8)
	if(high == low) {
		path* p = new path;
		p->pvertices.push_back(high);
		return p;
	}
	
	//0.1 (initialization v:= high
	vertex* v = high;
	
	path* p = NULL;
	vertex* u;
	while(u != low) {
		bool noUnvisited = true;
		//1. If v has no more "unvisited" predecessor edges
		while(v! = NULL && noUnvisited) {
			//1. If v has no more "unvisited" predecessor edges
			for(int i = 0; i < v->predecessors.size(); i++) {
				int mapi = v->neighborMap[v->predecessors[i]->id];
				if(v->edgeVisited[mapi] == 1) {
					noUnvisited = false;
					break;
				}
			}
			if(noUnvisited) {//1. If v has no more "unvisited" predecessor edges
				// v:= f(v) and go to step(1)
				v = v->fv;
			}
		}
		
		if(v == NULL) {
			return NULL;
		}
		
		
		//2. If blossom(v) = B then choose an "unvisited" predecessor edge v-u, mark e "visited"
		if(v->blos == B) {
			for(int i = 0; i < v->predecessors.size(); i++) {
				int mapi = v->neighborMap[v->predecessors[i]->id];
				if(v->edgeVisited[mapi] == 0) {
					u = v->predecessors[i];
					v->edgeVisited[mapi] = 1;
					int mapj = u->neighborMap[v->id];
					u->edgeVisited[mapj] = 1;
					break;
				}
			}
		} else {
			u = B->base;
		}
		//3. if u = low then go to step (6)
		if(u == low) {
			break;
		}
		//4. If (u is unvisited) or (level(u)) <= level(low)) or (blossom(u) = B and u does not have the same "left"/"rifht" mark as high)
		//then go to step 1
		if(u->visited == 0 || u->level() < low->level() || (u->blos == B && (u->markLeft != high->markLeft || u->markRight != high->markRight)) {
			continue;
		}
		//5. Mark u as visited
		//f(u) := v, v:= u and go to step(1)
		u->visited = 1;
		u->fv = v;
		v = u;
		
	}
	
	//6. (u = low)
	if(u == low) {
		//6. (u = low)
		p = new path;
		p->pvertices.push_back(u);
		//Until v= high do: Path:=v U Path and v:=f(v)
		while(v!=high) {
			p->push_front(v);
			v = v->fv;
		}
	}
	
	//7. Open(xj, xj+1)
	for(int j = 0; j < p->pvertices.size()-1; j++) {
		vertex* xj = p->vertices[j];
		if(xj->blos != B) {
			//replace xj and xj+1 with OPEN(xj, xj+1) in Path
			vertex* xjplus1 = p->vertices[j+1];
			path* openp = open(xj, xjplus1);
			std::deque<int>::iterator it = p->pvertices.begin()+j;
			p->pvertices.insert(it, openp->pvertices.begin(), openp->pvertices.end());
		}
	}
	
	//8. return path
	return p;
}

/***************************subroutine open()*******************************/
path *open(vertex* entrance, vertex* base) {
	blossom* B = entrance->blos;
	
	//1. if entrance is outer: level is even
	if(entrance->level()%2 == 0) {
		//path:= findpath(entrance, base, B) and return
		path* p = findpath(entrance, base, B);
		return p;
	} else { // entrance is inner
		//let peakL and peakR be the peak vertiecs of B
		vertex* peakL = B->peakL;
		vertex* peakR = B->peakR;
		path *p1;
		if(entrance->markLeft == 1) {
			p1 = findpath(peakL, entrance, B);
			inv(p1);
			path* p2 = findpath(peakR, base, B);
			contraction(p1, p2);
		} else {
			p1 = findpath(peakR, entrance, B);
			inv(p1);
			path* p2 = findpath(peakL, base, B);
			contraction(p1, p2);
		}
		return p1;
	}
	return NULL;
}

/***************************subroutine contraction(p1, p2)*****************/
void contraction(path* p1, path* p2) {
	std::deque<int>::iterator it = p1->pvertices.end();
	p1->pvertices.insert(it, p2->pvertices.begin(), p2->pvertices.end());
}

/****************************subroutine inv(path)**************************/
void inv(path* p) {
	int size = p->pvertices.size();
	for(int i = 0; i < (size-1)/2; i++) {
		vertex* front = p->pvertices[i];
		vertex* back = p->vertices[size -1- i];
		p->pvertices[i] = back;
		p->pvertices[size -1 - i] = front;
	}
}

/**************************subroutine augment(p1)**************************/
void augmentanderase(path* p) {
	int size = p->pvertices.size();
	vertex *u;
	vertex *v;
	p->pvertices[0]->erase = 1;
	for(int i = 0; i < size-1; i++) {
		v = p->pvertices[i];
		u = p->pvertices[i+1];
		if(i%2 == 0) {
			v->matched = 1;
			v->matchNeighbor = u;
						
			u->matched = 1;
			u->matchNeighbor = v;
		} 
		u->erase = 1;
	}
	//TODO: 1. erase edges in path from bridges
	//TODO: 2. take erase = 0 in code
}
		
			
		