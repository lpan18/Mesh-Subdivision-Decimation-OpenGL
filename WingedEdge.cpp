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
	MatrixXf positions = MatrixXf(3, mFaces * 9);
	for (int i = 0; i < mFaces; i++) {
		positions.col(i * 3) << (faces[i].edge->end->p - center) * scale;
		positions.col(i * 3 + 1) << (faces[i].edge->start->p - center) * scale;
		positions.col(i * 3 + 2) << (faces[i].edge->right_prev->start->p - center) * scale;

		positions.col(mFaces * 3 + i * 6) << positions.col(i * 3) * 1.005;
		positions.col(mFaces * 3 + i * 6 + 1) << positions.col(i * 3 + 1) * 1.005;
		positions.col(mFaces * 3 + i * 6 + 2) << positions.col(i * 3 + 1) * 1.005;
		positions.col(mFaces * 3 + i * 6 + 3) << positions.col(i * 3 + 2) * 1.005;
		positions.col(mFaces * 3 + i * 6 + 4) << positions.col(i * 3 + 2) * 1.005;
		positions.col(mFaces * 3 + i * 6 + 5) << positions.col(i * 3) * 1.005;
	}
	return positions;
}

MatrixXf WingedEdge::getNormals(MatrixXf positions) {
	MatrixXf normals = MatrixXf(3, mFaces * 9);

	for (int i = 0; i < mFaces; i++) {
		Vector3f e1 = positions.col(i * 3 + 1) - positions.col(i * 3);
		Vector3f e2 = positions.col(i * 3 + 2) - positions.col(i * 3 + 1);
		Vector3f normal = (e1.cross(e2)).normalized();

		normals.col(i * 3) = normals.col(i * 3 + 1) = normals.col(i * 3 + 2) = normal;
		normals.col(mFaces * 3 + i * 6) = normals.col(mFaces * 3 + i * 6 + 1) = normals.col(mFaces * 3 + i * 6 + 2)
		= normals.col(mFaces * 3 + i * 6 + 3) = normals.col(mFaces * 3 + i * 6 + 4) = normals.col(mFaces * 3 + i * 6 + 5)
		= normal;
	}
	return normals;
}

MatrixXf WingedEdge::getSmoothNormals(MatrixXf normals) {
	MatrixXf smoothNormals = MatrixXf(3, mFaces * 9);
	for (int i = 0; i < mFaces; i++) {
		Vertex* v1 = faces[i].edge->end;
		Vertex* v2 = faces[i].edge->start;
		Vertex* v3 = faces[i].edge->right_prev->start;

		smoothNormals.col(i * 3) << getVertexSN(v1, normals);
		smoothNormals.col(i * 3 + 1) << getVertexSN(v2, normals);
		smoothNormals.col(i * 3 + 2) << getVertexSN(v3, normals);
	}
	for (int i = mFaces * 3; i < mFaces * 9; i++) {
		smoothNormals.col(i) << normals.col(i);
	}
	return smoothNormals;
}

MatrixXf WingedEdge::getColors() {
	MatrixXf colors = MatrixXf(3, mFaces * 9);
	for (int i = 0; i < mFaces * 3; i++) {
		colors.col(i) << 1, 0, 0;
	}
	for (int i = mFaces * 3; i < mFaces * 9; i++) {
		colors.col(i) << 0, 0, 0;
	}
	return colors;
}

void WingedEdge::writeObj(string fileName) {
	stringstream ss;
	ss << "# " << nVertices << " " << mFaces << endl;
	for (int i = 0; i < nVertices; i++) {
		ss << "v " << vertices[i].p.x() << " " << vertices[i].p.y() << " " << vertices[i].p.z() << endl;
	}
	for (int i = 0; i < mFaces; i++) {
		ss << "f " << faces[i].edge->end - vertices + 1 << " " << faces[i].edge->start - vertices + 1 << " " << faces[i].edge->right_prev->start - vertices + 1 << endl;
	}

	ofstream outputFile(fileName);
	if (outputFile.is_open())
	{
		outputFile << ss.str();
		outputFile.close();
	}
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
	w_edges = new W_edge[lW_edges];

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
					w_edges[w_edgei].end = vertices + vn - 1;
					w_edges[w_edgei].right = faces + facei;
					vertices[vn - 1].edge = w_edges + w_edgei;

					if (start_vn == 0) {
						start_vn = vn;
					} else {
						w_edges[w_edgei - 1].start = vertices + vn - 1;
						w_edges[w_edgei].right_next = w_edges + w_edgei - 1;
						w_edges[w_edgei - 1].right_prev = w_edges + w_edgei;
					}

					w_edgei++;
				}

				w_edges[w_edgei - 1].start = vertices + start_vn - 1;
				w_edges[start_w_edgei].right_next = w_edges + w_edgei - 1;
				w_edges[w_edgei - 1].right_prev = w_edges + start_w_edgei;

				faces[facei].edge = w_edges + start_w_edgei;
				facei++;
			}
		}
	}
}

