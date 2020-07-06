#include "stdafx.h"
#include "SoftBodyVertexData.h"
#include "RenderConstants.h"
#include <PhysicEngine\SoftBody.h>
#include <EngineCore\IModel.h>
#include <EngineCore\MathHelpers.h>
#include <EngineCore\Logger.h>

#include "DebugRender.h"
#include <EngineCore\ResourceManager.h>
#include <EngineCore\SDLCore.h>

#include <glm/gtx/simd_vec4.hpp>

using namespace render::constant;

namespace loader
{

	glm::vec3 SoftBodyVertexData::calcNormal(std::vector<physics::Particle*>& data, int id)
	{

		auto &list = adjlist[id];

		glm::vec3 pos = data[id]->pos;

		glm::vec3 d1 = data[list[0]]->pos - pos;
		glm::vec3 d2 = data[list[1]]->pos - pos;
		float a = acos(glm::dot(d1, d2) / (glm::length(d1) * glm::length(d2)));
		glm::vec3 n = glm::normalize(glm::cross(d1,d2)) * a;

		for (int i = 2; i < list.size(); i+=2)
		{
			d1 = data[list[i+0]]->pos - pos;
			d2 = data[list[i+1]]->pos - pos;
			float a = acos(glm::dot(d1, d2) / (glm::length(d1) * glm::length(d2)));
			n += glm::normalize(glm::cross(d1, d2)) * a;
		}

		return glm::normalize(n);
	}

	SoftBodyVertexData::SoftBodyVertexData(render::Shader *s) : render::IVertexData(s)
	{
		vao = 0;
		//stream_tbo = 0;
		stream_tex = 0;
		static_vbo = 0;
		index_vbo = 0;

		sbdebug = nullptr;
	}
	/*
	SoftBodyVertexData::SoftBodyVertexData(SoftBodyVertexData & data)
	{
		vertex_count = data.vertex_count;
		count = data.count;

		vao = 0;
		stream_tbo = 0;
		stream_tbo_tex = 0;
		static_vbo = 0;
		index_vbo = 0;

		index_buffer = new GLuint[count];
		memcpy_s(index_buffer, count * sizeof(GLuint), data.index_buffer, count * sizeof(GLuint));

		static_buffer = new GLfloat[count * 2];
		memcpy_s(static_buffer, count * 2 * sizeof(GLfloat), data.static_buffer, count * 2 * sizeof(GLfloat));

		adjlist = data.adjlist;
	}
	*/

	SoftBodyVertexData::~SoftBodyVertexData()
	{
		if (static_buffer != nullptr)
			delete static_buffer;

		if(index_buffer != nullptr)
			delete index_buffer;

		if (cache != nullptr)
			delete[] cache;
	
	}

	void SoftBodyVertexData::setup()
	{
		//Generate
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &static_vbo);
		glGenBuffers(1, &index_vbo);

		//Data Texture
		glGenTextures(1, &stream_tex);
		glBindTexture(GL_TEXTURE_2D, stream_tex);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindVertexArray(vao);


		//Bind
		glBindBuffer(GL_ARRAY_BUFFER, static_vbo);
		glBufferData(GL_ARRAY_BUFFER, count * 2 * sizeof(float), static_buffer, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * 4, (void*)(0));
		glEnableVertexAttribArray(0);


		//Bind
		glBindBuffer(GL_ARRAY_BUFFER, index_vbo);
		glBufferData(GL_ARRAY_BUFFER, count * 1 * sizeof(unsigned int), index_buffer, GL_STATIC_DRAW);
		glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 0, (void*)(0));
		glEnableVertexAttribArray(1);


		glBindVertexArray(0);

		vdata_uloc = glGetUniformLocation(this->getAsignedShader()->program, "vertex_data_tex");

		delete[] index_buffer;
		index_buffer = nullptr;
		delete[] static_buffer;
		static_buffer = nullptr;
	}

	void SoftBodyVertexData::cleanup()
	{
		//glDeleteBuffers(1, &stream_tbo);
		glDeleteBuffers(1, &static_vbo);
		glDeleteBuffers(1, &index_vbo);
		glDeleteVertexArrays(1, &vao);
		glDeleteTextures(1, &stream_tex);
	}

	void SoftBodyVertexData::prepareRender()
	{
		//glBindVertexArray(vao);

		//Empty
		if (instances.size() == 0)
			return;

		//NO cache
		if (cache == nullptr)
		{
			cache = new float[2 * vertex_count * instances.size() * 3];
			cache_size = (GLsizei) instances.size();
		}

		//invalid cache
		if (instances.size() != cache_size)
		{
			cache = (float*) realloc(cache, instances.size() * vertex_count * 2 * 3 * sizeof(float));
			cache_size = (GLsizei) instances.size();
		}


		for (int i = 0; i < instances.size(); i++)
		{
			float* data = cache + i * vertex_count * 2 * 3;
			auto &list = instances[i]->particles;

			for (int p = 0; p < list.size(); p++)
			{
				auto pos = list[p]->pos;

#ifdef RENDER_PARTICLE_POS_SMOOTH

				auto ppos = list[p]->pos_prev_step;
				data[6 * p + 0] = (pos.x + ppos.x) / 2.0f;
				data[6 * p + 1] = (pos.y + ppos.y) / 2.0f;
				data[6 * p + 2] = (pos.z + ppos.z) / 2.0f;

#else

				data[6 * p + 0] = pos.x;
				data[6 * p + 1] = pos.y;
				data[6 * p + 2] = pos.z;

#endif // RENDER_PARTICLE_POS_SMOOTH

				//core::Logger::error("POS: " + std::to_string(pos.x) + " - " + std::to_string(pos.y) + " - " + std::to_string(pos.z));

				glm::vec3 normal = calcNormal(list, p);

				data[6 * p + 3] = normal.x;
				data[6 * p + 4] = normal.y;
				data[6 * p + 5] = normal.z;

			}

		}
		
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, stream_tex);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 2 * vertex_count, (GLsizei) instances.size(), 0, GL_RGB, GL_FLOAT, cache);
		
		glBindImageTexture(3, stream_tex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glUniform1i(vdata_uloc, 3);
		
		glDisable(GL_CULL_FACE);
		//glPolygonMode(GL_FRONT, GL_LINE);
		//glPolygonMode(GL_BACK, GL_LINE);
	}

	int SoftBodyVertexData::enqueInstance(physics::SoftBody *sb)
	{
		instances.push_back(sb);
		return instances.size() - 1;
	}

	void SoftBodyVertexData::dequeInstance(physics::SoftBody * sb)
	{
		instances.erase(std::remove(instances.begin(), instances.end(), sb), instances.end());
	}

}