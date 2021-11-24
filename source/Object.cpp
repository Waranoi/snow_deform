#include "Object.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void Object::Move(Vector3f move)
{
	model = Matrix4f::createTranslation(move.x, move.y, move.z) * model;
}

void Object::Rotate(Vector3f rotate)
{
	model = Matrix4f::createRotationAroundAxis(rotate.x, rotate.y, rotate.z) * model;
}

void Object::Bind_array()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
}

void Object::Bind_vertices(float* vertices, int length)
{
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, length * sizeof(float), vertices, GL_STATIC_DRAW);
}

void Object::Bind_indices(int* indices, int length)
{
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, length * sizeof(int), indices, GL_STATIC_DRAW);
	points = length;
}

Object Create_object::Box(Vector3f center, Vector3f half_dims, Vector3f color, unsigned int color_map)
{
	Vector3f min = center - half_dims;
	Vector3f max = center + half_dims;

	float vertices[] = 
	{
		// Front
		max.x, max.y, max.z,
		0.0f, 0.0f, 1.0f,
		1.0f, 1.0f,
		min.x, max.y, max.z,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f,
		min.x, min.y, max.z,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f,
		max.x, min.y, max.z,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f,

		// Back
		min.x, min.y, min.z,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f,
		min.x, max.y, min.z,
		0.0f, 0.0f, -1.0f,
		1.0f, 1.0f,
		max.x, max.y, min.z,
		0.0f, 0.0f, -1.0f,
		0.0f, 1.0f,
		max.x, min.y, min.z,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f,

		// Right
		max.x, max.y, max.z,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		max.x, min.y, max.z,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f,
		max.x, min.y, min.z,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f,
		max.x, max.y, min.z,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f,

		// Left
		min.x, min.y, min.z,
		-1.0f, 0.0f, 0.0f,
		0.0f, 0.0f,
		min.x, min.y, max.z,
		-1.0f, 0.0f, 0.0f,
		1.0f, 0.0f,
		min.x, max.y, max.z,
		-1.0f, 0.0f, 0.0f,
		1.0f, 1.0f,
		min.x, max.y, min.z,
		-1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,

		// Up
		max.x, max.y, max.z,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f,
		max.x, max.y, min.z,
		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f,
		min.x, max.y, min.z,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f,
		min.x, max.y, max.z,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f,

		// Down
		min.x, min.y, min.z,
		0.0f, -1.0f, 0.0f,
		1.0f, 1.0f,
		max.x, min.y, min.z,
		0.0f, -1.0f, 0.0f,
		0.0f, 1.0f,
		max.x, min.y, max.z,
		0.0f, -1.0f, 0.0f,
		0.0f, 0.0f,
		min.x, min.y, max.z,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f
	};

	int indices[] = 
	{
		// Front
		0, 	1, 	2,
		0, 	2, 	3,

		// Back
		4, 	5, 	6,
		4, 	6, 	7,

		// Right
		8, 	9, 	10,
		8, 	10, 11,

		// Left
		12, 13, 14,
		12, 14, 15,

		// Up
		16, 17, 18,
		16, 18, 19,

		// Down
		20, 21, 22,
		20, 22, 23
	};

	Object cube;
	cube.Bind_array();
	cube.Bind_vertices(vertices, sizeof(vertices) / sizeof(float));
	cube.Bind_indices(indices, sizeof(indices) / sizeof(int));
	cube.color = color;
	cube.color_map = color_map;
	return cube;
}

Object Create_object::Plane(Vector3f center, Vector2f half_dims, Vector3f color, unsigned int color_map)
{
	Vector3f min = center - Vector3f(half_dims.x, half_dims.y, 0);
	Vector3f max = center + Vector3f(half_dims.x, half_dims.y, 0);

	float vertices[] = 
	{
		max.x,	max.y,	center.z,
		0.0f, 	0.0f, 	1.0f,
		1.0f, 	1.0f,
		min.x,	max.y,	center.z,
		0.0f, 	0.0f, 	1.0f,
		0.0f, 	1.0f,
		min.x,	min.y,	center.z,
		0.0f, 	0.0f, 	1.0f,
		0.0f, 	0.0f,
		max.x,	min.y,	center.z,
		0.0f, 	0.0f, 	1.0f,
		1.0f, 	0.0f
	};

	int indices[] = 
	{
		0, 1, 2,
		0, 2, 3 
	};

	Object plane;
	plane.Bind_array();
	plane.Bind_vertices(vertices, sizeof(vertices) / sizeof(float));
	plane.Bind_indices(indices, sizeof(indices) / sizeof(int));
	plane.color = color;
	plane.color_map = color_map;
	return plane;
}

Object Create_object::Patch(Vector2f min_pos, Vector2f max_pos, Vector2f min_uv, Vector2f max_uv, Vector3f color)
{
	float vertices[] = 
	{
		min_pos.x,	0.0f,		min_pos.y,
		0.0f, 		1.0f, 		0.0f,
		min_uv.x, 	min_uv.y,
		max_pos.x,	0.0f,		min_pos.y,
		0.0f, 		1.0f, 		0.0f,
		max_uv.x, 	min_uv.y,
		max_pos.x,	0.0f,		max_pos.y,
		0.0f, 		1.0f, 		0.0f,
		max_uv.x, 	max_uv.y,
		min_pos.x,	0.0f,		max_pos.y,
		0.0f, 		1.0f, 		0.0f,
		min_uv.x, 	max_uv.y
	};

	int indices[] = 
	{
		0, 1, 2, 3
	};

	Object patch;
	patch.Bind_array();
	patch.Bind_vertices(vertices, sizeof(vertices) / sizeof(float));
	patch.Bind_indices(indices, sizeof(indices) / sizeof(int));
	patch.color = color;
	return patch;
}