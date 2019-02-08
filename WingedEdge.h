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

struct SdBuffer
{
	int nVertices;
	int mFaces;
	Vector3f center;
	float scale;

	Vector3f* vertices;
	Vector3i* faces;
};

class WingedEdge
{
public:
	WingedEdge(string fileName) {
		readObj(fileName);
		constructLeft();
		findCenterScale();
	}
	WingedEdge(SdBuffer buffer) {
		readSd(buffer);
		constructLeft();
		findCenterScale();
	}
	~WingedEdge() {
		delete []vertices;
		delete []faces;
		delete []w_edges;
	}
	MatrixXf getPositions();
	MatrixXf getNormals(MatrixXf positions);
	MatrixXf getSmoothNormals(MatrixXf* normals);
	MatrixXf getColors();
	void writeObj(string fileName);
	SdBuffer sdLoop();
	SdBuffer sdBtfl();
private:
	int nVertices = 0;
	int mFaces = 0;
	int lW_edges = 0;

	Vertex* vertices;
	Face* faces;
	W_edge* w_edges;
    
	Vector3f center;
	float scale;

	void readObj(string filename);
	void readSd(SdBuffer buffer);
	void constructLeft();
	void findCenterScale();

	Vector3f getVertexSN(Vertex* v, MatrixXf* normals);
};
#endif //WINGEDEDGE_H
