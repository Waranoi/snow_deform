#pragma once
#include "vmath.h"
#include "Object.h"
#include "Camera.h"
#include "Shader.h"

namespace Terrain_renderer
{
	void Init();
	void Draw(Object *objects, int count, Camera camera);
	Object Create_patch(Vector3f center, Vector2f half_dims, Vector3f color);
	
	extern Shader shader;
};