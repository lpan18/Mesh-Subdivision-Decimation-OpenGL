#include "W_edge.h"
#include <Eigen/Geometry>

using namespace std;

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