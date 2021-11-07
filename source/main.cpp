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

unsigned int window_w = 800;
unsigned int window_h = 800;
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
    window = glfwCreateWindow(window_w, window_h, "Snow Deform", NULL, NULL);
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

	// Framebuffer objects
	unsigned int fbo_texture;
	glGenTextures(1, &fbo_texture);
	glBindTexture(GL_TEXTURE_2D, fbo_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, window_w, window_h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbo_texture, 0);

	int fb_res = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(fb_res != GL_FRAMEBUFFER_COMPLETE)
		printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete! %d\n", fb_res);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

	// Environmental objects
	Object snow = Terrain_renderer::Create_patch(Vector2f(-50, -50), Vector2f(50, 50), Vector2f(0, 0), Vector2f(1, 1), Vector3f(0, 0, 1));
	Object ground = Shape_renderer::Create_plane(Vector3f(), Vector2f(50, 50), Vector3f(0.30f, 0.16f, 0.16f));
	ground.Rotate(Vector3f(90.0f, 0.0f, 0.0f));

	// Snow deforming objects
	Object cubes[] = 
	{
		Shape_renderer::Create_box(Vector3f(20, 12, 25), Vector3f(10, 10, 10), Vector3f(1, 0, 0)),
		Shape_renderer::Create_box(Vector3f(-20, 10, 10), Vector3f(7, 7, 7), Vector3f(0, 1, 0)),
		Shape_renderer::Create_box(Vector3f(-5, 15, -25), Vector3f(15, 15, 15), Vector3f(0, 0, 1))
	};
	
	// Miscellaneous objects
	Object depth_display = Shape_renderer::Create_plane(Vector3f(), Vector2f(1, 1), Vector3f(0, 1, 0), fbo_texture);
	
	// Cameras
	Camera camera = Camera::CreatePerspective();
	camera.Move(Vector3f(0.0f, 5.0f, 75.0f));

	Camera fbo_camera = Camera::CreateOrthographic(10, 0, 50, -50, 50, -50);
	fbo_camera.Move(Vector3f(0.0f, -0.1f, 0.0f));
	fbo_camera.Rotate(Vector3f(90.0f, 0.0f, 0.0f));

	Camera depth_camera = Camera::CreateOrthographic();

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

			// Render fbo
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			Shape_renderer::Draw(cubes, 3, fbo_camera);

			// Draw
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			Terrain_renderer::Draw(&snow, 1, camera, color_map, height_map);
			Shape_renderer::Draw(&ground, 1, camera);
			Shape_renderer::Draw(cubes, 3, camera);

			//Shape_renderer::Draw(&depth_display, 1, depth_camera);
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