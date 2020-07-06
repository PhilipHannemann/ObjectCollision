#include "stdafx.h"
#include "RenderObject.h"
#include "RenderCache.h"

namespace render
{

	RenderObject::RenderObject(IVertexData * vdata, int count, int first, int base_instance)
	{
		cmd.baseInstance = base_instance;
		cmd.first = first;
		cmd.instanceCount = 0;
		cmd.count = count;

		oid = -1;
		vertexData = vdata;

		RenderCache* rc = vdata->getAsignedRenderCache();

		if (vertexData != nullptr && rc != nullptr)
		{
			rc->registerRenderObject(this);
		}
	}

	RenderObject::~RenderObject()
	{
		RenderCache* rc = vertexData->getAsignedRenderCache();

		if (oid != -1 && rc != nullptr)
		{
			rc->removeRenderObject(this);
		}
	}
}