void WingedEdge::readSd(SdBuffer buffer) {
	nVertices = buffer.nVertices;
	mFaces = buffer.mFaces;
	lW_edges = buffer.mFaces * 3;

	vertices = new Vertex[nVertices];
	faces = new Face[mFaces];
	w_edges = new W_edge[lW_edges];

	center = buffer.center;
	scale = buffer.scale;

	for (int vertexi = 0; vertexi < nVertices; vertexi++) {
		vertices[vertexi].p = buffer.vertices[vertexi];
	}

	int w_edgei = 0;
	int vns [3] = {0, 0, 0};
	for (int facei = 0; facei < mFaces; facei++) {
		int start_w_edgei = w_edgei;
		int start_vn = 0;
		vns[0] = buffer.faces[facei].x();
		vns[1] = buffer.faces[facei].y();
		vns[2] = buffer.faces[facei].z();

		for (auto vn : vns) {
			// obj file is counterclockwise, while winged-edge structure is clock wise
			w_edges[w_edgei].end = vertices + vn - 1;
			w_edges[w_edgei].right = faces + facei;
			vertices[vn - 1].edge = w_edges + w_edgei;

			if (start_vn == 0) {
				start_vn = vn;
			} else {
				w_edges[w_edgei - 1].start = vertices + vn - 1;
				w_edges[w_edgei].right_next = w_edges + w_edgei - 1;
				w_edges[w_edgei - 1].right_prev = w_edges + w_edgei;
			}

			w_edgei++;
		}
		
		w_edges[w_edgei - 1].start = vertices + start_vn - 1;
		w_edges[start_w_edgei].right_next = w_edges + w_edgei - 1;
		w_edges[w_edgei - 1].right_prev = w_edges + start_w_edgei;

		faces[facei].edge = w_edges + start_w_edgei;
	}

	delete []buffer.vertices;
	delete []buffer.faces;
}

void WingedEdge::constructLeft() {
	W_edge** w_edgeP = new W_edge*[lW_edges];
	for (int i = 0; i < lW_edges; i++) {
		w_edgeP[i] = w_edges + i;
	}

	sort(w_edgeP, w_edgeP + lW_edges, sortByStartVertexThenByEndVertex);

	int bi = 0;
	for (int i = 0; i < lW_edges; i++) {
		if (i == lW_edges - 1 || w_edgeP[i + 1]->start != w_edgeP[i]->start) {
			for (int j = bi; j < i + 1; j++) {
				for (int k = bi; k < i + 1; k++) {
					if (w_edgeP[j]->start == w_edgeP[k]->right_prev->end && w_edgeP[j]->end == w_edgeP[k]->right_prev->start) {
						// This is a match
						w_edgeP[j]->left_prev = w_edgeP[k]->right_prev->right_prev;
						w_edgeP[j]->left_next = w_edgeP[k]->right_prev->right_next;
						w_edgeP[j]->left = w_edgeP[k]->right_prev->right;
						w_edgeP[k]->right_prev->left_prev = w_edgeP[j]->right_prev;
						w_edgeP[k]->right_prev->left_next = w_edgeP[j]->right_next;
						w_edgeP[k]->right_prev->left = w_edgeP[j]->right;
					}
				}
			}
			bi = i + 1;
		}
	}
}

void WingedEdge::findCenterScale() {
	float maxX, maxY, maxZ = numeric_limits<float>::min();
	float minX, minY, minZ = numeric_limits<float>::max();

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

Vector3f WingedEdge::getVertexSN(Vertex* v, MatrixXf normals) {

    Vector3f vec(0, 0, 0);
	int facei = -1;
	W_edge *e0 = v->edge->end == v ? v->edge->right_next : v->edge;
	W_edge *edge = e0;
    
	do {
		if (edge->end == v) {
			facei = edge->right - faces;
			edge = edge->right_next;
		} else {
			facei = edge->left - faces;
			edge = edge->left_next;
		}
		vec += normals.col(facei * 3);
	} while (edge != e0);

	return vec.normalized(); 
}