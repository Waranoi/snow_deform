#pragma once
#include "vmath.h"
#include "Object.h"
#include "Camera.h"
#include "Shader.h"

namespace Terrain_renderer
{
	void Init();
	void Draw(Object *objects, int count, Camera camera, unsigned int color_map, unsigned int height_map, float radius = 0);
	Object Create_patch(Vector2f min_pos, Vector2f max_pos, Vector2f min_uv, Vector2f max_uv, Vector3f color);
	
	extern Shader shader;
};