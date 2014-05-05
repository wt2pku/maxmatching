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
bool blossAug(vertex *w1, vertex *w2, int searchLevel );
path* findpath(vertex* high, vertex* low, blossom* B);
path *open(vertex* entrance, vertex* base);
void contraction(path* p1, path* p2);
void inv(path* p);
void augmentanderase(path* p);
vertex* basestar(blossom* B);

/**********************extern variables**********************/
//the vector vertices are to store the vertices in the graph
extern std::vector<vertex*> vertices;
//the map edges are to sotre the edges in the graph
//std::map<std::pair<int, int>, edge*> edges;
std::vector< vector<edge*> > bridges;
/***********************global variables**********************/


//////////////////////////////////////////////////////////////
/**************main routine search***************************/
void search() {
	/**************first define a matching****************/
	//std::map<std::pair<int, int>, edge*> matchings;

	for(unsigned int i = 0; i < vertices.size(); i++) {
		if(vertices[i]->matched == 0) {
			if(!vertices[i]->neighbors.empty()) {
				for(unsigned int j = 0; j < vertices[i]->neighbors.size(); j++) {
					vertex* u = vertices[i]->neighbors[j];
					if(u->matched == 0) {
						vertices[i]->matched = 1;
						vertices[i]->matchNeighbor = u;
						
						u->matched = 1;
						u->matchNeighbor = vertices[i];
						break;
					}
				}
			}
		}
	}
	
	int searchLevel = 0;
	
	/************Step 1: initialize the free vertex, evenlevel(v):= 0******/
	vector<vertex*> currentSearchSet;
	
	bool halt = false;
	while(!halt) {
		searchLevel = 0;
		for(unsigned i = 0; i < bridges.size(); i++) {
			bridges[i].clear();
		}
		
		currentSearchSet.clear();
		for(unsigned i = 0; i < vertices.size(); i++) {
			vertex* v = vertices[i];
			if(v->erase == 0) {
				v->evenlevel = INFTY;
				v->oddlevel = INFTY;
				v->blos = NULL;
				v->predecessors.clear();
				v->anomalies.clear();
				v->visited = 0;
				for(unsigned int j = 0; j < v->neighbors.size(); j++) {
					v->edgeVisit[j] = 0;
					v->edgeUsed[j] = 0;
				}
			}
			
			if(vertices[i]->matched == 0 && vertices[i]->erase == 0) {
				vertices[i]->evenlevel = 0;
				currentSearchSet.push_back(vertices[i]);
			}
			vector<edge*> tmpBridge;
			bridges.push_back(tmpBridge);
		}
		
		//int index;
		//int neighborIndex;
		
		int largestLevel = vertices.size();
		bool restart = false;
		while(searchLevel <= largestLevel && restart == false) {
			printf("currentSearchSet size: %d\n", currentSearchSet.size());
			printf("searchLevel = %d\n", searchLevel);
			//if searchLevel is even
			if(searchLevel%2 == 0) {
				//for each v with evenlevel(v) = searchLevel
				for(unsigned int index = 0; index < currentSearchSet.size(); index++) {
					vertex* v = currentSearchSet[index];
					//if v has been erased
					if(v->erase == 1) {
						continue;
					}
					for(unsigned int neighborIndex = 0; neighborIndex < v->neighbors.size(); neighborIndex++) {
						if(v->neighbors[neighborIndex]->erase == 1) {
							continue;
						}
						//for each v with evenlevel(v) = searchLevel, find its unmatched, unused and unerased neighbors
						if(v->matchNeighbor != v->neighbors[neighborIndex] && v->edgeUsed[neighborIndex] == 0 && v->neighbors[neighborIndex]->erase == 0) {
							//such neighbor is u
							vertex* u = v->neighbors[neighborIndex];
							
							//if evenlevel(u) is finit
							if(u->evenlevel < INFTY) {
								//then temp := (evenlevel(u) + evenlevel(v))/2
								int temp = (u->evenlevel + v->evenlevel) / 2;
								edge *tmpE = new edge(u, v);
								bridges[temp].push_back(tmpE);
							} else { 
								//(a) handle oddlevel
								if( u->oddlevel == INFTY) {
									u->oddlevel = searchLevel + 1;
									
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
				for(unsigned int index = 0; index < currentSearchSet.size(); index++) {
					vertex *v = currentSearchSet[index];
					//if v has been erase
					if(v->erase == 1 || v->blos != NULL) {
						continue;
					}
					
					if(v->oddlevel == searchLevel && v->blos== NULL) {
						//take v's matched neighbor u
						vertex* u = v->matchNeighbor;
						//if u is defined and not erased
						if(u != NULL && u->erase == 0) {
							
							//(a) handle bridge
							if(u->oddlevel == searchLevel) {
								//printf("u->oddlevel: %d\n", u->oddlevel);
								//printf("v->id: %d\n", v->id);
								//printf("u->id: %d\n", u->id);
								//printf("v->matchedNeighbor->id: %d\n", v->matchNeighbor->id);
			//					printf("in handle(a)\n");
								int temp = (u->oddlevel + v->oddlevel) / 2;
								edge *tmpE = new edge(u, v);
								//bridges(temp) := bridges(temp) union (u,v)
								bridges[temp].push_back(tmpE);
							}
							//(b) handle predecessors
							if(u->oddlevel == INFTY) {
			//					printf("in handle(b)\n");
								u->evenlevel = searchLevel + 1;
								//u will be in next search level
								//nextSearchSet.push_back(u);
								u->predecessors.push_back(v);
							}
						}
							
					}
				}
			}
			
		//	printf("Before bridges!\n");
		//	printf("bridges[searchLevel].size(): %ld\n", bridges[searchLevel].size());
			//int edgeIndex;
			for(unsigned int edgeIndex = 0; edgeIndex < bridges[searchLevel].size(); edgeIndex++) {
				edge* theEdge = bridges[searchLevel][edgeIndex];
				if(theEdge->left->erase == 0 && theEdge->right->erase == 0) {
					bool endofphase = blossAug(theEdge->left, theEdge->right, searchLevel);
					if(endofphase) {
						restart = true;
						break;
					}
				}
			}
		
		
			printf("arrive restart!\n");
			if(restart) {
				printf("restart = true!\n");
				continue;
			}

			searchLevel = searchLevel + 1;
			currentSearchSet.clear();
			bool nosearch = true;
			for(unsigned int index = 0; index < vertices.size(); index++) {
				if(vertices[index]->level() == searchLevel && vertices[index]->erase == 0) {
					nosearch = false;
					currentSearchSet.push_back(vertices[index]);
				}
			}
			if(nosearch) {
				printf("in no search!\n");
				halt = true;
				break;
			}
		}
		
		
	}
	
}

/*******************subroutine blossAug*****************************************************/
bool blossAug(vertex *w1, vertex *w2, int searchLevel ) {
	//if w1 and w2 belong to the same blossom then go to step(5), that is return
	printf("In blossAug!\n");
	if(w1->blos != NULL && w2->blos != NULL && w1->blos == w2->blos) {
		return false;
	}
	
	for(unsigned i = 0; i < vertices.size(); i++) {
		vertex* v = vertices[i];
		if(v->erase == 0) {
			v->visited = 0;
			for(unsigned int j = 0; j < v->neighbors.size(); j++) {
				v->edgeVisit[j] = 0;
				v->edgeUsed[j] = 0;
			}
		}
	}
	
	vertex* vl;
	vertex* vr;
	
	//(0) initialization: assign vl, vr
	if(w1->blos != NULL) {
		vl = basestar(w1->blos);
	} else {
		vl = w1;
	}
	
	if(w2->blos != NULL) {
		vr = basestar(w2->blos);
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
		if(vl->matched == 0 && vr->matched == 0) {
		//P:= (FINDPATH(w1, vl, undefined))-1 con FINDPATH(w2, vr, undefined)
			path* p1 = findpath(w1, vl, NULL);
			inv(p1);
			path* p2 = findpath(w2, vr, NULL);
			contraction(p1, p2);
			//augment the matching along P, do a topological erase, and go to step(5) return
			augmentanderase(p1);
			
			return true;
		} else {
			//(1.2) if vl and vr are not both free vertices
			//(2.1) if level(vl) <= level(vr)
			int vlLevel = vl->level();
			int vrLevel = vr->level();
			if(vlLevel == 0 && vrLevel == 0) {
				break;
			}
			if(vlLevel > vrLevel || vlLevel == vrLevel) {
				//mark vl "L" and visit
				vl->markLeft = 1;
				vl->visited = 1;
				bool noUnused = true;
				vertex* u = NULL;
				int uIndex;
				printf("in vl\n");
				printf("vl->id: %d\n", vl->id);
			//	printf("w1->id: %d\n", w1->id);
				if(vl->predecessors.size() != 0) {
					for(unsigned int i = 0; i < vl->predecessors.size(); i++) {
						if(vl->predecessors[i]->erase == 1) {
							continue;
						}
						int mapi = vl->neighborMap[vl->predecessors[i]->id];
			//			printf("mapi = %d\n", mapi);
			//			printf("pre->id: %d\n", vl->predecessors[i]->id);
					
						if(vl->edgeUsed[mapi] == 0) {
							noUnused = false;
							//u is an unused edge
							u = vl->predecessors[i];
				//			printf("u->id: %d\n", u->id);
							uIndex = mapi;
							break;
						} 
						
					}
				}
				//(2.1) If vl has no more "unused" ancestor edges then
				if(noUnused) {
					//if fvl is undefined
					if(vl->fv == NULL) {
		//				if(vl == w1) {
		//					printf("vl == w1\n");
		//					printf("vl->predecessor.size() : %d\n", vl->predecessors.size());
		//				}

						//then go to step (4) (create a new blossom)
						stop = true;
						newBlos = true;
						break;
					} else {
						// vl := f(vl) and go to step (1.1)
			//			printf("go back to f(v)!\n");
						vl = vl->fv;
						continue;
					}
				} else { //(2.2) vl has "unused" ancestor edges
					//mark vl-u used
					vl->noUnused = 0;
					vl->edgeUsed[uIndex] = 1;
					int vlInu = u->neighborMap[vl->id];
					u->edgeUsed[vlInu] = 1;
					//if u in B, u:= base(B)
					if(u->blos != NULL) {
						u = basestar(u->blos);
					}
					//(a) if u is unmarked, then mark u "left", f(u) := vl, vl :=u and go to (1.1)
					if(u->markLeft== 0 && u->markRight == 0) {
			//			printf("In mark left!\n");
						u->markLeft = 1;
						Bvertices.push_back(u);
						u->fv = vl;
						vl = u;
						continue;
					} else {//(b) Otherwise(u is marked)
						//If u = barries or u!= vr, go to step(1.1)
						if( u == barrier || u!= vr) {
//							printf("In u = barrier!\n");
							continue;
						} else {//mark u "left", vr := f(vr), vl := u, DCV := u, and go to step(1.1)
							u->markLeft = 1;
//							printf("In mark left and DCV is assigned!\n");
							Bvertices.push_back(u);
							vr = vr->fv;
							vl = u;
							DCV = u;
							continue;
						}
					}
				}
			} else if(vrLevel > vlLevel) { //(3.1) level(vl) > level(vr) 
				printf("In vr!\n");
				printf("vr->level(): %d\n", vr->level());
				printf("vl->level(): %d\n", vl->level());
				//printf("vr->id: %d\n", vr->id);
				bool noUnused = true;
				vertex* u = NULL;
				int uIndex;
				if(vr->predecessors.size() != 0) {
					for(unsigned int i = 0; i < vr->predecessors.size(); i++) {
						if(vr->predecessors[i]->erase == 1) {
							continue;
						}
						
						int mapi = vr->neighborMap[vr->predecessors[i]->id];
						if(vr->edgeUsed[mapi] == 0) {
							noUnused = false;
							//u is an unused edge
							u = vr->neighbors[mapi];
							uIndex = mapi;
							break;
						} 
						
					}
				}
				//(3.1) If vr has no more "unused" ancestor edges then
				if(noUnused) {
					printf("In noUnused\n");
					// if vr = barrier
					if(vr == barrier) {
						printf("vr->pre.size():%d\n", vr->predecessors.size());
						if(DCV == NULL) {
							printf("DCV is NULL!\n");
							//break;
						}
						
						// vr := DCV, barrier := DCV, mark vr "right"
						//vr = DCV;
						//barrier = DCV;
						barrier = vl;
						vr = vl;
						if(vr == NULL) {
							printf("vr == NULL\n");
						}
						
						vr->markRight = 1;
						Bvertices.push_back(vr);
						//vl:= f(vl) and go to step (1.1)
						if(vl == NULL) {
							printf("vl == NULL\n");
						}
						vl = vl->fv;
						continue;
					} else {
						if(vr->fv != NULL) {
							vr = vr->fv;
						}
					}
				} else { //(3.2) vr has "unused" ancestor edges
					//mark vr-u used
					printf("In unused\n");
					vr->edgeUsed[uIndex] = 1;
					int vrInu = u->neighborMap[vr->id];
					u->edgeUsed[vrInu] = 1;
					//if u in B, u:= base(B)
					if(u->blos != NULL) {
						u = basestar(u->blos);
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
						printf("u->id: %d", u->id);
						printf("vl->id: %d", vl->id);
						if(u == vl) {
							DCV = u;
						}
						//DCV = u;
						continue;
					}
				}
			}
		}
	}
	
	//(4) (creation of a newblossom)
	if(newBlos) {
		if(DCV == NULL || w1 == NULL || w2 == NULL) {
				return false;
		}
		printf("In creation of new blossom!\n");
		blossom *b = new blossom(DCV, w1, w2);
		//remove the right mark from DCV
		DCV->markRight = 0;
		printf("DCV != NULL\n");
		printf("Bvertices.size(): %ld\n", Bvertices.size());
		
		for(unsigned int uIndex = 0; uIndex < Bvertices.size(); uIndex++) {
			if(Bvertices[uIndex] != DCV) {
				vertex* u = Bvertices[uIndex];
				u->blos = b;
				//if u is outer then
				if(u->level()%2 == 0) {
					printf("In evenlevel\n");
					u->oddlevel = 2*searchLevel + 1 - u->evenlevel;
				} else {
					u->evenlevel = 2*searchLevel + 1 - u->oddlevel;
					//for each v in anomalies(u)
					
					printf("In oddlevel\n");
					for(unsigned int vIndex = 0; vIndex < u->anomalies.size(); vIndex++) {
						if(u->anomalies[vIndex] == NULL || u->anomalies[vIndex]->erase == 1) {
							continue;
						}
						
						vertex* v = u->anomalies[vIndex];
						int temp = (u->evenlevel + v->evenlevel)/2;
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
		printf("Finish crease blossom!\n");
	}
	
	return false;
}
	
					
					
					

/******************subroutine findpath***************************************************/
path* findpath(vertex* high, vertex* low, blossom* B) {
	printf("In findpath!\n");
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
		while(v != NULL && noUnvisited) {
			//1. If v has no more "unvisited" predecessor edges
			for(unsigned int i = 0; i < v->predecessors.size(); i++) {
				if(v->predecessors[i]->erase == 1) {
					continue;
				}
				
				int mapi = v->neighborMap[v->predecessors[i]->id];
				if(v->edgeVisit[mapi] == 0) {
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
			for(unsigned int i = 0; i < v->predecessors.size(); i++) {
				if(v->predecessors[i]->erase == 1) {
					continue;
				}
				int mapi = v->neighborMap[v->predecessors[i]->id];
				if(v->edgeVisit[mapi] == 0) {
					u = v->predecessors[i];
					v->edgeVisit[mapi] = 1;
					int mapj = u->neighborMap[v->id];
					u->edgeVisit[mapj] = 1;
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
		if(u->visited == 0 || u->level() < low->level() || (u->blos == B && (u->markLeft != high->markLeft || u->markRight != high->markRight))) {
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
			p->pvertices.push_front(v);
			v = v->fv;
		}
	}
	
	//7. Open(xj, xj+1)
	for(unsigned int j = 0; j < p->pvertices.size()-1; j++) {
		vertex* xj = p->pvertices[j];
		if(xj->blos != B) {
			//replace xj and xj+1 with OPEN(xj, xj+1) in Path
			vertex* xjplus1 = p->pvertices[j+1];
			path* openp = open(xj, xjplus1);
			std::deque<vertex*>::iterator it = p->pvertices.begin()+j;
			p->pvertices.insert(it, openp->pvertices.begin(), openp->pvertices.end());
		}
	}
	
	//8. return path
	return p;
}

/***************************subroutine open()*******************************/
path *open(vertex* entrance, vertex* base) {
	printf("In open!\n");
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
	printf("In contraction!\n");
	if(p1 == NULL && p2 != NULL) {
		p1->pvertices = p2->pvertices;
	}
	
	if(p1 != NULL && p2 == NULL) {
		return;
	}
	std::deque<vertex*>::iterator it = p1->pvertices.end();
	p1->pvertices.insert(it, p2->pvertices.begin(), p2->pvertices.end());
}

/****************************subroutine inv(path)**************************/
void inv(path* p) {
	printf("In inv!\n");
	
	int size = p->pvertices.size();
	if(size == 0) {
		return;
	}
	
	for(int i = 0; i < size/2; i++) {
		vertex* front = p->pvertices[i];
		vertex* back = p->pvertices[size -1- i];
		p->pvertices[i] = back;
		p->pvertices[size -1 - i] = front;
	}
}

/**************************subroutine augment(p1)**************************/
void augmentanderase(path* p) {
	printf("In augmentanderase!\n");
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
			v->erase = 1;
			u->erase = 1;
		}
		
	}
	//TODO: 1. erase edges in path from bridges
	//TODO: 2. take erase = 0 in code
}

/************************subroutine basestar()*******************************/
vertex* basestar(blossom* B) {
	blossom* blo = B;
	vertex* base = blo->base;
	while(base->blos != NULL) {
		blo = base->blos;
	}
	return base;
}
		
			
		
