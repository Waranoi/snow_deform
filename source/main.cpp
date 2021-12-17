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
#include "Object.h"
#include "Renderer.h"
#include "Camera.h"

unsigned int window_w = 800;
unsigned int window_h = 800;
GLFWwindow* window;

Vector3f movement;
bool rotate = false;
Vector3f rotation;
Vector3f prev_rot;

int render_depth = 0;
unsigned int polygon_mode = GL_FILL;

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
    Renderer::Init();
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    // Depth texture and framebuffer
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

    // Color map
    int color_w, color_h, color_chan;
    unsigned char *color_data = stbi_load("../resources/textures/snow.jpg", &color_w, &color_h, &color_chan, 0);

    unsigned int color_map;
    glGenTextures(1, &color_map);
    glBindTexture(GL_TEXTURE_2D, color_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, color_w, color_h, 0, GL_RGB, GL_UNSIGNED_BYTE, color_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(color_data);

    // Height maps and framebuffer
    int height_w = window_w;
    int height_h = window_h;
    int height_tot = height_w * height_h * 3;
    unsigned char height_data[height_tot];
    for (int i = 0; i < height_tot; i += 3)
    {
        height_data[i+0] = 255;
        height_data[i+1] = 0;
        height_data[i+2] = 0;
    }

    int height_id = 0;
    unsigned int height_map[2];
    glGenTextures(2, height_map);
    for (int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, height_map[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, height_w, height_h, 0, GL_RGB, GL_UNSIGNED_BYTE, height_data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    unsigned int height_fbo;
    glGenFramebuffers(1, &height_fbo);

    // Depress snow texture and framebuffer. Can I not just use the height_fbo for this? Should I make a general use fbo instead? Can I not just use the default fbo instead?
    unsigned int depress_texture;
    glGenTextures(1, &depress_texture);
    glBindTexture(GL_TEXTURE_2D, depress_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_w, window_h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned int depress_fbo;
    glGenFramebuffers(1, &depress_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, depress_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depress_texture, 0);

    // Environmental objects
    Object snow[] =
    {
        Create_object::Patch(Vector2f(-50, -50), Vector2f(0, 0), Vector2f(0, 0), Vector2f(0.5f, 0.5f), Vector3f(0, 0, 1)),
        Create_object::Patch(Vector2f(0, -50), Vector2f(50, 0), Vector2f(0.5f, 0), Vector2f(1, 0.5f), Vector3f(0, 0, 1)),
        Create_object::Patch(Vector2f(0, 0), Vector2f(50, 50), Vector2f(0.5f, 0.5f), Vector2f(1, 1), Vector3f(0, 0, 1)),
        Create_object::Patch(Vector2f(-50, 0), Vector2f(0, 50), Vector2f(0, 0.5f), Vector2f(0.5f, 1), Vector3f(0, 0, 1))
    };

    Object ground = Create_object::Plane(Vector3f(), Vector2f(50, 50), Vector3f(0.30f, 0.16f, 0.16f));
    ground.Rotate(Vector3f(90.0f, 0.0f, 0.0f));

    // Snow deforming objects
    Object cubes[] = 
    {
        Create_object::Box(Vector3f(20, 12, 25), Vector3f(10, 10, 10), Vector3f(1, 0, 0)),
        Create_object::Box(Vector3f(-20, 10, 10), Vector3f(7, 7, 7), Vector3f(0, 1, 0)),
        Create_object::Box(Vector3f(-5, 15, -25), Vector3f(14, 14, 14), Vector3f(0, 0, 1))
    };
    
    // Miscellaneous objects
    Object simple_square = Create_object::Plane(Vector3f(), Vector2f(1, 1), Vector3f(0, 1, 0));
    Object depth_display = Create_object::Plane(Vector3f(), Vector2f(1, 1), Vector3f(0, 1, 0), fbo_texture);
    Object depress_display = Create_object::Plane(Vector3f(), Vector2f(1, 1), Vector3f(0, 1, 0), depress_texture);
    
    // Cameras
    Camera camera = Camera::CreatePerspective();
    camera.Move(Vector3f(0.0f, 50.0f, 75.0f));
    camera.Rotate(Vector3f(-30.0f, 0.0f, 0.0f));

    Camera fbo_camera = Camera::CreateOrthographic(10, 0, 50, -50, 50, -50);
    fbo_camera.Move(Vector3f(0.0f, 0.0f, 0.0f));
    fbo_camera.Rotate(Vector3f(90.0f, 0.0f, 0.0f));

    Camera height_camera = Camera::CreateOrthographic();
    Camera ortho_camera = Camera::CreateOrthographic();
    Camera depth_camera = Camera::CreateOrthographic();

    while (!glfwWindowShouldClose(window))
    {
        auto begin = std::chrono::steady_clock::now();

        // Game loop
        {
            // Directional movement
            //camera.Move(movement * 1.0f);
            cubes[2].Move(movement * 1.0f);
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
            Renderer::Draw_simple(cubes, 3, fbo_camera);

            // Render snow height map
            int height_target = height_id;
            height_id = (height_id + 1) % 2;
            int height_source = height_id;
            glBindFramebuffer(GL_FRAMEBUFFER, height_fbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, height_map[height_target], 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Renderer::Draw_snow(&simple_square, 1, height_camera, height_map[height_source], fbo_texture);

            // Depress snow
            glBindFramebuffer(GL_FRAMEBUFFER, depress_fbo);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Renderer::Draw_snow_depression(simple_square, ortho_camera, height_map[height_target], window_w, window_h);

            // Blur snow

            // Draw
            glPolygonMode( GL_FRONT_AND_BACK, polygon_mode );
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Renderer::Draw_terrain(snow, 4, camera, color_map, depress_texture);
            Renderer::Draw_simple(&ground, 1, camera);
            Renderer::Draw_simple(cubes, 3, camera);
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

            if (render_depth == 1)
                Renderer::Draw_simple(&depth_display, 1, depth_camera);
            else if (render_depth == 2)
                Renderer::Draw_simple(&depress_display, 1, depth_camera);
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

    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        if (++render_depth > 2)
            render_depth = 0;
    }

    if (key == GLFW_KEY_T && action == GLFW_PRESS)
    {
        if (polygon_mode == GL_FILL)
            polygon_mode = GL_LINE;
        else
            polygon_mode = GL_FILL;
    }

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