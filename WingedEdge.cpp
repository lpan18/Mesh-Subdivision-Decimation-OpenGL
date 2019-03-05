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

#include "WingedEdge.h"

using namespace std;

// Function for sorting w_edges
bool sortByStartVertexThenByEndVertex(const W_edge* edge1, const W_edge* edge2) {
	if (edge1->start < edge2->start)
		return true;
	else if (edge1->start > edge2->start)
		return false;
	else
		return edge1->end < edge2->end;
}

void ObjBuffer::setCenterAndScale() {
	float maxX, maxY, maxZ;
	float minX, minY, minZ;
	maxX = maxY = maxZ = -MAXVALUE;
	minX = minY = minZ = MAXVALUE;

	for (int i = 0; i < nVertices; i++) {
		maxX = vertices[i].x() > maxX ? vertices[i].x() : maxX;
		maxY = vertices[i].y() > maxY ? vertices[i].y() : maxY;
		maxZ = vertices[i].z() > maxZ ? vertices[i].z() : maxZ;
		minX = vertices[i].x() < minX ? vertices[i].x() : minX;
		minY = vertices[i].y() < minY ? vertices[i].y() : minY;
		minZ = vertices[i].z() < minZ ? vertices[i].z() : minZ;
	}

	center = Vector3f(maxX / 2.0f + minX / 2.0f, maxY / 2.0f + minY / 2.0f, maxZ / 2.0f + minZ / 2.0f);
	Vector3f maxOffset = Vector3f(maxX, maxY, maxZ) - center;
	scale = 1.0f / maxOffset.maxCoeff();
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

ObjBuffer WingedEdge::mcd(int k, int countCollapse) {
	if (k <= 0) throw "k must be great than 0";
	if (countCollapse >= lW_edges / 2) throw "countCollapse must be smaller than the number of edges";

	// Initialize Q for all vertices
	for (int i = 0; i < nVertices; i++) {
		vertices[i].setInitialQ();
	}

	// Initialize valid W_edges
	vector<W_edge*> validW_edges;
	validW_edges.reserve(lW_edges);
	for (int i = 0; i < lW_edges; i++) {
		validW_edges.push_back(w_edges + i);
	}
    
	// Initialize random number generator
	srand(12345678);

	// Main loop for mcd
	for (int i = 0; i < countCollapse; i++) {
		do {
			bool success = mcdOneStep(k, validW_edges);
			if (success) break;
		}
		while (true);
	}

	// Write the decimated mesh to an ObjBuffer
	int dvCount = 0;
	int dfCount = 0;
	int* dvNumber = new int[nVertices];
	for (int i = 0; i < nVertices; i++) {
		if (vertices[i].edge != NULL) {
			dvCount++;
			dvNumber[i] = dvCount;
		} else {
			dvNumber[i] = -1;
		}
	}
	for (int i = 0; i < mFaces; i++) {
		if (faces[i].edge != NULL) {
			dfCount++;
		}
	}

	ObjBuffer buffer;
	buffer.center = center;
	buffer.scale = scale;
	buffer.nVertices = dvCount;
	buffer.mFaces = dfCount;
	buffer.vertices = new Vector3f[buffer.nVertices];
	buffer.faces = new Vector3i[buffer.mFaces];

	int vi = 0;
	for (int i = 0; i < nVertices; i++) {
		if (vertices[i].edge != NULL) {
			buffer.vertices[vi] = vertices[i].p;
			vi++;
		}
	}

	cout << "Vertices " <<  dvCount << endl;

	int fi = 0;
	for (int i = 0; i < mFaces; i++) {
		if (faces[i].edge != NULL) {
			int v1 = dvNumber[faces[i].edge->start - vertices];
			int v2 = dvNumber[faces[i].edge->end - vertices];
			int v3 = dvNumber[faces[i].edge->right_next->end -vertices];
			buffer.faces[fi] = Vector3i(v3, v2, v1);
			fi++;
		}
	}

	cout << "Faces " << dfCount << endl;

	return buffer;
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
	buffer.setCenterAndScale();

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
						w_edgeP[j]->PairLeftW_edge(w_edgeP[k]->right_prev);
					}
				}
			}
			bi = i + 1;
		}
	}
	delete w_edgeP;
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

bool WingedEdge::mcdOneStep(int k, vector<W_edge*>& validW_edges) {
	// First, randomly select k elements from validW_edges and move them to the start of validW_edges.
	// Empty W_edges are removed when detected.
	for (int i = 0; i < k; i++) {
		do {
			int r = rand() % (validW_edges.size() - i) + i;
			W_edge* re = validW_edges[r];
			if (re->isNull()) {
				validW_edges.erase(validW_edges.begin() + r);
				continue;
			} else {
				if (i == r) {
					// DO NOTHING
				} else {
					// Switch the rth and ith elements in validW_edges
					W_edge* temp = validW_edges[r];
					validW_edges[r] = validW_edges[i];
					validW_edges[i] = temp;
				}
				break;
			}
		} while (true);
	}

	// Next, calculate the cost for each randomly selected edge and find the W_edge
	// with minimun collapse cost
	vector<float> errors;
	errors.reserve(k);
	for (int i = 0; i < k; i++) {
		nanogui::Vector4f vt = validW_edges[i]->getTargetV();
		float e = vt.transpose() * validW_edges[i]->getQ() * vt;
		errors.push_back(e);
	}
	int minEI = std::min_element(errors.begin(),errors.end()) - errors.begin();
	W_edge* me = validW_edges[minEI];

	// There must be exactly two joint neighbour vertices
	if (me->start->countJointNeighbourVertices(me->end) != 2) {
		return false;
	} else {
		mcdCollapse(me);
		return true;
	}
}

void WingedEdge::mcdCollapse(W_edge* w_edge) {
	Vector4f vt = w_edge->getTargetV();
	Vertex* v1 = w_edge->start;
	Vertex* v2 = w_edge->end;
	Vertex* v3 = w_edge->right_next->end;
	Vertex* v4 = w_edge->left_next->end;
	Face* f1 = w_edge->right;
	Face* f2 = w_edge->left;
	W_edge* e1 = w_edge->right_prev->leftW_edge();
	W_edge* e2 = w_edge->right_next->leftW_edge();
	W_edge* e3 = w_edge->left_next->leftW_edge();
	W_edge* e4 = w_edge->left_prev->leftW_edge();
	
	// for all v1Edges, update v1 to v2
	for (auto e : v1->getAllW_edges()) {
		if (e->start == v1)
			e->start = v2;
		else if (e->end == v1)
		    e->end = v2;
		else
		    throw "not a W_edge of vertex v1";
	}

	// Re-pairing e1 with e2, and e3 with e4
	e1->PairLeftW_edge(e2);
	e3->PairLeftW_edge(e4);

	// Set W_edges on f1 and f2 to null
	for (auto e : f1->getW_edges()) {
		e->toNull();
	}
	for (auto e : f2->getW_edges()) {
		e->toNull();
	}

	// Udpate v1 and v2
	v2->p = vt.head(3);
	v2->edge = e2;
	v2->q = v2->q + v1->q;

	v1->p = Vector3f::Zero();
	v1->edge = NULL;

	v3->edge = e1;
	v4->edge = e4;

	// Update f1 and f2
	f1->edge = NULL;
	f2->edge = NULL;
}