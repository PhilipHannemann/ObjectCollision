#pragma once

#include "Shader.h"
#include "Camera.h"
#include <EngineCore\IDebugRender.h>
#include "TinyObjVertexData.h"

namespace render
{

	class DebugRender : public IDebugRender
	{
	private:
		Shader *shader;
		GLuint buffer;
		GLuint vao;
		Camera *cam;
		loader::TinyObjVertexData *sphere_vd;

	public:
		DebugRender();
		~DebugRender();

		void setCamera(Camera *c) { cam = c; }

		virtual void drawData(float* data, int size, GLuint pr_type, int start, int end, glm::vec4 color) override;
		virtual void drawSphere(glm::vec3 center, float radius, glm::vec4 color) override;

	};

}