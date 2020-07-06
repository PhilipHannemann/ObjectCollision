#include "stdafx.h"
#include "UBOGlobals.h"

namespace render
{
	UBOGlobals::UBOGlobals() : AUniformBufferObject(sizeof(DataStruct))
	{

	}

	UBOGlobals::~UBOGlobals()
	{
	}

	void UBOGlobals::updateAll()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DataStruct), &data);
	}

	void UBOGlobals::updateRange(GLuint start, GLuint count)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DataStruct), &data);
	}

}