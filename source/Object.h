#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "vmath.h"

class Object
{
public:
	void Draw(GLuint program);

	static std::shared_ptr<Object> Box(Vector3f center, Vector3f half_dims, Vector3f color);
	static std::shared_ptr<Object> Plane(Vector3f center, Vector3f half_dims, Vector3f color);

private:
	Object();
	void Bind_array();
	void Bind_vertices(float* vertices, int length);
	void Bind_indices(int* indices, int length);

private:
	unsigned int vao, vbo, ebo;
	int points;
	Vector3f color;
};