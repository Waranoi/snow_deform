#include "Camera.h"

Camera::Camera()
{
	float far = 100.0f;
	float near = 0.1f;
	projection[10] = -(far/(far-near));
	projection[11] = -1;
	projection[14] = -((far*near)/(far-near));
	rot_x = 0;
}

void Camera::Move(Vector3f move)
{
	view = Matrix4f::createTranslation(-move.x, -move.y, -move.z) * view;
}

void Camera::Rotate(Vector3f rotate)
{
	rot_x += rotate.x;
	if (rot_x > 90.0f)
		rot_x = 90.0f;
	else if (rot_x < -90.0f)
		rot_x = -90.0f;

	view = Matrix4f::createRotationAroundAxis(0, rotate.y, 0) * view;
}

Matrix4f Camera::Get_projection()
{
	return projection;
}

Matrix4f Camera::Get_view()
{
	return Matrix4f::createRotationAroundAxis(rot_x, 0, 0) * view;
}