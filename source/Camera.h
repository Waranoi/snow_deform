#pragma once
#include "vmath.h"

class Camera
{
public:
	Camera();
	void Move(Vector3f move);
	void Rotate(Vector3f rotate);
	Matrix4f Get_projection();
	Matrix4f Get_view();

	static Camera CreatePerspective(float far = 100, float near = 0.1f);
	static Camera CreateOrthographic(float far = 1, float near = 0, float right = 1, float left = -1, float top = 1, float bot = -1);

private:
	Matrix4f projection;
	Matrix4f view;
	float rot_x = 0;
};