#pragma once
#include "IVertexData.h"
#include "RenderConstants.h"
#include <glm\glm.hpp>

namespace render
{
	class RenderObject
	{
	private:
		friend class RenderCache;
		IVertexData *vertexData;
		int oid;

	public:		
		DrawArraysIndirectCommand cmd;

		RenderObject(IVertexData *vdata, int count, int first, int base_instance);
		virtual ~RenderObject();

		inline IVertexData* getAsignedVertexData() { return oid == -1 ? nullptr : vertexData; }
		inline Shader* getAsignedShader() { return oid == -1 ? nullptr : vertexData->getAsignedShader(); }
		inline RenderCache* getAsignedRenderCache() { return oid == -1 ? nullptr : vertexData->getAsignedRenderCache(); }

	};
}
