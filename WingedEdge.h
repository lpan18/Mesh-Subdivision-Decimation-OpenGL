#include <nanogui/common.h>
#include <string>

using namespace std;
using nanogui::Vector3f;
using nanogui::MatrixXf;

#ifndef WINGEDEDGE_H
#define WINGEDEDGE_H

struct W_edge
{
	struct Vertex* start; Vertex* end;
	struct Face* left; Face* right;
	W_edge* left_prev; W_edge* left_next;
	W_edge* right_prev; W_edge* right_next;
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

class WingedEdge
{
public:
	WingedEdge(string fileName) {
		readObj(fileName);
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
	MatrixXf getColors();
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
	void constructLeft();
	void findCenterScale();
};
#endif //WINGEDEDGE_H
