#include "stdafx.h"
#include "StaticVertexData.h"

namespace loader
{

	StaticVertexData::StaticVertexData(render::Shader * s):render::IVertexData(s)
	{
	}

	StaticVertexData::~StaticVertexData()
	{
		cleanup();

		if (buffer != nullptr)
			delete[] buffer;
	}

	void StaticVertexData::cleanup()
	{
		if (vbo != 0)
		{
			glDeleteBuffers(1, &vbo);
			vbo = 0;
		}

		if (vao != 0)
		{
			glDeleteVertexArrays(1, &vao);
			vao = 0;
		}
	}

	void StaticVertexData::setup()
	{
		if (vbo != 0 || vao != 0)
			cleanup();

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, count * 8 * sizeof(float), buffer, GL_STATIC_DRAW);

		// Set up the vertex attributes
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * 4, (void*)(0 * 4));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * 4, (void*)(3 * 4));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * 4, (void*)(6 * 4));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		delete[] buffer;
		buffer = nullptr;
	}
}