#pragma once
#include "vmath.h"
#include "Object.h"
#include "Camera.h"

namespace Renderer
{
    void Init();
    void Draw_simple(Object *objects, int count, Camera camera);
    void Draw_phong(Object *objects, int count, Camera camera, Vector3f light_pos, Vector3f light_col);
    void Draw_snow(Object *objects, int count, Camera camera, unsigned int snow_map = 0, unsigned int deform_map = 0);
    void Draw_snow_depression(Object &object, Camera camera, unsigned int snow_deform_map, int snow_deform_width, int snow_deform_height);
    void Draw_gaussian_blur(Object &object, Camera camera, unsigned int source_texture, Vector2i direction, int texture_width, int texture_height);
    void Draw_terrain(Object *objects, int count, Camera camera, Vector3f light_pos, Vector3f light_col, unsigned int color_map, unsigned int height_map, float surface_width, float texture_width, float texture_height, float surface_height, float radius = 0, bool debug_vertex_normals = false, bool debug_fragment_normals = false);
};