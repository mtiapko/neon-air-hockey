#include <sys/stat.h>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include "Shader.h"
#include "Log.h"

namespace hockey
{

/* static */ std::string Shader::load(std::string_view path)
{
	std::unique_ptr<std::FILE, decltype(std::fclose)*> file {
		std::fopen(path.data(), "rb"), std::fclose
	};

	if (!file) {
		LOG_ERROR("Failed to open file '" << path << "': " << std::strerror(errno));
		return {};
	}

	struct stat file_stat;
	fstat(fileno(file.get()), &file_stat);
	if (file_stat.st_size <= 0) {
		LOG_ERROR("File '" << path << "' is empty");
		return {};
	}

	std::string src(file_stat.st_size, '\0');
	if (fread(src.data(), sizeof(decltype(src)::value_type), file_stat.st_size, file.get())
			!= (size_t)file_stat.st_size) {
		LOG_ERROR("Failed to read file '" << path << '\'');
		return {};
	}

	return src;
}

/* static */ auto Shader::compile(std::string_view path, GLenum type)
{
	std::unique_ptr<GLuint, ResourceDeleter<decltype(glDeleteShader)>> shader {
		0, glDeleteShader
	};

	auto src = Shader::load(path);
	if (src.empty())
		return shader;

	const char* src_lines[] = { src.data() };
	shader.reset(glCreateShader(type));
	glShaderSource(shader.get(), 1, src_lines, nullptr);
	glCompileShader(shader.get());

	GLint val;
	glGetShaderiv(shader.get(), GL_COMPILE_STATUS, &val);
	if (val != GL_TRUE) {
		glGetShaderiv(shader.get(), GL_INFO_LOG_LENGTH, &val);

		char buff[val];
		glGetShaderInfoLog(shader.get(), val, nullptr, buff);
		LOG_ERROR("OpenGL failed to compile shader '" << path << "':\n\t"
			<< std::string_view(buff, val));
		shader.reset(0);
	}

	return shader;
}

/* static */ auto Shader::link(GLuint vert, GLuint frag)
{
	std::unique_ptr<GLuint, ResourceDeleter<decltype(glDeleteShader)>> prog {
		glCreateProgram(), glDeleteProgram
	};

	glAttachShader(prog.get(), vert);
	glAttachShader(prog.get(), frag);
	glLinkProgram(prog.get());

	glDetachShader(prog.get(), vert);
	glDetachShader(prog.get(), frag);

	GLint val;
	glGetProgramiv(prog.get(), GL_LINK_STATUS, &val);
	if (val != GL_TRUE) {
		glGetProgramiv(prog.get(), GL_INFO_LOG_LENGTH, &val);

		char buff[val];
		glGetProgramInfoLog(prog.get(), val, nullptr, buff);
		LOG_ERROR("OpenGL failed to link program:\n\t" << std::string_view(buff, val));
		prog.reset(0);
	}

	return prog;
}

void Shader::enable() const
{
	glUseProgram(m_prog.get());
}

Status Shader::create(const std::string& vert_path, const std::string& frag_path)
{
	auto vert = Shader::compile(vert_path, GL_VERTEX_SHADER);
	if (!vert)
		return Status::ERROR;

	auto frag = Shader::compile(frag_path, GL_FRAGMENT_SHADER);
	if (!frag)
		return Status::ERROR;

	m_prog = Shader::link(vert.get(), frag.get());
	if (!m_prog)
		return Status::ERROR;

	return Status::OK;
}

}
