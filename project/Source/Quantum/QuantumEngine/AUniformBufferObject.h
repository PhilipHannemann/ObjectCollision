#pragma once
#include <opengl_include.h>

namespace render
{

	class AUniformBufferObject
	{
	protected:
		GLuint ubo;

	public:
		AUniformBufferObject(GLsizeiptr size);
		virtual ~AUniformBufferObject();

		void bind(int binding_point);
		void attach(GLuint shader, int block_index, int binding_point);

		virtual void updateAll() = 0;
		virtual void updateRange(GLuint start, GLuint count) = 0;
	};

}