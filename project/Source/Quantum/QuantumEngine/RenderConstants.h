#pragma once
#include <opengl_include.h>

namespace render 
{

	struct DrawArraysIndirectCommand 
	{
		GLuint  count;
		GLuint  instanceCount;
		GLuint  first;
		GLuint  baseInstance;
	};

	namespace constant
	{
		struct pair 
		{
			const GLuint index;
			const char* name;

			pair(const GLuint i, const char* n) : index(i), name(n) {}
		};

		namespace attribute_index 
		{
			const pair position(0, "position");
			const pair texcoord(1, "texcoord");
			const pair normal(2, "normal");
			const pair color(1, "color");
			const pair index(3, "index");
		}

		namespace binding_points
		{
			const GLuint globals = 0;

		}

		namespace uniform_index
		{
			const GLuint globals = 0;

			const GLuint modelview = 0;
			const GLuint projection = 1;
			const GLuint normalmatrix = 2;
			const GLuint diffuse = 10;
		}

	}
}
