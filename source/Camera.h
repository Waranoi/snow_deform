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

	static Camera CreatePerspective();
	static Camera CreateOrthographic();

private:
	Matrix4f projection;
	Matrix4f view;
	float rot_x = 0;
};