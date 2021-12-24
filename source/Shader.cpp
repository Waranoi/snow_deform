#include "Shader.h"
#include <iostream>
#include <fstream>

Shader::Shader()
{
    
}

Shader::Shader(const char* path_vs, const char* path_fs, const char* path_tc, const char* path_te, const char* path_gs)
{
    // Create a program object
    program = glCreateProgram();

    // Setup vertex shader
    std::string vs_str = Load_shader(path_vs);
    const char* vs_char = vs_str.c_str();
    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vs_char, NULL);
    glCompileShader(vs);
    Shader_error_log("[VERTEX SHADER COMPILE ERROR]:", vs);
    glAttachShader(program, vs);

    // Setup fragment shader
    if (path_fs)
    {
        std::string fs_str = Load_shader(path_fs);
        const char* fs_char = fs_str.c_str();
        fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fs_char, NULL);
        glCompileShader(fs);
        Shader_error_log("[FRAGMENT SHADER COMPILE ERROR]:", fs);
        glAttachShader(program, fs);
    }

    // Setup tessellation control shader
    if (path_tc)
    {
        std::string tc_str = Load_shader(path_tc);
        const char* tc_char = tc_str.c_str();
        tc = glCreateShader(GL_TESS_CONTROL_SHADER);
        glShaderSource(tc, 1, &tc_char, NULL);
        glCompileShader(tc);
        Shader_error_log("[TESSELATION CONTROL SHADER COMPILE ERROR]:", tc);
        glAttachShader(program, tc);
    }

    // Setup tessellation evaluation shader
    if (path_te)
    {
        std::string te_str = Load_shader(path_te);
        const char* te_char = te_str.c_str();
        te = glCreateShader(GL_TESS_EVALUATION_SHADER);
        glShaderSource(te, 1, &te_char, NULL);
        glCompileShader(te);
        Shader_error_log("[TESSELATION EVALUATION SHADER COMPILE ERROR]:", te);
        glAttachShader(program, te);
    }

    // Setup geometry shader
    if (path_gs)
    {
        std::string gs_str = Load_shader(path_gs);
        const char* gs_char = gs_str.c_str();
        gs = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(gs, 1, &gs_char, NULL);
        glCompileShader(gs);
        Shader_error_log("[GEOMETRY SHADER COMPILE ERROR]:", gs);
        glAttachShader(program, gs);
    }

    // Link program object
    glLinkProgram(program);
    Program_error_log("[PROGRAM LINK ERROR]:", program);
}

unsigned int Shader::Get_program()
{
    return program;
}

std::string Shader::Load_shader(const char* path)
{
    std::string content;
    std::ifstream file_stream(path, std::ios::in);

    if (!file_stream.is_open()) {
        std::cerr << "Could not read file " << path << ". File does not exist." << std::endl;
        return nullptr;
    }

    std::string line = "";
    while (!file_stream.eof()) {
        std::getline(file_stream, line);
        content.append(line + "\n");
    }

    file_stream.close();
    return content;
}

void Shader::Shader_error_log(const char* msg, GLuint shader)
{
    GLint log_length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length > 1)
    {
        GLchar* buf = new GLchar[log_length];
        glGetShaderInfoLog(shader, log_length, NULL, buf);
        printf("%s %s %d\n", msg, buf, log_length);
        delete[] buf;
    }
}

void Shader::Program_error_log(const char* msg, GLuint program)
{
    GLint log_length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length > 1)
    {
        GLchar* buf = new GLchar[log_length];
        glGetProgramInfoLog(program, log_length, NULL, buf);
        printf("%s %s \n", msg, buf);
        delete[] buf;
    }
}