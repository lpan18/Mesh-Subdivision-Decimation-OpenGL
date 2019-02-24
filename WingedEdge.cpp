#include <algorithm>
#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <Eigen/Geometry>

#include "W_edge.h"
#include "WingedEdge.h"

using namespace std;

const float PI = 3.14159265359;
const float MAXVALUE = 99999999;

// Function for sorting w_edges
bool sortByStartVertexThenByEndVertex(const W_edge* edge1, const W_edge* edge2) {
	if (edge1->start < edge2->start)
		return true;
	else if (edge1->start > edge2->start)
		return false;
	else
		return edge1->end < edge2->end;
}

void setCenterAndScale(ObjBuffer* buffer) {
	float maxX, maxY, maxZ;
	float minX, minY, minZ;
	maxX = maxY = maxZ = -MAXVALUE;
	minX = minY = minZ = MAXVALUE;

	for (int i = 0; i < buffer->nVertices; i++) {
		maxX = buffer->vertices[i].x() > maxX ? buffer->vertices[i].x() : maxX;
		maxY = buffer->vertices[i].y() > maxY ? buffer->vertices[i].y() : maxY;
		maxZ = buffer->vertices[i].z() > maxZ ? buffer->vertices[i].z() : maxZ;
		minX = buffer->vertices[i].x() < minX ? buffer->vertices[i].x() : minX;
		minY = buffer->vertices[i].y() < minY ? buffer->vertices[i].y() : minY;
		minZ = buffer->vertices[i].z() < minZ ? buffer->vertices[i].z() : minZ;
	}

	buffer->center = Vector3f(maxX / 2.0f + minX / 2.0f, maxY / 2.0f + minY / 2.0f, maxZ / 2.0f + minZ / 2.0f);
	Vector3f maxOffset = Vector3f(maxX, maxY, maxZ) - buffer->center;
	buffer->scale = 1.0f / maxOffset.maxCoeff();
}

// Loop Subdivision, update the positions of existing vertices
Vector3f sdLoopVertex(Vertex* v) {
    Vector3f vec(0, 0, 0);
	int k = v->countFaces();
	// Pre-calculate beta to improve efficiency
	float beta = 1.0f / k * (5.0f / 8.0f - pow(3.0f / 8.0f + 1.0f / 4.0f * cos(2.0f * PI / k), 2));
	float q = 1 - k * beta;

	vec += v->p * q;
	W_edge *e0 = v->edge->end == v ? v->edge->leftW_edge() : v->edge;
	W_edge *edge = e0;
	do {
		if (edge->end == v) {
			vec += edge->start->p * beta;
			edge = edge->right_next;
		} else {
			vec += edge->end->p * beta;
			edge = edge->left_next;
		}
	} while (edge != e0);

	return vec;
}

// Loop Subdivision, new vertices at edges
Vector3f sdLoopEdge(W_edge* w_edge) {
	Vector3f vec(0, 0, 0);
	vec += 3.0f / 8.0f * w_edge->start->p;
	vec += 3.0f / 8.0f * w_edge->end->p;
	vec += 1.0f / 8.0f * w_edge->right_next->end->p;
	vec += 1.0f / 8.0f * w_edge->left_next->end->p;

	return vec;
}

// Butterfly Subdivision, both sides regular
Vector3f sdBtflEdgeBothRegular(W_edge* w_edge) {
	Vector3f vec(0, 0, 0);
	vec += 1.0f / 2.0f * w_edge->start->p;
	vec += 1.0f / 2.0f * w_edge->end->p;
	vec += 1.0f / 8.0f * w_edge->right_next->end->p;
	vec += 1.0f / 8.0f * w_edge->left_next->end->p;
	vec += -1.0f / 16.0f * w_edge->right_next->left_next->end->p;
	vec += -1.0f / 16.0f * w_edge->right_prev->left_next->end->p;
	vec += -1.0f / 16.0f * w_edge->left_next->left_next->end->p;
	vec += -1.0f / 16.0f * w_edge->left_prev->left_next->end->p;
	
	return vec;
}

float getS(int j, int k) {
	if (j >= k) throw "Invalid j";
	if (k <= 4) throw "Invalid k";

	return 1.0f / k * (0.25f + cos(2.0f * j * PI / k) + 0.5f * cos(4.0f * j * PI / k));
}

