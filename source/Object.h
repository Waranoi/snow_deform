#pragma once
#include "vmath.h"

struct Object
{
    void Move(Vector3f move);
    void Rotate(Vector3f rotate);

    void Bind_array();
    void Bind_vertices(float* vertices, int length);
    void Bind_indices(int* indices, int length);

    unsigned int vao, vbo, ebo, color_map;
    int points;
    Vector3f color;
    Matrix4f model;
};

namespace Create_object
{
    Object Box(Vector3f center, Vector3f half_dims, Vector3f color, unsigned int color_map = 0);
    Object Plane(Vector3f center, Vector2f half_dims, Vector3f color, unsigned int color_map = 0);
    Object Patch(Vector2f min_pos, Vector2f max_pos, Vector2f min_uv, Vector2f max_uv, Vector3f color);
};