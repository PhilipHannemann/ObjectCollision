#include "stdafx.h"
#include "DebugRender.h"
#include "ConstructShaders.h"
#include <EngineCore/ResourceManager.h>
#include <EngineCore/FileManager.h>
#include "Camera.h"
#include <opengl_include.h>
#include "TinyObjModel.h"


namespace render
{

	DebugRender::DebugRender()
	{
		/*memleaks*/
		shader = construct::make_PointShader(nullptr);

		glGenBuffers(1, &buffer);
		glGenVertexArrays(1, &vao);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);

		glBufferData(GL_ARRAY_BUFFER, 2 * 3 * sizeof(float), NULL, GL_STREAM_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)(0));
		glEnableVertexAttribArray(0);

		loader::TinyObjModel *m_static = new loader::TinyObjModel(RM_FILE_BASE->getDirPath("content"), "sphere.obj");

		sphere_vd = new loader::TinyObjVertexData(shader);

		m_static->createRenderData(sphere_vd);

		delete m_static;

		cam = nullptr;
	}


	DebugRender::~DebugRender()
	{
		glDeleteBuffers(1, &buffer);
		glDeleteVertexArrays(1, &vao);

		delete sphere_vd;
		delete shader;
	}

	void DebugRender::drawData(float * data, int size, GLuint pr_type, int start, int count, glm::vec4 color)
	{
		if (cam != nullptr)
		{
			shader->useProgram();

			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, 3 * size * sizeof(float), data, GL_STREAM_DRAW);

			glm::mat4x4 mvm = cam->getViewMatrix();
			glUniformMatrix4fv(render::constant::uniform_index::modelview, 1, GL_FALSE, glm::value_ptr(mvm));
			glUniformMatrix4fv(render::constant::uniform_index::projection, 1, GL_FALSE, glm::value_ptr(cam->getProjectionMatrix()));

			glUniform4f(100, color.r, color.g, color.b, color.a);

			glDrawArrays(pr_type, start, count);
		}
	}

	void DebugRender::drawSphere(glm::vec3 center, float radius, glm::vec4 color)
	{
		if (cam != nullptr)
		{
			shader->useProgram();

			//glBindVertexArray(vao);
			//glBindBuffer(GL_ARRAY_BUFFER, buffer);

			//sphere_vd->bind();

			//glBufferData(GL_ARRAY_BUFFER, 3 * size * sizeof(float), data, GL_STREAM_DRAW);

			glm::mat4 m = glm::scale(glm::translate(glm::mat4(1), center), glm::vec3(radius, radius, radius));

			glm::mat4x4 mvm = cam->getViewMatrix();
			glUniformMatrix4fv(render::constant::uniform_index::modelview, 1, GL_FALSE, glm::value_ptr(mvm * m));
			glUniformMatrix4fv(render::constant::uniform_index::projection, 1, GL_FALSE, glm::value_ptr(cam->getProjectionMatrix()));

			glUniform4f(100, color.r, color.g, color.b, color.a);

			sphere_vd->render();

			//glDrawArrays(pr_type, start, count);
		}
	}


}