// Butterfly Subdivision, start vertex regular
Vector3f sdBtflEdgeStartRegular(W_edge* w_edge, int k) {
	Vector3f vec(0, 0, 0);

	if (k == 3) {
		// v0
		vec += 5.0f / 12.0f * w_edge->start->p;
		// v1
		vec += -1.0f / 12.0f * w_edge->right_next->end->p;
		// v2
		vec += -1.0f / 12.0f * w_edge->right_next->left_next->end->p;
		// vq
		vec += 3.0f / 4.0f * w_edge->end->p;
	} else if (k == 4) {
		// v0
		vec += 3.0f / 8.0f * w_edge->start->p;
		// v1 is 0
		// v2
		vec += -1.0f / 8.0f * w_edge->right_next->left_next->end->p;
		// v3 is 0
		// vq
		vec += 3.0f / 4.0f * w_edge->end->p;
	} else {
		float s = 0;
		float sum_s = 0;

		s = getS(0, k);
		vec += s * w_edge->start->p;
		sum_s += s;

		W_edge* btflEdge = w_edge->right_next;
		for (int j = 1; j < k; j++) {
			s = getS(j, k);
			vec += s * btflEdge->end->p;
			sum_s += s;

			btflEdge = btflEdge->left_next;
		}

		float q = 1 - sum_s;
		vec += q * w_edge->end->p;
	}
	return vec;
}

// Butterfly Subdivision, new vertices at edges
Vector3f sdBtflEdge(W_edge* w_edge) {
	Vector3f vec;
    int startCount = w_edge->start->countFaces();
	int endCount = w_edge->end->countFaces();

	if (startCount == 6 && endCount == 6) {
		vec = sdBtflEdgeBothRegular(w_edge);
	} else if (startCount == 6) {
		vec = sdBtflEdgeStartRegular(w_edge, endCount);
	} else if (endCount == 6) {
		vec = sdBtflEdgeStartRegular(w_edge->leftW_edge(), startCount);
	} else {
		vec = sdBtflEdgeStartRegular(w_edge, endCount) / 2.0f + sdBtflEdgeStartRegular(w_edge->leftW_edge(), startCount) / 2.0f;
	}

	return vec;
}

// Get mesh vertex positions
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

// Get normals for flat shading
MatrixXf WingedEdge::getNormals(MatrixXf* positions) {
	MatrixXf normals = MatrixXf(3, mFaces * 9);

	for (int i = 0; i < mFaces; i++) {
		Vector3f e1 = positions->col(i * 3 + 1) - positions->col(i * 3);
		Vector3f e2 = positions->col(i * 3 + 2) - positions->col(i * 3 + 1);
		Vector3f normal = (e1.cross(e2)).normalized();

		normals.col(i * 3) = normals.col(i * 3 + 1) = normals.col(i * 3 + 2) = normal;
		normals.col(mFaces * 3 + i * 6) = normals.col(mFaces * 3 + i * 6 + 1) = normals.col(mFaces * 3 + i * 6 + 2)
		= normals.col(mFaces * 3 + i * 6 + 3) = normals.col(mFaces * 3 + i * 6 + 4) = normals.col(mFaces * 3 + i * 6 + 5)
		= normal;
	}
	return normals;
}

