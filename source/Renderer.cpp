#include "Renderer.h"
#include "Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

static Shader mvp_shader;
static Shader snow_shader;
static Shader depress_snow_shader;
static Shader gaussian_blur_shader;
static Shader terrain_shader;
static Shader terrain_shader_debug;

void Renderer::Init()
{
    mvp_shader = Shader("../resources/shaders/simple_mvp_vs", "../resources/shaders/simple_mvp_fs");
    snow_shader = Shader("../resources/shaders/simple_mvp_vs", "../resources/shaders/snow_deform_fs");
    depress_snow_shader = Shader("../resources/shaders/simple_mvp_vs", "../resources/shaders/snow_depress_fs");
    gaussian_blur_shader = Shader("../resources/shaders/simple_mvp_vs", "../resources/shaders/gaussian_blur_fs");
    terrain_shader = Shader("../resources/shaders/simple_tess_vs", "../resources/shaders/simple_tess_fs", "../resources/shaders/simple_tess_tc", "../resources/shaders/simple_tess_te");
    terrain_shader_debug = Shader("../resources/shaders/simple_tess_vs", "../resources/shaders/simple_tess_fs", "../resources/shaders/simple_tess_tc", "../resources/shaders/simple_tess_te", "../resources/shaders/debug_normals_gs");
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

void Renderer::Draw_snow_depression(Object &object, Camera camera, unsigned int snow_deform_map, int snow_deform_width, int snow_deform_height)
{
    unsigned int program = depress_snow_shader.Get_program();
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
    glBindTexture(GL_TEXTURE_2D, snow_deform_map);

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
    glUniform1i(glGetUniformLocation(program, "width"), snow_deform_width);
    glUniform1i(glGetUniformLocation(program, "height"), snow_deform_height);

    glDrawElements(GL_TRIANGLES, object.points, GL_UNSIGNED_INT, nullptr);

    glUniform3fv(glGetUniformLocation(program, "object_col"), 1, Vector3f(1, 1, 1));
    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, Matrix4f());
    
}

void Renderer::Draw_gaussian_blur(Object &object, Camera camera, unsigned int source_texture, Vector2i direction, int width, int height)
{
    unsigned int program = gaussian_blur_shader.Get_program();
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
    glBindTexture(GL_TEXTURE_2D, source_texture);

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
    glUniform2iv(glGetUniformLocation(program, "dir"), 1, direction);
    glUniform1i(glGetUniformLocation(program, "width"), width);
    glUniform1i(glGetUniformLocation(program, "height"), height);

    glDrawElements(GL_TRIANGLES, object.points, GL_UNSIGNED_INT, nullptr);

    glUniform3fv(glGetUniformLocation(program, "object_col"), 1, Vector3f(1, 1, 1));
    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, Matrix4f());
    
}

void Renderer::Draw_terrain(Object *objects, int count, Camera camera, unsigned int color_map, unsigned int height_map, int width, int height, float radius, bool debug)
{
    unsigned int program;
    if (debug)
        program = terrain_shader_debug.Get_program();
    else
        program = terrain_shader.Get_program();
    
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
        glUniform1i(glGetUniformLocation(program, "width"), width);
        glUniform1i(glGetUniformLocation(program, "height"), height);

        glDrawElements(GL_PATCHES, 4, GL_UNSIGNED_INT, 0);

        glUniform3fv(glGetUniformLocation(program, "object_col"), 1, Vector3f(1, 1, 1));
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, Matrix4f());
    }
}