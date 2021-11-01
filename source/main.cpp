#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <algorithm>
#include "vmath.h"
#include "Shape_renderer.h"
#include "Terrain_renderer.h"
#include "Camera.h"

GLFWwindow* window;
Vector3f movement;
bool rotate = false;
Vector3f rotation;
Vector3f prev_rot;
bool render_depth = false;

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

    // Init renderers
    Terrain_renderer::Init();
	Shape_renderer::Init();
	
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	// Textures
	int color_w, color_h, color_chan;
	unsigned char *color_data = stbi_load("../resources/textures/snow.jpg", &color_w, &color_h, &color_chan, 0);

	unsigned int color_map;
	glGenTextures(1, &color_map);
	glBindTexture(GL_TEXTURE_2D, color_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, color_w, color_h, 0, GL_RGB, GL_UNSIGNED_BYTE, color_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(color_data);

	int height_w = 256;
	int height_h = 256;
	int height_tot = height_w * height_h;
	float height_data[height_tot];
	for (int i = 0; i < height_tot; i++)
	{
		height_data[i] = 10;
	}

	unsigned int height_map;
	glGenTextures(1, &height_map);
	glBindTexture(GL_TEXTURE_2D, height_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, height_w, height_h, 0, GL_RED, GL_FLOAT, height_data);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Objects
	Object snow = Terrain_renderer::Create_patch(Vector2f(-50, -50), Vector2f(50, 50), Vector2f(0, 0), Vector2f(1, 1), Vector3f(0, 0, 1));
	Object ground = Shape_renderer::Create_plane(Vector3f(), Vector2f(50, 50), Vector3f(0, 1, 0));
	ground.Rotate(Vector3f(-90.0f, 0.0f, 0.0f));
	
	// Camera
	Camera camera;
	camera.Move(Vector3f(0.0f, 20.0f, 60.0f));
	camera.Rotate(Vector3f(-30.0f, 0.0f, 0.0f));

	while (!glfwWindowShouldClose(window))
	{
		auto begin = std::chrono::steady_clock::now();

        // Game loop
        {
        	// Directional movement
			camera.Move(movement * 1.0f);
			if (movement.y > 0.0f)
				movement.y = std::max(movement.y - 1.0f, 0.0f);
			else if (movement.y < 0.0f)
				movement.y = std::min(movement.y + 1.0f, 0.0f);

			// Rotational movement
			camera.Rotate(prev_rot - rotation);
			prev_rot = rotation;

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// Draw
			Terrain_renderer::Draw(&snow, 1, camera, color_map, height_map);
			Shape_renderer::Draw(&ground, 1, camera);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();

		auto end = std::chrono::steady_clock::now();
		auto elapsed = end - begin;
		auto target = std::chrono::milliseconds(1000 / 30);
		std::this_thread::sleep_for(target);
	}
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