// Get normals for smooth shading
MatrixXf WingedEdge::getSmoothNormals(MatrixXf* normals) {
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
		smoothNormals.col(i) << normals->col(i);
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

// Write mesh to an obj file
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

// Loop subdivision
ObjBuffer WingedEdge::sdLoop() {
	ObjBuffer sd;
	sd.nVertices = nVertices + lW_edges / 2;
	sd.mFaces = mFaces * 4;
	sd.center = center;
	sd.scale = scale;
	sd.vertices = new Vector3f[sd.nVertices];
	sd.faces = new Vector3i[sd.mFaces];

	int vi = 0;
	for (; vi < nVertices; vi++) {
		sd.vertices[vi] = sdLoopVertex(vertices + vi);
	}

	for (int j = 0; j < lW_edges; j++) {
		if (w_edges[j].edgeVertex == NULL) {
			sd.vertices[vi] = sdLoopEdge(w_edges + j);
			w_edges[j].edgeVertex = sd.vertices + vi;
			w_edges[j].leftW_edge()->edgeVertex = sd.vertices + vi;
			vi++;
		}
	}

	int v1, v2, v3, v4, v5, v6;
	for (int k = 0; k < mFaces; k++) {
		v1 = faces[k].edge->start - vertices + 1;
		v2 = faces[k].edge->end - vertices + 1;
		v3 = faces[k].edge->right_next->end - vertices + 1;
		v4 = faces[k].edge->edgeVertex - sd.vertices + 1;
		v5 = faces[k].edge->right_next->edgeVertex - sd.vertices + 1;
		v6 = faces[k].edge->right_prev->edgeVertex - sd.vertices + 1;

		sd.faces[k * 4] << v1, v6, v4;
		sd.faces[k * 4 + 1] << v3, v5, v6;
		sd.faces[k * 4 + 2] << v2, v4, v5;
		sd.faces[k * 4 + 3] << v4, v6, v5;
	}

	return sd;
}

// Butterfly subdivision
ObjBuffer WingedEdge::sdBtfl() {
	ObjBuffer sd;
	sd.nVertices = nVertices + lW_edges / 2;
	sd.mFaces = mFaces * 4;
	sd.center = center;
	sd.scale = scale;
	sd.vertices = new Vector3f[sd.nVertices];
	sd.faces = new Vector3i[sd.mFaces];

	int vi = 0;
	for (; vi < nVertices; vi++) {
		sd.vertices[vi] = vertices[vi].p;
	}

	for (int j = 0; j < lW_edges; j++) {
		if (w_edges[j].edgeVertex == NULL) {
			sd.vertices[vi] = sdBtflEdge(w_edges + j);
			w_edges[j].edgeVertex = sd.vertices + vi;
			w_edges[j].leftW_edge()->edgeVertex = sd.vertices + vi;
			vi++;
		}
	}

	int v1, v2, v3, v4, v5, v6;
	for (int k = 0; k < mFaces; k++) {
		v1 = faces[k].edge->start - vertices + 1;
		v2 = faces[k].edge->end - vertices + 1;
		v3 = faces[k].edge->right_next->end - vertices + 1;
		v4 = faces[k].edge->edgeVertex - sd.vertices + 1;
		v5 = faces[k].edge->right_next->edgeVertex - sd.vertices + 1;
		v6 = faces[k].edge->right_prev->edgeVertex - sd.vertices + 1;

		sd.faces[k * 4] << v1, v6, v4;
		sd.faces[k * 4 + 1] << v3, v5, v6;
		sd.faces[k * 4 + 2] << v2, v4, v5;
		sd.faces[k * 4 + 3] << v4, v6, v5;
	}

	return sd;
}

// Read Obj file
ObjBuffer WingedEdge::readObj(string filename) {
	string line;
	int vn = 0, fm = 0;

	// Read the file and get the numbers of vertices and faces.
	ifstream fin(filename);
	while (getline(fin, line)) {
		if (line.length() > 1) {
			if (line[0] == 'v' && line[1] == ' ') {
				vn++;
			}
			else if (line[0] == 'f' && line[1] == ' ') {
				fm++;
			}
		}
	}

	ObjBuffer buffer;
	buffer.nVertices = vn;
	buffer.mFaces = fm;
	buffer.vertices = new Vector3f[buffer.nVertices];
	buffer.faces = new Vector3i[buffer.mFaces];

	// read the file again and initialize vertices, faces, and w_edges.
	ifstream fin1(filename);
	int vi = 0, fi = 0;
	float x, y, z;
	int v1, v2, v3;

	while (getline(fin1, line)) {
		if (line.length() > 0) {
			if (line[0] == 'v' && line[1] == ' ') {
				string str = line.substr(2, line.size() - 1);
				istringstream iss(str);
				iss >> x >> y >> z;
				buffer.vertices[vi] = Vector3f(x, y, z);
				vi++;
			} else if (line[0] == 'f' && line[1] == ' ') {
				string str = line.substr(2, line.size() - 1);
				istringstream iss(str);
				iss >> v1 >> v2 >> v3;
				buffer.faces[fi] = Vector3i(v1, v2, v3);
				fi++;
			}
		}
	}

	// Set Center and Scale
	setCenterAndScale(&buffer);

	return buffer;
}

// Read obj buffer
void WingedEdge::readObjBuffer(ObjBuffer buffer) {
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
// Fill in left parameters (left_prev, left_next, and left) of W_edge
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

// Get vertex normals for smooth shading
Vector3f WingedEdge::getVertexSN(Vertex* v, MatrixXf* normals) {
    Vector3f vec(0, 0, 0);
	vector<Face*> vfs = v->getFaces();
	int facei = -1;

	for (auto f : vfs) {
		facei = f - faces;
		vec += normals->col(facei * 3);
	}

    return vec.normalized();
}