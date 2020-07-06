#pragma once
#include <glm\glm.hpp>
#include <opengl_include.h>

namespace render
{

	class IDebugRender
	{
	public:
		static IDebugRender* getDebugRender();
		static void setDebugRender(IDebugRender* idr);
		virtual ~IDebugRender() {};
		virtual void drawData(float* data, int size, GLuint pr_type, int start, int end, glm::vec4 color) = 0;
		virtual void drawSphere(glm::vec3 center, float radius, glm::vec4 color) = 0;
	};

}