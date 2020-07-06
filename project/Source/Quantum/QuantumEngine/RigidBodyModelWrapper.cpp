#include "stdafx.h"
#include "RigidBodyModelWrapper.h"
#include <PhysicEngine\AABBCollisionHull.h>

namespace loader
{

	RigidBodyModelWrapper::RigidBodyModelWrapper(render::Shader * s, IModel * imod)
	{
		vao = new StaticVertexData(s);
		IndexedTriagnles *tri = imod->getIndexedTriagnles();
		
		glm::vec3 min(tri->data.vertex[3 * tri->indices[0].vertex + 0], tri->data.vertex[3 * tri->indices[0].vertex + 1], tri->data.vertex[3 * tri->indices[0].vertex + 2]);
		glm::vec3 max = min;

		vao->count = tri->indices_size * 3;
		vao->buffer = new float[vao->count * 8];

		//TODO: use indexed Buffer
		for (int i = 0; i < tri->indices_size * 3; i ++)
		{
			glm::vec3 pos;
			pos.x = tri->data.vertex[3 * tri->indices[i].vertex + 0];
			pos.y = tri->data.vertex[3 * tri->indices[i].vertex + 1];
			pos.z = tri->data.vertex[3 * tri->indices[i].vertex + 2];

			min.x = glm::min(min.x, pos.x); min.y = glm::min(min.y, pos.y); min.z = glm::min(min.z, pos.z);
			max.x = glm::max(max.x, pos.x); max.y = glm::max(max.y, pos.y); max.z = glm::max(max.z, pos.z);

			vao->buffer[8 * i + 0] = pos.x;
			vao->buffer[8 * i + 1] = pos.y;
			vao->buffer[8 * i + 2] = pos.z;
			vao->buffer[8 * i + 3] = tri->data.normal[3 * tri->indices[i].normal + 0];
			vao->buffer[8 * i + 4] = tri->data.normal[3 * tri->indices[i].normal + 1];
			vao->buffer[8 * i + 5] = tri->data.normal[3 * tri->indices[i].normal + 2];
			vao->buffer[8 * i + 6] = tri->data.texcoord[3 * tri->indices[i].texcoord + 0];
			vao->buffer[8 * i + 7] = tri->data.texcoord[3 * tri->indices[i].texcoord + 1];

		}

		vao->setup();

		stdro = new render::RenderObject(vao, vao->count, 0, 0);

		//building Collision Hull (AABB)
		hull = new physics::AABBCollisionHull(min, max);

		delete tri;
	}

	loader::RigidBodyModelWrapper::~RigidBodyModelWrapper()
	{
		delete hull;
		delete stdro;
		delete vao;
	}

	physics::CollisionHull * RigidBodyModelWrapper::getTransformedCollisionHullCopy(glm::mat4 transform)
	{
		glm::vec3 min = glm::vec3(transform * glm::vec4(((physics::AABBCollisionHull*)hull)->min, 1));
		glm::vec3 max = glm::vec3(transform * glm::vec4(((physics::AABBCollisionHull*)hull)->max, 1));

		glm::vec3 nmin, nmax;

		nmin.x = glm::min(min.x, max.x); nmin.y = glm::min(min.y, max.y); nmin.z = glm::min(min.z, max.z);
		nmax.x = glm::max(max.x, min.x); nmax.y = glm::max(max.y, min.y); nmax.z = glm::max(max.z, min.z);

		return new physics::AABBCollisionHull(nmin, nmax);
	}

}