#include <string>

#ifndef WINGEDEDGE_H
#define WINGEDEDGE_H

// TODO: add headers that you want to pre-compile here
struct W_edge
{
	struct Vertex* start; Vertex* end;
	struct Face* left; Face* right;
	W_edge* left_prev; W_edge* left_next;
	W_edge* right_prev; W_edge* right_next;
};

struct Vertex
{
	float x, y, z = 0;
	W_edge *edge;
};

struct Face
{
	W_edge *edge;
};

class WingedEdge
{
public:
	WingedEdge(std::string fileName) {
		readObj(fileName);
		constructLeft();
	}
	~WingedEdge() {
		delete []vertices;
		delete []faces;
		delete []edges;
	}

private:
	int nVertices = 0;
	int mFaces = 0;
	int lW_edges = 0;

	Vertex* vertices;
	Face* faces;
	W_edge* edges;

	void readObj(std::string filename);
	void constructLeft();
	void constructLeftRange(int starti, int endi);
};
#endif //WINGEDEDGE_H
