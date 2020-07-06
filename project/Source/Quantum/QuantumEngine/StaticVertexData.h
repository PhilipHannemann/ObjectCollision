#pragma once
#include "IVertexData.h"

namespace loader
{
	class RigidBodyModelWrapper;

	class StaticVertexData : public render::IVertexData
	{
	private:
		friend class RigidBodyModelWrapper;
		GLuint vbo = 0;
		GLsizei count = -1;
		float *buffer = nullptr;

	public:
		StaticVertexData(render::Shader *s);
		~StaticVertexData();

		virtual void setup();
		virtual void cleanup();
	};

}
