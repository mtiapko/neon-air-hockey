#ifndef __GRAPHIC_RESOURCE_H__
#define __GRAPHIC_RESOURCE_H__

#include <cstddef>
#include "GL/glew.h"

namespace hockey
{

class GraphicResource
{
private:
	GLuint m_res;

public:
	GraphicResource() : m_res(0) {}
	GraphicResource(GLuint res) : m_res(res) {}

	operator GLuint() const { return m_res; }
};

inline bool operator!=(const GraphicResource& l, std::nullptr_t)
{
	return (GLuint)l != 0;
}

template<typename D>
class GraphicDeleter
{
private:
	D m_deleter;

public:
	using pointer = GraphicResource;

	GraphicDeleter(D deleter) : m_deleter(deleter) {}

	void operator()(const GraphicResource& res) const
	{
		m_deleter((GLuint)res);
	}
};

}

#endif  //  !__GRAPHIC_RESOURCE_H__
