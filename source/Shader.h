#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>

class Shader
{
public:
	Shader(const char* path_vs, const char* path_fs);
	unsigned int Get_program();

private:
	std::string Load_shader(const char* path);
	void Shader_error_log(const char* msg, GLuint shader);
	void Program_error_log(const char* msg, GLuint program);

private:
	unsigned int vs, fs, program;

};