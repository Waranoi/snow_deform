#pragma once
#include "vmath.h"

struct Object
{
	void Rotate(Vector3f rotate);

	void Bind_array();
	void Bind_vertices(float* vertices, int length);
	void Bind_indices(int* indices, int length);

	unsigned int vao, vbo, ebo, color_map;
	int points;
	Vector3f color;
	Matrix4f model;
};