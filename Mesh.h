#include <nanogui/common.h>
#include <string>
#include "W_edge.h"

using namespace std;
using nanogui::Vector3f;
using nanogui::Vector3i;
using nanogui::Vector4f;
using nanogui::Matrix4f;
using nanogui::MatrixXf;

#ifndef MESH_H
#define MESH_H

// Intermediate data of subdivision
struct ObjBuffer
{
	int nVertices;
	int mFaces;
	Vector3f center;
	float scale;

	Vector3f* vertices;
	Vector3i* faces;

	void setCenterAndScale();
};

// Main Mesh class
class Mesh
{
public:
    // Regular constructor
	Mesh(string fileName) {
		ObjBuffer buffer = readObj(fileName);
		readObjBuffer(buffer);
		constructLeft();
	}
	// Constructor used in subdivision
	Mesh(ObjBuffer buffer) {
		readObjBuffer(buffer);
		constructLeft();
	}
	~Mesh() {
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
	// Multiple choices decimation
	ObjBuffer mcd(int k, int countCollapse);
protected:
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
	// One step of multiple choices decimation
	// return true if successful, false if not
	bool mcdOneStep(int k, vector<W_edge*>& validW_edges);
	// Collapse a w_edge
	void mcdCollapse(W_edge* w_edge);
};
#endif //MESH_H
