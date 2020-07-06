#pragma once

#include "AShaderStorageBufferObject.h"
#include "RenderObject.h"
#include "Shader.h"

namespace render
{
	class RenderCache;
	class RenderObject;

	
	class ARenderInstance
	{
	protected:
		friend class RenderCache;
		
		RenderObject *ro;

	public:
		//temp
		int ssboid;
		glm::ivec4 indizes;		//For UBODynamicData: indizes[0] will be overriden
		ARenderInstance(RenderObject *ro) 
		{
			ssboid = ro->getAsignedShader()->ssbo->addBlock();
			indizes = glm::ivec4(0, ssboid, 0, 0);
			this->ro = ro;	
		}
		virtual ~ARenderInstance() 
		{
			ro->getAsignedShader()->ssbo->removeBlock(ssboid);
		}
	};

	template<typename T>
	class RenderInstance : public ARenderInstance
	{
	public:
		RenderInstance(RenderObject *ro) : ARenderInstance(ro) {}
		virtual ~RenderInstance() {/*TODO*/}
		T* getSSBO() { return ((T*)ro->getAsignedShader()->ssbo); }
	};
}