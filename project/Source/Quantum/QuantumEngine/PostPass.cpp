#include "stdafx.h"
#include "PostPass.h"

namespace render
{

	GLuint PostPass::quad_vao = 0;
	GLuint PostPass::quad_vbo = 0;

	PostPass::PostPass()
	{
		if (quad_vao == 0)
		{
			float buffer[6][4] = { { 1.0, -1.0, 0.0, 1.0 },{ 1.0, 1.0, 0.0, 1.0 }, { -1.0, 1.0, 0.0, 1.0 },{ -1.0, 1.0, 0.0, 1.0 },{ -1.0, -1.0, 0.0, 1.0 },{ 1.0, -1.0, 0.0, 1.0 } };

			glGenBuffers(1, &quad_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
			glBufferData(GL_ARRAY_BUFFER, 6 * 4 * sizeof(float), buffer, GL_STATIC_DRAW);

			// Set up the vertex attributes
			glGenVertexArrays(1, &quad_vao);
			glBindVertexArray(quad_vao);

			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)(0));
			glEnableVertexAttribArray(0);


		}

		glBindVertexArray(quad_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);

	}


	PostPass::~PostPass()
	{
	}
}
