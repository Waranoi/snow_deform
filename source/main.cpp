#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>

void Shader_error_log(const char* msg, GLuint shader);
void Program_error_log(const char* msg, GLuint program);
std::string Get_shader(const char* path);

int main()
{
	// glfw: initialize and configure
    // ------------------------------
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(800, 600, "Snow Deform", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Setup vertex shader
    std::string vs_str = Get_shader("../resources/shaders/simple_vs");
    const char* vs_char = vs_str.c_str();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_char, NULL);
	glCompileShader(vs);
	Shader_error_log("[VERTEX SHADER COMPILE ERROR]:", vs);

	// Setup fragment shader
    std::string fs_str = Get_shader("../resources/shaders/simple_fs");
    const char* fs_char = fs_str.c_str();
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_char, NULL);
	glCompileShader(fs);
	Shader_error_log("[FRAGMENT SHADER COMPILE ERROR]:", fs);

	// Create a program object
	GLuint program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	Program_error_log("[PROGRAM LINK ERROR]:", program);
	
	// Set clear color to gray
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	float vertices[] =
	{
		-0.25f,	-0.25f,	0,
		0,		0.25f,	0,
		0.25f,	-0.25f,	0,
	};
	int indices[] = { 0, 1, 2 };
	unsigned int vao, vbo, ebo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glUseProgram(program);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);

	while (!glfwWindowShouldClose(window))
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        	glfwSetWindowShouldClose(window, true);

        glClear(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Shader_error_log(const char* msg, GLuint shader)
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

void Program_error_log(const char* msg, GLuint program)
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

std::string Get_shader(const char* path)
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