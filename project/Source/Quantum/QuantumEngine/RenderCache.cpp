#include "stdafx.h"
#include "RenderCache.h"
#include "Shader.h"
#include "IVertexData.h"
#include "RenderObject.h"
#include "RenderInstance.h"

namespace render
{

	RenderCache::RenderCache()
	{
		//glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUniformSize);
		//dynamicUniformCache = new unsigned char[maxUniformSize];
		dyndata = new UBODynamicData();
		globals = new UBOGlobals();
		cmd = new DrawArraysIndirectCommand[UBODynamicDataCount];
		//ubo bind
	}

	RenderCache::~RenderCache()
	{
		for (PerShaderCache *psc : shaders)
		{
			for (PerVAOCache* pvc : psc->vaos)
			{
				for (PerObjectCache *poc : pvc->objects)
				{
					poc->o->oid = -1;
					delete poc;
				}
				pvc->vao->rid = -1;
				delete pvc;
			}
			psc->shader->sid = -1;
			delete psc;
		}
		delete globals;
		delete dyndata;
		delete[] cmd;
	}


	void RenderCache::registerShader(render::Shader *s)
	{
		s->useProgram();

		//Prepare Shader
		globals->attach(s->program, glGetUniformBlockIndex(s->program, "shader_data"), 0);
		globals->bind(0);

		dyndata->attach(s->program, glGetUniformBlockIndex(s->program, "dynamic_data"), 1);
		dyndata->bind(1);

		//Add Shader
		s->sid = (int) shaders.size();
		shaders.push_back(new PerShaderCache(s));

	}

	void RenderCache::removeShader(render::Shader * s)
	{
		for (PerVAOCache* pvc : shaders[s->sid]->vaos)
		{
			for (PerObjectCache *poc : pvc->objects)
			{
				poc->o->oid = -1;
				delete poc;
			}
			pvc->vao->rid = -1;
			delete pvc;
		}

		//swap with last
		std::swap(shaders[s->sid], shaders[shaders.size() - 1]);

		//asign old id
		shaders[s->sid]->shader->sid = s->sid;

		delete shaders[shaders.size() - 1];

		//remove from list
		shaders.pop_back();
		
		//remove
		s->sid = -1;
	}


	void RenderCache::registerModel(render::IVertexData * v)
	{
		v->rid = (int)shaders[v->shader->sid]->vaos.size();
		shaders[v->shader->sid]->vaos.push_back(new PerVAOCache(v));
	}

	void RenderCache::removeModel(render::IVertexData * v)
	{
		for (PerObjectCache *poc : shaders[v->shader->sid]->vaos[v->rid]->objects)
		{
			poc->o->oid = -1;
			delete poc;
		}

		//swap with last
		std::swap(shaders[v->shader->sid]->vaos[v->rid], shaders[v->shader->sid]->vaos[shaders[v->shader->sid]->vaos.size() - 1]);

		//asign old id
		shaders[v->shader->sid]->vaos[v->rid]->vao->rid = v->rid;

		delete shaders[v->shader->sid]->vaos[shaders[v->shader->sid]->vaos.size() - 1];

		//remove from list
		shaders[v->shader->sid]->vaos.pop_back();

		//remove
		v->rid = -1;
	}


	void RenderCache::registerRenderObject(render::RenderObject * ro)
	{
		ro->oid = (int)shaders[ro->vertexData->shader->sid]->vaos[ro->vertexData->rid]->objects.size();
		shaders[ro->vertexData->shader->sid]->vaos[ro->vertexData->rid]->objects.push_back(new PerObjectCache(ro));
	}

	void RenderCache::removeRenderObject(render::RenderObject * ro)
	{
		//swap with last
		int size = (int) shaders[ro->vertexData->shader->sid]->vaos[ro->vertexData->rid]->objects.size();
		std::swap(shaders[ro->vertexData->shader->sid]->vaos[ro->vertexData->rid]->objects[ro->oid], shaders[ro->vertexData->shader->sid]->vaos[ro->vertexData->rid]->objects[size - 1]);

		//asign old id
		shaders[ro->vertexData->shader->sid]->vaos[ro->vertexData->rid]->objects[ro->oid]->o->oid = ro->oid;

		delete shaders[ro->vertexData->shader->sid]->vaos[ro->vertexData->rid]->objects[size - 1];

		//remove from list
		shaders[ro->vertexData->shader->sid]->vaos[ro->vertexData->rid]->objects.pop_back();

		//remove
		ro->oid = -1;
	}



	void RenderCache::render()
	{
		//ubo-global bind
		globals->updateAll();
		globals->bind(0);
		dyndata->bind(1);
		
		//global ssbo bind

		for (PerShaderCache *psc : shaders)
		{
			psc->shader->useProgram();

			for (PerVAOCache* pvc : psc->vaos)
			{
				pvc->vao->bind();
				pvc->vao->prepareRender();

				int cmdindex = 0;
				int instanceindex = 0;

				for (PerObjectCache *poc : pvc->objects)
				{
					if (poc->instances.size() > 0)
					{
						cmd[cmdindex] = poc->o->cmd;
						cmd[cmdindex].instanceCount = (int) poc->instances.size();

						dyndata->data[cmdindex][0] = instanceindex;

						for (ARenderInstance *r : poc->instances)
						{
							dyndata->data[instanceindex][1] = r->indizes[1];
							dyndata->data[instanceindex][2] = r->indizes[2];
							dyndata->data[instanceindex][3] = r->indizes[3];

							instanceindex++;
						}

						poc->instances.clear();

						cmdindex++;
					}	
				}

				dyndata->updateRange(0, instanceindex + 1);
				glMultiDrawArraysIndirect(GL_TRIANGLES, cmd, cmdindex, sizeof(DrawArraysIndirectCommand));	//cmdindex is used as count
			}
		}
	}

	void RenderCache::enqueueInstance(render::ARenderInstance *o)
	{
		int sid = o->ro->vertexData->shader->sid;
		int rid = o->ro->vertexData->rid;
		int oid = o->ro->oid;

		assert(sid >= 0); assert(sid < shaders.size());

		shaders[sid]->vaos[rid]->objects[oid]->instances.push_back(o);	
	}

}