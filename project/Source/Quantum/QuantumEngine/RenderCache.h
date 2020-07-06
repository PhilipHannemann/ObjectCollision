#pragma once
#include "AUniformBufferObject.h"
#include "RenderConstants.h"
#include "UBOGlobals.h"
#include "UBODynamicData.h"

namespace render
{
	class IVertexData;
	class Shader;
	class IVertexData;
	class RenderObject;
	class ARenderInstance;
}

#include <EngineCore\Logger.h>
#include "RenderInstance.h"

namespace render
{

class RenderCache
{
private:

	class PerObjectCache
	{
	public:
		render::RenderObject *o;
		std::vector<ARenderInstance*> instances;

		PerObjectCache(render::RenderObject *ro) { o = ro; }
		~PerObjectCache() {}
	};

	class PerVAOCache
	{
	public:
		IVertexData *vao;

		std::vector<PerObjectCache*> objects;

		PerVAOCache(render::IVertexData* v) { vao = v; }
		~PerVAOCache() {}
	};


	class PerShaderCache
	{
	public:
		render::Shader* shader;

		std::vector<PerVAOCache*> vaos;

		PerShaderCache(render::Shader *s) { this->shader = s; }
		~PerShaderCache() {}
	};
	

private:
	std::vector<PerShaderCache*> shaders;

private:
	UBODynamicData *dyndata;
	DrawArraysIndirectCommand *cmd;

public:
	UBOGlobals *globals;

public:
	RenderCache();
	~RenderCache();
	void registerRenderObject(render::RenderObject * ro);
	void registerModel(render::IVertexData* v);
	void registerShader(render::Shader *s);
	void removeRenderObject(render::RenderObject *ro);
	void removeModel(render::IVertexData* v);
	void removeShader(render::Shader *s);
	void render();

	void enqueueInstance(render::ARenderInstance *o);

};

}