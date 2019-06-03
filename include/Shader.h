#ifndef __SHADER_H__
#define __SHADER_H__

#include <memory>
#include <string>
#include "Status.h"
#include "GraphicResource.h"

namespace hockey
{

class Shader
{
private:
	std::unique_ptr<GLuint, GraphicDeleter<decltype(glDeleteProgram)>> m_prog { 0, glDeleteProgram };

	static std::string load(std::string_view path);
	static auto        compile(std::string_view path, GLenum type);
	static auto        link(GLuint vert, GLuint frag);

public:
	void enable() const;

	GLuint id() const { return m_prog.get(); }

	Status create(const std::string& vert_path, const std::string& frag_path);
};

}

#endif  //  !__SHADER_H__
