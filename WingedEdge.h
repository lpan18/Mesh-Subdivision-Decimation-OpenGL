#include <nanogui/common.h>
#include <string>

using namespace std;
using nanogui::Vector3f;
using nanogui::Vector3i;
using nanogui::MatrixXf;

#ifndef WINGEDEDGE_H
#define WINGEDEDGE_H

struct W_edge
{
	struct Vertex* start; Vertex* end;
	struct Face* left; Face* right;
	W_edge* left_prev; W_edge* left_next;
	W_edge* right_prev; W_edge* right_next;
	// Used in Subdivision
	Vector3f* edgeVertex = NULL;
	// The left W_edge
	W_edge* leftW_edge() {
		return left_prev->right_next;
	}
};

struct Vertex
{
	Vector3f p;
	W_edge *edge;
};

struct Face
{
	W_edge *edge;
};

// Intermediate data of subdivision
struct ObjBuffer
{
	int nVertices;
	int mFaces;
	Vector3f center;
	float scale;

	Vector3f* vertices;
	Vector3i* faces;
};

// Main WingedEdge class
class WingedEdge
{
public:
    // Regular constructor
	WingedEdge(string fileName) {
		ObjBuffer buffer = readObj(fileName);
		readObjBuffer(buffer);
		constructLeft();
	}
	// Constructor used in subdivision
	WingedEdge(ObjBuffer buffer) {
		readObjBuffer(buffer);
		constructLeft();
	}
	~WingedEdge() {
		delete[] vertices;
		delete[] faces;
		delete[] w_edges;
	}
	// Get mesh vertex positions
	MatrixXf getPositions();
	// Get normals for flat shading
	MatrixXf getNormals(MatrixXf* positions);
	// Get normals for smooth shading
	MatrixXf getSmoothNormals(MatrixXf* normals);
	// Get colors
	MatrixXf getColors();
	// Write mesh to an obj file
	void writeObj(string fileName);
	// Loop subdivision
	ObjBuffer sdLoop();
	// Butterfly subdivision
	ObjBuffer sdBtfl();
private:
    // Number of vertices
	int nVertices = 0;
	// Number of faces
	int mFaces = 0;
	// Number of W_edges
	int lW_edges = 0;

	// Pointer to vertex array
	Vertex* vertices;
	// Pointer to face array
	Face* faces;
	// Pointer to w_edge array
	W_edge* w_edges;
    
	// Center of the mesh
	Vector3f center;
	// Scale to be multiplied to the original mesh
	float scale;
	
	// Read obj file
	ObjBuffer readObj(string filename);
	// Read obj buffer
	void readObjBuffer(ObjBuffer buffer);
	// Fill in left parameters (left_prev, left_next, and left) of W_edge
	void constructLeft();
	// Get vertex normals for smooth shading
	Vector3f getVertexSN(Vertex* v, MatrixXf* normals);
};
#endif //WINGEDEDGE_H
