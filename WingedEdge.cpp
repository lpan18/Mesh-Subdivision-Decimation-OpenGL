#include <algorithm>
#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "WingedEdge.h"

using namespace std;

// bool sortByStartVertexThenByEndVertex(const Face &face1, const Face &face2) {
// 	if (face1.edge->start < face2.edge->start)
// 		return true;
// 	else if (face1.edge->start > face2.edge->start)
// 		return false;
// 	else
// 		return face1.edge->end < face2.edge->end;
// }

bool sortByStartVertexThenByEndVertex(const W_edge & edge1, const W_edge &edge2) {
	if (edge1.start < edge2.start)
		return true;
	else if (edge1.start > edge2.start)
		return false;
	else
		return edge1.end < edge2.end;
}

void WingedEdge::readObj(string filename) {
	string line;
	int vn;

	// Read the file and get the numbers of vertices, faces, and w_edges.
	ifstream fin(filename);
	while (getline(fin, line)) {
		if (line.length() > 1) {
			if (line[0] == 'v' && line[1] == ' ') {
				nVertices++;
			}
			else if (line[0] == 'f' && line[1] == ' ') {
				mFaces++;
				string str = line.substr(2, line.size() - 1);
				istringstream iss(str);
				while (iss >> vn) {
					lW_edges++;
				}
			}
		}
	}

	// Generate vertex, face, and w_edge arrays
	vertices = new Vertex[nVertices];
	faces = new Face[mFaces];
	edges = new W_edge[lW_edges];

	// read the file again and initialize vertices, faces, and w_edges.
	ifstream fin1(filename);
	int vertexi = 0;
	int facei = 0;
	int w_edgei = 0;

	while (getline(fin1, line)) {
		if (line.length() > 0) {
			if (line[0] == 'v' && line[1] == ' ') {
				string str = line.substr(2, line.size() - 1);
				istringstream iss(str);

				iss >> vertices[vertexi].x;
				iss >> vertices[vertexi].y;
				iss >> vertices[vertexi].z;

				vertexi++;
			} else if (line[0] == 'f' && line[1] == ' ') {
				string str = line.substr(2, line.size() - 1);
				istringstream iss(str);
				int start_w_edgei = w_edgei;
				int start_vn = 0;
				while (iss >> vn) {
					// obj file is counterclockwise, while winged-edge structure is clock wise
					edges[w_edgei].end = vertices + vn - 1;
					edges[w_edgei].right = faces + facei;
					vertices[vn - 1].edge = edges + w_edgei;

					if (start_vn == 0) {
						start_vn = vn;
					}
					else {
						edges[w_edgei - 1].start = vertices + vn - 1;
						edges[w_edgei].right_next = edges + w_edgei - 1;
						edges[w_edgei - 1].right_prev = edges + w_edgei;
					}

					w_edgei++;
				}

				edges[w_edgei - 1].start = vertices + start_vn - 1;
				edges[start_w_edgei].right_next = edges + w_edgei - 1;
				edges[w_edgei - 1].right_prev = edges + start_w_edgei;

				// Find the edge with the smallest start vertex
				faces[facei].edge = edges + start_w_edgei;
				for (int i = start_w_edgei; i < w_edgei; i++) {
					if (edges[i].start - faces[facei].edge->start < 0) {
						faces[facei].edge = edges + i;
					}
				}

				facei++;
			}
		}
	}
}

void WingedEdge::constructLeft() {
	for (int i = 0; i < lW_edges; i++) {
		for (int j = 0; j < lW_edges; j++) {
			if (edges[i].start == edges[j].end && edges[i].end == edges[j].start) {
				edges[i].left_prev = edges[j].right_prev;
				edges[i].left_next = edges[j].right_next;
				edges[i].left = edges[j].right;
				edges[j].left_prev = edges[i].right_prev;
				edges[j].left_next = edges[i].right_next;
				edges[j].left = edges[i].right;
			}
		}
	}

	// sort(edges, edges + lW_edges, sortByStartVertexThenByEndVertex);

	// int bi = 0;
	// for (int i = 0; i < lW_edges; i++) {
	// 	if (i == lW_edges - 1 || edges[i + 1].start != edges[i].start) {
	// 		constructLeftRange(bi, i + 1);
	// 		bi = i + 1;
	// 	}
	// }

	// cout << nVertices << " " << mFaces << " " << lW_edges << " " << endl;
	// sort(faces, faces + mFaces, sortByStartVertexThenByEndVertex);

	// int starti = 0;
	// int endi = 0;

	// for (int i = 0; i < mFaces; i++) {
	// 	if (i == mFaces - 1 || faces[i + 1].edge->start != faces[i].edge->start) {
	// 		endi = i;
	// 		cout << "Boundary " << i << endl;

	// 		starti = i + 1;
	// 		endi = i + 1;
	// 	}


	// }
}

void WingedEdge::constructLeftRange(int starti, int endi) {
	for (int i = starti; i < endi; i++) {
		for (int j = starti; j < endi; j++) {
			if (edges[i].start == edges[j].right_prev->end && edges[i].end == edges[j].right_prev->start) {
				cout << "match" << i << "  " << j << endl;
			}
		}
	}
}

// int main() {
// 	const char * path = "testfiles/venus.obj";

// 	WingedEdge we = WingedEdge((char*)path);

// 	cout << "Read Obj Complete" << endl;
// }