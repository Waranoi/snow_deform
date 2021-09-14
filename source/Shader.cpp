#include "Shader.h"
#include <iostream>
#include <fstream>

Shader::Shader(const char* path_vs, const char* path_fs)
{
	// Setup vertex shader
    std::string vs_str = Load_shader(path_vs);
    const char* vs_char = vs_str.c_str();
	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_char, NULL);
	glCompileShader(vs);
	Shader_error_log("[VERTEX SHADER COMPILE ERROR]:", vs);

	// Setup fragment shader
    std::string fs_str = Load_shader(path_fs);
    const char* fs_char = fs_str.c_str();
	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_char, NULL);
	glCompileShader(fs);
	Shader_error_log("[FRAGMENT SHADER COMPILE ERROR]:", fs);

	// Create a program object
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
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