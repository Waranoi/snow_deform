#pragma once
#include "vmath.h"
#include "Object.h"
#include "Camera.h"

namespace Renderer
{
    void Init();
    void Draw_simple(Object *objects, int count, Camera camera);
    void Draw_snow(Object *objects, int count, Camera camera, unsigned int snow_map = 0, unsigned int deform_map = 0);
    void Draw_snow_depression(Object &object, Camera camera, unsigned int snow_deform_map, int snow_deform_width, int snow_deform_height);
    void Draw_terrain(Object *objects, int count, Camera camera, unsigned int color_map, unsigned int height_map, float radius = 0);
};