#include <memory>

class Shader
{
public:
	void Activate();

	static std::shared_ptr<Shader> Phong();

private:
	Shader();

private:
	unsigned int vs, fs, program;
};