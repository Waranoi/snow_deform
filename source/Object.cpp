#include "Object.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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