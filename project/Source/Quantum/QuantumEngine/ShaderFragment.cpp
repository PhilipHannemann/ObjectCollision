#include "stdafx.h"
#include "ShaderFragment.h"
#include <EngineCore/ResourceManager.h>
#include "RenderExceptions.h"

namespace render
{

	ShaderFragment::ShaderFragment(std::string src, GLenum type)
	{
		id = glCreateShader(type);

		//read file
		const GLchar* source = readShader(src);

		//fatal fail
		if (source == nullptr)
		{
			glDeleteShader(id);
			core::Logger::error("Can't load Shader from: " + src);
			throw LoadingResourceException("ShaderFragment", src);
			return;
		}

		//now compile
		glShaderSource(id, 1, &source, NULL);
		delete[] source;

		glCompileShader(id);

		GLint compiled;
		glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);

		//errors?
		if (!compiled)
		{
			logShaderErrors(id, src);
			glDeleteShader(id);
			throw LoadingResourceException("ShaderFragment", src);
			return;
		}

		//take it
		//glAttachShader(program, id);
	}


	ShaderFragment::~ShaderFragment()
	{
		glDeleteShader(id);
	}

	const GLchar* ShaderFragment::readShader(std::string src)
	{
		FILE* infile;
		fopen_s(&infile, src.c_str(), "rb");

		if (!infile)
			return nullptr;

		fseek(infile, 0, SEEK_END);
		int len = ftell(infile);
		fseek(infile, 0, SEEK_SET);

		GLchar* source = new GLchar[len + 1];

		fread(source, 1, len, infile);
		fclose(infile);

		source[len] = 0;

		return const_cast<const GLchar*>(source);
	}

	//get and Log complie errors
	void ShaderFragment::logShaderErrors(GLuint id, string filename)
	{
		GLsizei len;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = new GLchar[len + 1];

		glGetShaderInfoLog(id, len, &len, log);

		core::Logger::error("Shader compilation failed for " + filename + ":\n" + string(log));

		delete[] log;
	}

}