#include <cmath>
#include <Eigen/Geometry>
#include <iostream>

#include "W_edge.h"

using namespace std;

// v0, v1, and v2 are ordered clock-wise
Vector3f calculateNormal(Vector3f v0, Vector3f v1, Vector3f v2) {
	Vector3f e1 = v2 - v0;
	Vector3f e2 = v1 - v0;
	Vector3f normal = (e1.cross(e2)).normalized();
	return normal;
}

float limitRange(float f, float low, float high) {
	return f < low ? low : f > high ? high : f;
}

void W_edge::PairLeftW_edge(W_edge *leftW_edge) {
	leftW_edge->left = right;
	leftW_edge->left_prev = right_prev;
	leftW_edge->left_next = right_next;

	left = leftW_edge->right;
	left_prev = leftW_edge->right_prev;
	left_next = leftW_edge->right_next;
}

Matrix4f W_edge::getQ() {
	return start->q + end->q;
}

Vector4f W_edge::getTargetV() {
	Matrix4f q = getQ();
	Matrix4f drv;
	drv << q(0, 0), q(0, 1), q(0, 2), q(0, 3),
	       q(0, 1), q(1, 1), q(1, 2), q(1, 3),
		   q(0, 2), q(1, 2), q(2, 2), q(2, 3),
		   0, 0, 0, 1;
	Matrix4f drv_inv = drv.inverse();

	if (drv_inv.hasNaN()) {
		// Temp code, return mid-point
		Vector3f mp = start->p * 0.5 + end->p * 0.5;
		cout << "drv is not invertible" << mp << endl;
		return Vector4f(mp.x(), mp.y(), mp.z(), 1);
	} else {
		return drv_inv * Vector4f(0, 0, 0, 1);
	}
}

void W_edge::toNull() {
	start = NULL;
	end = NULL;
}

bool W_edge::isNull() {
	if (start == NULL && end == NULL) {
		return true;
	} else if (start != NULL && end != NULL) {
		return false;
	} else {
		throw "Invalid W_edge state. start and end must be both null, or not null.";
	}
}

float W_edge::getDiffAngleFaces(Vertex* v, Vector3f newP) {
	Vector3f ln0 = left->getNormal();
	Vector3f rn0 = right->getNormal();
	float cosa0 = ln0.dot(rn0);
	cosa0 = limitRange(cosa0, -1.0f, 1.0f);
	float a0 = acos(cosa0);

	Vector3f ln1 = left->getNewNormal(v, newP);
	Vector3f rn1 = right->getNewNormal(v, newP);
	float cosa1 = ln1.dot(rn1);
	cosa1 = limitRange(cosa1, -1.0f, 1.0f);
	float a1 = acos(cosa1);

	return abs(a1 - a0);
}

bool W_edge::detectFoldOver() {
	Vector3f vt = getTargetV().head(3);
	Vertex* v1 = start;
	Vertex* v2 = end;
	Vertex* v3 = right_next->end;
	Vertex* v4 = left_next->end;

	vector<W_edge*> v1W_edges = v1->getAllW_edgesStart();
	for (auto e : v1W_edges) {
		if (e->end != v2 && e->end != v3 && e->end != v4) {
			if (e->getDiffAngleFaces(v1, vt) > PI / 2) {
				return true;
			}
		}
	}

	vector<W_edge*> v2W_edges = v2->getAllW_edgesStart();
	for (auto e : v2W_edges) {
		if (e->end != v1 && e->end != v3 && e->end != v4) {
			if (e->getDiffAngleFaces(v2, vt) > PI / 2) {
				return true;
			}
		}
	}

	return false;
}

vector<Face*> Vertex::getFaces() {
	vector<Face*> vec;
	W_edge *e0 = edge->end == this ? edge->leftW_edge() : edge;
	W_edge *e = e0;
    
	do {
		if (e->end == this) {
			vec.push_back(e->right);
			e = e->right_next;
		} else {
			vec.push_back(e->left);
			e = e->left_next;
		}
	} while (e != e0);

	if (vec.size() == 0) throw "No face at vertex v";

	return vec;
}

vector<W_edge*> Vertex::getAllW_edges() {
	vector<W_edge*> vec;
	W_edge *e0 = edge->end == this ? edge->leftW_edge() : edge;
	W_edge *e = e0;
    
	do {
		vec.push_back(e);
		vec.push_back(e->leftW_edge());
		if (e->end == this) {
			e = e->right_next;
		} else {
			e = e->left_next;
		}
	} while (e != e0);

	if (vec.size() == 0) throw "No edge at vertex v";

	return vec;
}

vector<W_edge*> Vertex::getAllW_edgesStart() {
	vector<W_edge*> vec;
	W_edge *e0 = edge->end == this ? edge->leftW_edge() : edge;
	W_edge *e = e0;
    
	do {
		vec.push_back(e);
		e = e->left_next;
	} while (e != e0);

	if (vec.size() == 0) throw "No edge at vertex v";

	return vec;
}

int Vertex::countJointNeighbourVertices(Vertex* v2) {
	int count = 0;
	for (auto v1e : this->getAllW_edges()) {
		for (auto v2e : v2->getAllW_edges()) {
			if (v1e->start == this && v2e->start == v2 && v1e->end == v2e->end) {
				count++;
			}
		}
	}
	return count;
}

void Vertex::setInitialQ() {
	q = Matrix4f::Zero();
	vector<Face*> faces = getFaces();
	for (auto f : faces) {
		q += f->getK_p();
	}
}

// Number of faces at Vertex v
int Vertex::countFaces() {
	return (int)getFaces().size();
}

vector<W_edge*> Face::getW_edges() {
	vector<W_edge*> vec;
	W_edge *e = edge;

	do {
		vec.push_back(e);
		e = e->right_next;
	} while (e != edge);

	if (vec.size() < 3) throw "Less than 3 W_edges on face f";

	return vec;
}

vector<Vertex*> Face::getVertices() {
	vector<Vertex*> vec;
	W_edge *e = edge;

	do {
		vec.push_back(e->start);
		e = e->right_next;
	} while (e != edge);

	if (vec.size() < 3) throw "Less than 3 Vertices on face f";

	return vec;
}

Vector3f Face::getNormal() {
	vector<Vertex*> vertices = getVertices();
	return calculateNormal(vertices[0]->p, vertices[1]->p, vertices[2]->p);
}

Vector3f Face::getNewNormal(Vertex* v, Vector3f newP) {
	vector<Vertex*> vertices = getVertices();
	Vector3f v0 = vertices[0] == v ? newP : vertices[0]->p;
	Vector3f v1 = vertices[1] == v ? newP : vertices[1]->p;
	Vector3f v2 = vertices[2] == v ? newP : vertices[2]->p;
	return calculateNormal(v0, v1, v2);
}

Matrix4f Face::getK_p() {
	vector<Vertex*> vertices = getVertices();
	Vector3f normal = calculateNormal(vertices[0]->p, vertices[1]->p, vertices[2]->p);
	float d = (Vector3f::Zero() - vertices[0]->p).dot(normal);
	Vector4f p = Vector4f(normal.x(), normal.y(), normal.z(), d);
	return p * p.transpose();
}