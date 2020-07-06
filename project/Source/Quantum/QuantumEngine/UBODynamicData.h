#pragma once
#include "AUniformBufferObject.h"
#include <glm\glm.hpp>

#define UBODynamicDataCount 1000

namespace render
{

	class UBODynamicData : public AUniformBufferObject
	{
	public:
		UBODynamicData();
		virtual ~UBODynamicData();

	public:

		glm::ivec4 data[UBODynamicDataCount];

		virtual void updateAll() override;
		virtual void updateRange(GLuint start, GLuint count) override;
	};

}