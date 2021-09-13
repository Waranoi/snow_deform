#include "Object.h"

void Object::Draw(GLuint program)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));
	glUniform3f(glGetUniformLocation(program, "object_col"), color.r, color.g, color.b);
	glDrawElements(GL_TRIANGLES, points, GL_UNSIGNED_INT, nullptr);
}

std::shared_ptr<Object> Object::Box(Vector3f center, Vector3f half_dims, Vector3f color)
{
	std::shared_ptr<Object> cube(new Object());

	Vector3f min = center - half_dims;
	Vector3f max = center + half_dims;

	float vertices[] = 
	{
		// Front
		max.x, max.y, max.z,
		0.0f, 0.0f, 1.0f,
		min.x, max.y, max.z,
		0.0f, 0.0f, 1.0f,
		min.x, min.y, max.z,
		0.0f, 0.0f, 1.0f,
		max.x, min.y, max.z,
		0.0f, 0.0f, 1.0f,

		// Back
		min.x, min.y, min.z,
		0.0f, 0.0f, -1.0f,
		min.x, max.y, min.z,
		0.0f, 0.0f, -1.0f,
		max.x, max.y, min.z,
		0.0f, 0.0f, -1.0f,
		max.x, min.y, min.z,
		0.0f, 0.0f, -1.0f,

		// Right
		max.x, max.y, max.z,
		1.0f, 0.0f, 0.0f,
		max.x, min.y, max.z,
		1.0f, 0.0f, 0.0f,
		max.x, min.y, min.z,
		1.0f, 0.0f, 0.0f,
		max.x, max.y, min.z,
		1.0f, 0.0f, 0.0f,

		// Left
		min.x, min.y, min.z,
		-1.0f, 0.0f, 0.0f,
		min.x, min.y, max.z,
		-1.0f, 0.0f, 0.0f,
		min.x, max.y, max.z,
		-1.0f, 0.0f, 0.0f,
		min.x, max.y, min.z,
		-1.0f, 0.0f, 0.0f,

		// Up
		max.x, max.y, max.z,
		0.0f, 1.0f, 0.0f,
		max.x, max.y, min.z,
		0.0f, 1.0f, 0.0f,
		min.x, max.y, min.z,
		0.0f, 1.0f, 0.0f,
		min.x, max.y, max.z,
		0.0f, 1.0f, 0.0f,

		// Down
		min.x, min.y, min.z,
		0.0f, -1.0f, 0.0f,
		max.x, min.y, min.z,
		0.0f, -1.0f, 0.0f,
		max.x, min.y, max.z,
		0.0f, -1.0f, 0.0f,
		min.x, min.y, max.z,
		0.0f, -1.0f, 0.0f
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

	cube->Bind_array();
	cube->Bind_vertices(vertices, sizeof(vertices) / sizeof(float));
	cube->Bind_indices(indices, sizeof(indices) / sizeof(int));
	cube->color = color;
	return cube;
}

std::shared_ptr<Object> Object::Plane(Vector3f center, Vector3f half_dims, Vector3f color)
{
	std::shared_ptr<Object> plane(new Object());

	Vector3f min = center - half_dims;
	Vector3f max = center + half_dims;

	float vertices[] = 
	{
		max.x,	center.y,	min.z,
		0.0f, 	1.0f, 		0.0f,
		max.x,	center.y,	max.z,
		0.0f, 	1.0f, 		0.0f,
		min.x,	center.y,	max.z,
		0.0f, 	1.0f, 		0.0f,
		min.x,	center.y,	min.z,
		0.0f, 	1.0f, 		0.0f,
	};

	int indices[] = 
	{
		0, 1, 2,
		0, 2, 3 
	};

	plane->Bind_array();
	plane->Bind_vertices(vertices, sizeof(vertices) / sizeof(float));
	plane->Bind_indices(indices, sizeof(indices) / sizeof(int));
	plane->color = color;
	return plane;
}

Object::Object()
{

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