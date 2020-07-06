#include "stdafx.h"
#include "Shader.h"
#include "ShaderFragment.h"
#include "RenderExceptions.h"
#include <EngineCore\Logger.h>
#include "RenderCache.h"

using namespace std;

namespace render
{
	void Shader::createShaderProgram(ShaderFragment* fragments[], int num)
	{
		//check parameters
		if (program != 0 || num == 0 || fragments == nullptr)
		{
			core::Logger::error("Program already created!");
			return;
		}

		program = glCreateProgram();

		//for each shader
		for (int i = 0; i < num; i++)
		{
			glAttachShader(program, fragments[i]->getShaderId());
		}

		//EXTRA LINK PARAMETERS HERE
		//if (GLEW_VERSION_4_1) {
		// glProgramParameteri( program, GL_PROGRAM_SEPARABLE, GL_TRUE );
		//}

		//now link
		glLinkProgram(program);

		GLint linked;
		glGetProgramiv(program, GL_LINK_STATUS, &linked);

		//errors?
		if (!linked)
		{
			logProgramErrors(program);
			glDeleteProgram(program);
			throw LoadingResourceException("Shader", "shader-linking");
			return;
		}
	}

	void Shader::initShaderAndFragments(int n, SCInfo sci[])
	{
		ShaderFragment** units = new ShaderFragment*[n];

		for (int i = 0; i < n; i++)        // Loop until all numbers are added
		{
			units[i] = new ShaderFragment(sci[i]);
		}

		this->createShaderProgram(units, n);

		for (int i = 0; i < n; i++)        // Loop until all numbers are added
		{
			delete units[i];
		}

		delete[] units;

		if (rc != nullptr)
		{
			rc->registerShader(this);
		}
	}

	//get and Log linking errors
	void Shader::logProgramErrors(GLuint program)
	{
		GLsizei len;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = new GLchar[len + 1];

		glGetProgramInfoLog(program, len, &len, log);

		core::Logger::error("Shader linking failed:\n" + string(log));

		delete[] log;
	}


	Shader::Shader(RenderCache *cache)
	{
		program = 0;
		sid = -1;
		rc = cache;
	}

	Shader::Shader(RenderCache *cache, int n, SCInfo sci[])
	{
		program = 0;
		sid = -1;
		rc = cache;

		initShaderAndFragments(n, sci);
	}


	Shader::~Shader()
	{
		if (program != 0) 
		{
			glDeleteProgram(program);
		}

		if (sid != -1 && rc != nullptr)
		{
			rc->removeShader(this);
		}
	}

}