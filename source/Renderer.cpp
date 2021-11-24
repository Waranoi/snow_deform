#include "Renderer.h"
#include "Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

static Shader mvp_shader;
static Shader snow_shader;
static Shader terrain_shader;

void Renderer::Init()
{
	mvp_shader = Shader("../resources/shaders/simple_mvp_vs", "../resources/shaders/simple_mvp_fs", nullptr, nullptr);
	snow_shader = Shader("../resources/shaders/simple_mvp_vs", "../resources/shaders/snow_deform_fs", nullptr, nullptr);
	terrain_shader = Shader("../resources/shaders/simple_tess_vs", "../resources/shaders/simple_tess_fs", "../resources/shaders/simple_tess_tc", "../resources/shaders/simple_tess_te");
}

void Renderer::Draw_simple(Object *objects, int count, Camera camera)
{
	unsigned int program = mvp_shader.Get_program();
	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_FALSE, camera.Get_projection());
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, camera.Get_view());
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, Matrix4f());
	glUniform3f(glGetUniformLocation(program, "light_pos"), 0.0f, 30.0f, 0.0f);
	glUniform3f(glGetUniformLocation(program, "light_col"), 1.0f, 1.0f, 1.0f);
	glUniform3fv(glGetUniformLocation(program, "view_pos"), 1, camera.Get_view().getTranslation());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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
		
		// If color map is 0, use_texture is set to false
		glUniform1i(glGetUniformLocation(program, "use_texture"), object.color_map);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, object.color_map);

		glDrawElements(GL_TRIANGLES, object.points, GL_UNSIGNED_INT, nullptr);

		glUniform3fv(glGetUniformLocation(program, "object_col"), 1, Vector3f(1, 1, 1));
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, Matrix4f());
	}
}

void Renderer::Draw_snow(Object *objects, int count, Camera camera, unsigned int snow_map, unsigned int deform_map)
{
	unsigned int program = snow_shader.Get_program();
	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_FALSE, camera.Get_projection());
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, camera.Get_view());
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, Matrix4f());
	glUniform3f(glGetUniformLocation(program, "light_pos"), 0.0f, 30.0f, 0.0f);
	glUniform3f(glGetUniformLocation(program, "light_col"), 1.0f, 1.0f, 1.0f);
	glUniform3fv(glGetUniformLocation(program, "view_pos"), 1, camera.Get_view().getTranslation());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, snow_map);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, deform_map);

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

void Renderer::Draw_terrain(Object *objects, int count, Camera camera, unsigned int color_map, unsigned int height_map, float radius)
{
	unsigned int program = terrain_shader.Get_program();
	glUseProgram(program);
	glUniform1f(glGetUniformLocation(program, "radius"), radius);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_FALSE, camera.Get_projection());
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, camera.Get_view());
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, Matrix4f());

	// Will there be a noticeable seam between the edges of a texture if I use GL_CLAMP_TO_EDGE instead
	// of GL_NEAREST if the edges of the plane we're mapping to are wrapped around to meet each other?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, color_map);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, height_map);

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