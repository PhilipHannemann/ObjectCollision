#pragma once

#include "IVertexData.h"
#include <opengl_include.h>
#include <glm\glm.hpp>
#include <vector>
#include <PhysicEngine\SoftBody.h>

namespace loader
{

	class SoftBodyVertexData : public render::IVertexData
	{
	private:
		friend class SoftBodyModelWrapper;
		//GLuint vao = 0;
		GLuint stream_tex = 0;
		GLuint static_vbo = 0;
		GLuint index_vbo = 0;

		GLsizei vertex_count = -1;
		GLsizei count = -1;

		float *cache = nullptr;
		GLsizei cache_size = -1;

		GLuint *index_buffer;
		GLfloat *static_buffer;

		std::vector<std::vector<int>> adjlist;

		glm::vec3 calcNormal(std::vector<physics::Particle*> &data, int id);

		physics::SoftBody *sbdebug;

		std::vector<physics::SoftBody*> instances;

		GLint vdata_uloc;

	public:
		SoftBodyVertexData(render::Shader *s);
		//SoftBodyVertexData(SoftBodyVertexData &data);
		virtual ~SoftBodyVertexData();

		virtual void setup();
		virtual void cleanup();
		void prepareRender() override;
		//virtual void update(physics::SoftBody *soft);
		int enqueInstance(physics::SoftBody *sb);
		inline void updateSoftBodyDataID(physics::SoftBody *sb, int &old_id)
		{
			while (sb != instances[old_id] || old_id <= -1)
				old_id--;
		}
		void dequeInstance(physics::SoftBody *sb);
	};

}
