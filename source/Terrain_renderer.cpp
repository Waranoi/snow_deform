#include "Terrain_renderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

Shader Terrain_renderer::shader;

void Terrain_renderer::Init()
{
	Terrain_renderer::shader = Shader("../resources/shaders/simple_tess_vs", "../resources/shaders/simple_tess_fs", "../resources/shaders/simple_tess_tc", "../resources/shaders/simple_tess_te");
}

void Terrain_renderer::Draw(Object *objects, int count, Camera camera, unsigned int color_map)
{
	unsigned int program = Terrain_renderer::shader.Get_program();
	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_FALSE, camera.Get_projection());
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, camera.Get_view());
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, Matrix4f());
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glBindTexture(GL_TEXTURE_2D, color_map);

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
		glDrawElements(GL_PATCHES, 4, GL_UNSIGNED_INT, 0);
		glUniform3fv(glGetUniformLocation(program, "object_col"), 1, Vector3f(1, 1, 1));
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, Matrix4f());
	}
}

Object Terrain_renderer::Create_patch(Vector2f min_pos, Vector2f max_pos, Vector2f min_uv, Vector2f max_uv, Vector3f color)
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