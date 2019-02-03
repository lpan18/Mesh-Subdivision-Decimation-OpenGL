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
#include <Eigen/Geometry>

#include "WingedEdge.h"

using namespace std;

bool sortByStartVertexThenByEndVertex(const W_edge* edge1, const W_edge* edge2) {
	if (edge1->start < edge2->start)
		return true;
	else if (edge1->start > edge2->start)
		return false;
	else
		return edge1->end < edge2->end;
}

MatrixXf WingedEdge::getPositions() {
	MatrixXf positions = MatrixXf(3, mFaces * 3);
	for (int i = 0; i < mFaces; i++) {
		positions.col(i * 3) << (faces[i].edge->end->p - center) * scale;
		positions.col(i * 3 + 1) << (faces[i].edge->start->p - center) * scale;
		positions.col(i * 3 + 2) << (faces[i].edge->right_prev->start->p - center) * scale;
	}
	return positions;
}

MatrixXf WingedEdge::getNormals(MatrixXf positions) {
	MatrixXf normals = MatrixXf(3, mFaces * 3);

	for (int i = 0; i < mFaces; i++) {
		Vector3f e1 = positions.col(i * 3 + 1) - positions.col(i * 3);
		Vector3f e2 = positions.col(i * 3 + 2) - positions.col(i * 3 + 1);
		Vector3f normal = (e2.cross(e1)).normalized();

		normals.col(i * 3) = normals.col(i * 3 + 1) = normals.col(i * 3 + 2) = normal;
	}
	return normals;
}

MatrixXf WingedEdge::getColors() {
	MatrixXf colors = MatrixXf(3, mFaces * 3);
	for (int i = 0; i < mFaces * 3; i++) {
		colors.col(i) << 1, 0, 0;
	}
	return colors;
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
				float x, y, z;
				iss >> x >> y >> z;
				vertices[vertexi].p = Vector3f(x,y,z);
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
					} else {
						edges[w_edgei - 1].start = vertices + vn - 1;
						edges[w_edgei].right_next = edges + w_edgei - 1;
						edges[w_edgei - 1].right_prev = edges + w_edgei;
					}

					w_edgei++;
				}

				edges[w_edgei - 1].start = vertices + start_vn - 1;
				edges[start_w_edgei].right_next = edges + w_edgei - 1;
				edges[w_edgei - 1].right_prev = edges + start_w_edgei;

				faces[facei].edge = edges + start_w_edgei;

				// // Find the edge with the smallest start vertex
				// faces[facei].edge = edges + start_w_edgei;
				// for (int i = start_w_edgei; i < w_edgei; i++) {
				// 	if (edges[i].start - faces[facei].edge->start < 0) {
				// 		faces[facei].edge = edges + i;
				// 	}
				// }

				facei++;
			}
		}
	}
}

void WingedEdge::constructLeft() {
	W_edge** w_edgeP = new W_edge*[lW_edges];
	for (int i = 0; i < lW_edges; i++) {
		w_edgeP[i] = edges + i;
	}

	sort(w_edgeP, w_edgeP + lW_edges, sortByStartVertexThenByEndVertex);

	int bi = 0;
	for (int i = 0; i < lW_edges; i++) {
		if (i == lW_edges - 1 || w_edgeP[i + 1]->start != w_edgeP[i]->start) {
			for (int j = bi; j < i + 1; j++) {
				for (int k = bi; k < i + 1; k++) {
					// This is a match
					if (w_edgeP[j]->start == w_edgeP[k]->right_prev->end && w_edgeP[j]->end == w_edgeP[k]->right_prev->start) {
						w_edgeP[i]->left_prev = w_edgeP[j]->right_prev;
						w_edgeP[i]->left_next = w_edgeP[j]->right_next;
						w_edgeP[i]->left = w_edgeP[j]->right;
						w_edgeP[j]->left_prev = w_edgeP[i]->right_prev;
						w_edgeP[j]->left_next = w_edgeP[i]->right_next;
						w_edgeP[j]->left = w_edgeP[i]->right;
					}
				}
			}
			bi = i + 1;
		}
	}
}

void WingedEdge::findCenterScale() {
	float maxX = numeric_limits<float>::min();
	float maxY = numeric_limits<float>::min();
	float maxZ = numeric_limits<float>::min();
	float minX = numeric_limits<float>::max();
	float minY = numeric_limits<float>::max();
	float minZ = numeric_limits<float>::max();

	for (int i = 0; i < nVertices; i++) {
		if (vertices[i].p.x() > maxX) maxX = vertices[i].p.x();
		if (vertices[i].p.y() > maxY) maxY = vertices[i].p.y();
		if (vertices[i].p.z() > maxZ) maxZ = vertices[i].p.z();
		if (vertices[i].p.x() < minX) minX = vertices[i].p.x();
		if (vertices[i].p.y() < minY) minY = vertices[i].p.y();
		if (vertices[i].p.z() < minZ) minZ = vertices[i].p.z();
	}

	center = Vector3f(maxX / 2.0f + minX / 2.0f, maxY / 2.0f + minY / 2.0f, maxZ / 2.0f + minZ / 2.0f);
	Vector3f maxOffset = Vector3f(maxX, maxY, maxZ) - center;
	scale = 1.0f / maxOffset.maxCoeff();
}

// int main() {
// 	const char * path = "testfiles/venus.obj";

// 	WingedEdge we = WingedEdge((char*)path);

// 	cout << "Read Obj Complete" << endl;
// }