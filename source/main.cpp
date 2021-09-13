#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <algorithm>
#include "vmath.h"
#include "Object.h"
#include "Camera.h"

GLFWwindow* window;
Vector3f movement;
bool rotate = false;
Vector3f rotation;
Vector3f prev_rot;

void Shader_error_log(const char* msg, GLuint shader);
void Program_error_log(const char* msg, GLuint program);
std::string Get_shader(const char* path);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

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
    window = glfwCreateWindow(800, 800, "Snow Deform", NULL, NULL);
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

    // User inputs
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Setup vertex shader
    std::string vs_str = Get_shader("../resources/shaders/phong_vs");
    const char* vs_char = vs_str.c_str();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_char, NULL);
	glCompileShader(vs);
	Shader_error_log("[VERTEX SHADER COMPILE ERROR]:", vs);

	// Setup fragment shader
    std::string fs_str = Get_shader("../resources/shaders/phong_fs");
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

	glEnable(GL_DEPTH_TEST);

	glUseProgram(program);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);

	// Objects
	std::shared_ptr<Object> plane 	= Object::Plane(Vector3f(), Vector3f(10.0f, 0, 10.0f), Vector3f(0.3f, 0.5f, 0.7f));
	std::shared_ptr<Object> plateau = Object::Box(Vector3f(-5.0f, 1.0f, -5.0f), Vector3f(3.0f, 1.0f, 3.0f), Vector3f(1.0f, 0, 0));
	std::shared_ptr<Object> cube 	= Object::Box(Vector3f(-3.0f, 1.5f, 5.0f), Vector3f(1.5f, 1.5f, 1.5f), Vector3f(0, 1.0f, 0));
	std::shared_ptr<Object> tower 	= Object::Box(Vector3f(6.0f, 3.0f, -1.0f), Vector3f(1.0f, 3.0f, 1.0f), Vector3f(0, 0, 1.0f));
	
	// Camera
	Camera camera;
	camera.Move(Vector3f(0, 5, 12));
	camera.Rotate(Vector3f(-30, 0, 0));

	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_FALSE, camera.Get_projection());
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, camera.Get_view());
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, Matrix4f());
	glUniform3f(glGetUniformLocation(program, "light_pos"), 0.0f, 30.0f, 0.0f);
	glUniform3f(glGetUniformLocation(program, "light_col"), 1.0f, 1.0f, 1.0f);

	while (!glfwWindowShouldClose(window))
	{
		auto begin = std::chrono::steady_clock::now();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Game loop
        {
        	// Directional movement
			camera.Move(movement * 0.1f);
			if (movement.y > 0.0f)
				movement.y = std::max(movement.y - 1.0f, 0.0f);
			else if (movement.y < 0.0f)
				movement.y = std::min(movement.y + 1.0f, 0.0f);

			// Rotational movement
			camera.Rotate(prev_rot - rotation);
			prev_rot = rotation;

			// Acknowledge movement
			glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, camera.Get_view());
			Vector3f view_pos = camera.Get_view().getTranslation();
			glUniform3f(glGetUniformLocation(program, "view_pos"), view_pos.x, view_pos.y, view_pos.z);

			// Draw
			plane->Draw(program);
			plateau->Draw(program);
			cube->Draw(program);
			tower->Draw(program);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();

		auto end = std::chrono::steady_clock::now();
		auto elapsed = end - begin;
		auto target = std::chrono::milliseconds(1000 / 30);
		std::this_thread::sleep_for(target);
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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// I need to do controls like this so it works with teamviewer...
	static int right;
	static int left;
	static int up;
	static int down;
	static int forward;
	static int backward;

	if (key == GLFW_KEY_RIGHT)
	{
		if (action == GLFW_PRESS)
			right = 1;
		else if (action == GLFW_RELEASE)
			right = 0;
	}
	if (key == GLFW_KEY_LEFT)
	{
		if (action == GLFW_PRESS)
			left = -1;
		else if (action == GLFW_RELEASE)
			left = 0;
	}

	if (key == GLFW_KEY_UP)
	{
		if (action == GLFW_PRESS)
			forward = -1;
		else if (action == GLFW_RELEASE)
			forward = 0;
	}
	if (key == GLFW_KEY_DOWN)
	{
		if (action == GLFW_PRESS)
			backward = 1;
		else if (action == GLFW_RELEASE)
			backward = 0;
	}

	movement.x = right + left;
	movement.z = forward + backward;

	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		rotate = true;
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		rotation = Vector3f(ypos, xpos, 0);
		prev_rot = rotation;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		rotate = false;
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (rotate)
		rotation = Vector3f(ypos, xpos, 0);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	movement.y += yoffset;
}