#include "stdafx.h"
#include "AShaderStorageBufferObject.h"

namespace render
{

	AShaderStorageBufferObject::AShaderStorageBufferObject(GLsizei size)
	{
		glGenBuffers(1, &ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		this->size = size;
	}

	AShaderStorageBufferObject::~AShaderStorageBufferObject()
	{
		glDeleteBuffers(1, &ssbo);
	}

	void AShaderStorageBufferObject::attach(GLuint shader, int block_index, int binding_point)
	{
		glShaderStorageBlockBinding(shader, block_index, binding_point);
	}

	void AShaderStorageBufferObject::bind(int binding_point)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point, ssbo);
	}

	void AShaderStorageBufferObject::resize(GLsizei newsize, bool copy)
	{
		GLuint ossbo = ssbo;
		ssbo = 0;
		glGenBuffers(1, &ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, newsize, NULL, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_COPY_READ_BUFFER, ossbo);
		glBindBuffer(GL_COPY_WRITE_BUFFER, ssbo);

		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, glm::min(size, newsize));

		glDeleteBuffers(1, &ossbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

}