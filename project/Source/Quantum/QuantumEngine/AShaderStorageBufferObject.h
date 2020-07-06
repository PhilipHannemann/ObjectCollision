#pragma once
#include <opengl_include.h>

namespace render
{
	/*
		Use later Sparse Buffers for better resize
	*/


	class AShaderStorageBufferObject
	{
	protected:
		GLuint ssbo;
		GLsizei size;

	public:
		AShaderStorageBufferObject(GLsizei size);
		virtual ~AShaderStorageBufferObject();

		void bind(int binding_point);
		void attach(GLuint shader, int block_index, int binding_point);

		void resize(GLsizei newsize, bool copy);

		virtual void updateAll() = 0;
		virtual void updateRange(GLuint start, GLuint count) = 0;

		virtual int addBlock() = 0;
		virtual void removeBlock(int id) = 0;
	};

}