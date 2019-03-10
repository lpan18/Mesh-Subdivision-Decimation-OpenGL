#include <nanogui/common.h>
#include <string>
#include <vector>
#include "W_edge.h"
#include "Mesh.h"

using namespace std;
using nanogui::Vector3f;
using nanogui::Vector3i;
using nanogui::Vector4f;
using nanogui::Matrix4f;
using nanogui::MatrixXf;

#ifndef MESHSD_H
#define MESHSD_H

// Loop Subdivision, update the positions of existing vertices
Vector3f sdLoopVertex(Vertex* v);

// Loop Subdivision, new vertices at edges
Vector3f sdLoopEdge(W_edge* w_edge);

// Butterfly Subdivision, both sides regular
Vector3f sdBtflEdgeBothRegular(W_edge* w_edge);

float getS(int j, int k);

// Butterfly Subdivision, start vertex regular
Vector3f sdBtflEdgeStartRegular(W_edge* w_edge, int k);

// Butterfly Subdivision, new vertices at edges
Vector3f sdBtflEdge(W_edge* w_edge);

class MeshSd : public Mesh {
public:
    // Regular constructor
	MeshSd(string fileName)
    : Mesh(fileName) {
	}
	// Constructor used in subdivision
	MeshSd(ObjBuffer buffer)
    : Mesh(buffer) {
	}

    // Loop subdivision
	ObjBuffer sdLoop();
	// Butterfly subdivision
	ObjBuffer sdBtfl();
};

#endif // MESHSD_H