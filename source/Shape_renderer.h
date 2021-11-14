#pragma once
#include "vmath.h"
#include "Object.h"
#include "Camera.h"
#include "Shader.h"

namespace Shape_renderer
{
	void Init();
	void Draw(Object *objects, int count, Camera camera);
	void Draw_snow_deform(Object *objects, int count, Camera camera, unsigned int snow_map = 0, unsigned int deform_map = 0);
	Object Create_box(Vector3f center, Vector3f half_dims, Vector3f color, unsigned int color_map = 0);
	Object Create_plane(Vector3f center, Vector2f half_dims, Vector3f color, unsigned int color_map = 0);

	extern Shader mvp_shader;
	extern Shader snow_shader;
};