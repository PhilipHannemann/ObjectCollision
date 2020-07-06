#pragma once
#include "AUniformBufferObject.h"
#include <glm\glm.hpp>

#define MAX_LIGHT 16

namespace render
{

	class UBOGlobals : public AUniformBufferObject
	{
	public:
		UBOGlobals();
		virtual ~UBOGlobals();

	public:
		struct LightStruct
		{
			glm::vec4 ambient;
			glm::vec4 diffuse;
			glm::vec4 specular;
			glm::vec4 position;
		};

		struct DataStruct
		{
			glm::mat4 projectionMatrix;
			glm::mat4 viewMatrix;
			glm::mat4 normalMatrix;	//transpose(inverse(viewMatrix))

			LightStruct lights[16];

			int numligths;

		} data;

		virtual void updateAll() override;
		virtual void updateRange(GLuint start, GLuint count) override;

	};

}