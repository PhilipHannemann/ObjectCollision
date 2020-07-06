#pragma once
#include "Shader.h"

namespace render
{
	class Shader;

	class IVertexData
	{
	private:
		friend class RenderCache;
		int rid;
		Shader *shader;
	protected:		
		GLuint vao = 0;
	public:
		IVertexData(Shader * s);
		virtual ~IVertexData();
		virtual void setup() = 0;
		virtual void cleanup() = 0;
		virtual void bind() { glBindVertexArray(vao); }
		virtual void prepareRender() {}
		inline Shader* getAsignedShader() { return rid == -1 ? nullptr : shader; }
		inline RenderCache* getAsignedRenderCache() { return rid == -1 ? nullptr : shader->getAsignedRenderCache(); }
	};

}
