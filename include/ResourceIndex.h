#ifndef __GRAPHIC_RESOURCE_H__
#define __GRAPHIC_RESOURCE_H__

#include <cstddef>
#include <GL/glew.h>

namespace hockey
{

class ResourceIndex
{
private:
	GLuint m_res;

public:
	ResourceIndex() : m_res(0) {}
	ResourceIndex(GLuint res) : m_res(res) {}

	operator GLuint() const { return m_res; }
};

inline bool operator!=(const ResourceIndex& l, std::nullptr_t)
{
	return (GLuint)l != 0;
}

template<typename D>
class ResourceDeleter
{
private:
	D m_deleter;

public:
	using pointer = ResourceIndex;

	ResourceDeleter(D deleter) : m_deleter(deleter) {}

	void operator()(const ResourceIndex& res) const
	{
		m_deleter((GLuint)res);
	}
};

}

#endif  //  !__GRAPHIC_RESOURCE_H__
