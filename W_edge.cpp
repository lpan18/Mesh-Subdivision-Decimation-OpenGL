#include <cmath>
#include <Eigen/Geometry>
#include <iostream>

#include "W_edge.h"

using namespace std;

Vector3f W_edge::getOptimalV() {
	Matrix4f q = start->getQ() + end->getQ();
	Matrix4f drv;
	drv << q(0, 0), q(0, 1), q(0, 2), q(0, 3),
	       q(0, 1), q(1, 1), q(1, 2), q(1, 3),
		   q(0, 2), q(1, 2), q(2, 2), q(2, 3),
		   0, 0, 0, 1;
	Matrix4f drv_inv = drv.inverse();

	if (drv_inv.hasNaN()) {
		// Temp code, return mid-point
		// cout << start->p * 0.5 + end->p * 0.5 << endl << endl;
		return start->p * 0.5 + end->p * 0.5;
	} else {
		Vector4f v = drv_inv * Vector4f(0, 0, 0, 1);
		return v.head(3);
	}
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

vector<W_edge*> Vertex::getW_edges() {
	vector<W_edge*> vec;
	W_edge *e0 = edge->end == this ? edge->leftW_edge() : edge;
	W_edge *e = e0;
    
	do {
		vec.push_back(e);
		if (e->end == this) {
			e = e->right_next;
		} else {
			e = e->left_next;
		}
	} while (e != e0);

	if (vec.size() == 0) throw "No edge at vertex v";

	return vec;
}

Matrix4f Vertex::getQ() {
	Matrix4f q = Matrix4f::Zero();
	vector<Face*> faces = getFaces();
	for (auto f : faces) {
		q += f->getK_p();
	}
	return q;
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
	Vector3f e1 = vertices[2]->p - vertices[0]->p;
	Vector3f e2 = vertices[1]->p - vertices[0]->p;
	Vector3f normal = (e1.cross(e2)).normalized();

	return normal;
}

Vector4f Face::getP() {
	vector<Vertex*> vertices = getVertices();
	Vector3f e1 = vertices[2]->p - vertices[0]->p;
	Vector3f e2 = vertices[1]->p - vertices[0]->p;
	Vector3f normal = (e1.cross(e2)).normalized();
	float d = (Vector3f::Zero() - vertices[0]->p).dot(normal);
	Vector4f p = Vector4f(normal.x(), normal.y(), normal.z(), d);
	return p;
}

Matrix4f Face::getK_p() {
	Vector4f p = getP();
	return p * p.transpose();
}