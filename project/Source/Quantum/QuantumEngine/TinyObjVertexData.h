#pragma once
#include "IVertexData.h"
#include <opengl_include.h>
#include "TinyObjModel.h"

namespace loader
{

	class TinyObjVertexData : public render::IVertexData
	{
	private:
		friend class TinyObjModel;
		GLuint vbo = 0;
		GLsizei count = -1;
		float *buffer = nullptr;

	public:
		virtual void setup();
		virtual void cleanup();
		virtual void render();

		TinyObjVertexData(render::Shader *s);
		~TinyObjVertexData();
	};

}

