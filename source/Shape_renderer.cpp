#include "Shape_renderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

Shader Shape_renderer::shader;

void Shape_renderer::Init()
{
	Shape_renderer::shader = Shader("../resources/shaders/simple_mvp_vs", "../resources/shaders/simple_mvp_fs", nullptr, nullptr);
}

void Shape_renderer::Draw(Object *objects, int count, Camera camera)
{
	unsigned int program = Shape_renderer::shader.Get_program();
	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_FALSE, camera.Get_projection());
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, camera.Get_view());
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, Matrix4f());
	glUniform3f(glGetUniformLocation(program, "light_pos"), 0.0f, 30.0f, 0.0f);
	glUniform3f(glGetUniformLocation(program, "light_col"), 1.0f, 1.0f, 1.0f);
	glUniform3fv(glGetUniformLocation(program, "view_pos"), 1, camera.Get_view().getTranslation());

	for (int i = 0; i < count; i++)
	{
		Object object = objects[i];
		glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.ebo);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, nullptr);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));
		glUniform3fv(glGetUniformLocation(program, "object_col"), 1, object.color);
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, object.model);
		glDrawElements(GL_TRIANGLES, object.points, GL_UNSIGNED_INT, nullptr);
		glUniform3fv(glGetUniformLocation(program, "object_col"), 1, Vector3f(1, 1, 1));
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, Matrix4f());
	}
}

Object Shape_renderer::Create_box(Vector3f center, Vector3f half_dims, Vector3f color)
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
	return cube;
}

Object Shape_renderer::Create_plane(Vector3f center, Vector2f half_dims, Vector3f color)
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
	return plane;
}