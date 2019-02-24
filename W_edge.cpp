#include "W_edge.h"

vector<Face*> Vertex::getFaces() {
	vector<Face*> vec;
	W_edge *e0 = edge->end == this ? edge->leftW_edge() : edge;
	W_edge *e = e0;
    
	do {
		if (e->end == this) {
			vec.push_back(e->right);
			e = e->right_next;
		} else {
			vec.push_back(e->left);
			e = e->left_next;
		}
	} while (e != e0);

	if (vec.size() == 0) throw "No face at vertex v";

	return vec;
}

vector<W_edge*> Vertex::getEdges() {
	vector<W_edge*> vec;
	W_edge *e0 = edge->end == this ? edge->leftW_edge() : edge;
	W_edge *e = e0;
    
	do {
		vec.push_back(e);
		if (e->end == this) {
			e = e->right_next;
		} else {
			e = e->left_next;
		}
	} while (e != e0);

	if (vec.size() == 0) throw "No edge at vertex v";

	return vec;
}

// Number of faces at Vertex v
int Vertex::countFaces() {
	return (int)getFaces().size();
}