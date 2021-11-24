#include "Camera.h"

Camera::Camera()
{
    
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

Camera Camera::CreatePerspective(float far, float near)
{
    Camera camera;
    camera.projection[10] = -(far/(far-near));
    camera.projection[11] = -1;
    camera.projection[14] = -((far*near)/(far-near));
    return camera;
}

Camera Camera::CreateOrthographic(float far, float near, float right, float left, float top, float bot)
{
    Camera camera;
    camera.projection[0] = 2 / (right-left);
    camera.projection[5] = 2 / (top-bot);
    camera.projection[10] = -2 / (far-near);
    camera.projection[12] = -((right+left)/(right-left));
    camera.projection[13] = -((top+bot)/(top-bot));
    camera.projection[14] = -((far+near)/(far-near));
    camera.projection[15] = 1;
    return camera;
}