#include "stdafx.h"
#include "AUniformBufferObject.h"

namespace render
{

	AUniformBufferObject::AUniformBufferObject(GLsizeiptr size)
	{
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	AUniformBufferObject::~AUniformBufferObject()
	{
		glDeleteBuffers(1, &ubo);
	}

	void AUniformBufferObject::attach(GLuint shader, int block_index, int binding_point)
	{
		glUniformBlockBinding(shader, block_index, binding_point);
	}

	void AUniformBufferObject::bind(int binding_point)
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, ubo);
	}

}