#include "stdafx.h"
#include "IVertexData.h"
#include "RenderCache.h"

namespace render
{

	IVertexData::IVertexData(Shader * s)
	{
		shader = s;
		rid = -1;
		RenderCache* rc = s->getAsignedRenderCache();
		
		if (s != nullptr && rc != nullptr)
		{
			rc->registerModel(this);
		}
	}

	IVertexData::~IVertexData()
	{
		RenderCache* rc = getAsignedRenderCache();

		if (rid != -1 && rc != nullptr)
		{
			rc->removeModel(this);
		}
	}


}