#pragma once
#include "vmath.h"
#include "Object.h"
#include "Camera.h"
#include "Shader.h"

namespace Shape_renderer
{
	void Init();
	void Draw(Object *objects, int count, Camera camera);
	Object Create_box(Vector3f center, Vector3f half_dims, Vector3f color, unsigned int color_map = 0);
	Object Create_plane(Vector3f center, Vector2f half_dims, Vector3f color, unsigned int color_map = 0);

	extern Shader shader;
};