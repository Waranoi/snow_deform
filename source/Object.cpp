#include "Object.h"

void Object::Draw(GLuint program)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);
	glUniform3f(glGetUniformLocation(program, "in_color"), color.r, color.g, color.b);
	glDrawElements(GL_TRIANGLES, points, GL_UNSIGNED_INT, nullptr);
}

std::shared_ptr<Object> Object::Box(Vector3f center, Vector3f half_dims, Vector3f color)
{
	std::shared_ptr<Object> cube(new Object());

	Vector3f min = center - half_dims;
	Vector3f max = center + half_dims;

	float vertices[] = 
	{
		max.x,	min.y,	min.z,
		max.x,	min.y,	max.z,
		min.x,	min.y,	max.z,
		min.x,	min.y,	min.z,

		max.x,	max.y,	min.z,
		max.x,	max.y,	max.z,
		min.x,	max.y,	max.z,
		min.x,	max.y,	min.z,
	};

	int indices[] = 
	{
		4, 0, 3,
		4, 3, 7,
		2, 6, 7,
		2, 7, 3,
		1, 5, 2,
		5, 6, 2,
		0, 4, 1,
		4, 5, 1,
		4, 7, 5,
		7, 6, 5,
		0, 1, 2,
		0, 2, 3 
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
		max.x,	center.y,	max.z,
		min.x,	center.y,	max.z,
		min.x,	center.y,	min.z,
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