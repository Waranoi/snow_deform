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
#include "Shader.h"

GLFWwindow* window;
Vector3f movement;
bool rotate = false;
Vector3f rotation;
Vector3f prev_rot;
bool render_depth = false;

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

    Shader phong("../resources/shaders/phong_vs", "../resources/shaders/phong_fs");
    Shader simple_tex("../resources/shaders/simple_depth_vs", "../resources/shaders/simple_depth_fs");
	unsigned int program = phong.Get_program();
	
	// Set clear color to gray
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	glEnable(GL_DEPTH_TEST);

	glUseProgram(program);

	// Objects
	std::shared_ptr<Object> depth_w	= Object::Plane(Vector3f(0, 0, 0), Vector2f(1.0f, 1.0f), Vector3f(0, 1.0f, 0));
	std::shared_ptr<Object> plane 	= Object::Plane(Vector3f(), Vector2f(10.0f, 10.0f), Vector3f(0.3f, 0.5f, 0.7f));
	std::shared_ptr<Object> plateau = Object::Box(Vector3f(-5.0f, 1.0f, -5.0f), Vector3f(3.0f, 1.0f, 3.0f), Vector3f(1.0f, 0, 0));
	std::shared_ptr<Object> cube 	= Object::Box(Vector3f(-3.0f, 1.5f, 5.0f), Vector3f(1.5f, 1.5f, 1.5f), Vector3f(0, 1.0f, 0));
	std::shared_ptr<Object> tower 	= Object::Box(Vector3f(6.0f, 3.0f, -1.0f), Vector3f(1.0f, 3.0f, 1.0f), Vector3f(0, 0, 1.0f));
	plane->Rotate(Vector3f(90.0f, 0, 0));
	
	// Camera
	Camera camera;
	camera.Move(Vector3f(0, 5, 12));
	camera.Rotate(Vector3f(-30, 0, 0));

	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_FALSE, camera.Get_projection());
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, camera.Get_view());
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, Matrix4f());
	glUniform3f(glGetUniformLocation(program, "light_pos"), 0.0f, 30.0f, 0.0f);
	glUniform3f(glGetUniformLocation(program, "light_col"), 1.0f, 1.0f, 1.0f);

	// Framebuffer objects
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 800, 800, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);

	int fb_res = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(fb_res != GL_FRAMEBUFFER_COMPLETE)
		printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete! %d\n", fb_res);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	while (!glfwWindowShouldClose(window))
	{
		auto begin = std::chrono::steady_clock::now();

        // Game loop
        {
			program = phong.Get_program();
			glUseProgram(program);

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
			if (render_depth)
				glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			else
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			plane->Draw(program);
			plateau->Draw(program);
			cube->Draw(program);
			tower->Draw(program);

			if (render_depth)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				program = simple_tex.Get_program();
				glUseProgram(program);
				depth_w->Draw(program);
			}
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

	if(key == GLFW_KEY_R && action == GLFW_PRESS)
        render_depth = !render_depth;

	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
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