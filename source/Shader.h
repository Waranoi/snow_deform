#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>

class Shader
{
public:
    Shader();
    Shader(const char* path_vs, const char* path_fs, const char* path_tc = nullptr, const char* path_te = nullptr, const char* path_gs = nullptr);
    unsigned int Get_program();

private:
    std::string Load_shader(const char* path);
    void Shader_error_log(const char* msg, GLuint shader);
    void Program_error_log(const char* msg, GLuint program);

private:
    unsigned int vs, fs, tc, te, gs, program;

};