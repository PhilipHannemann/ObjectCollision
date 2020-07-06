#include "stdafx.h"
#include "UBODynamicData.h"

namespace render
{

	UBODynamicData::UBODynamicData() : AUniformBufferObject(UBODynamicDataCount * sizeof(glm::ivec4))
	{

	}

	UBODynamicData::~UBODynamicData()
	{
	}

	void UBODynamicData::updateAll()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, UBODynamicDataCount * sizeof(glm::ivec4), data);
	}

	void UBODynamicData::updateRange(GLuint start, GLuint count)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, start * sizeof(glm::ivec4), count * sizeof(glm::ivec4), data + start);
	}

}
