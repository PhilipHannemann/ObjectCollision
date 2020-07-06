#pragma once
#include <EngineCore\IModel.h>
#include "SoftBodyVertexData.h"
#include "RenderObject.h"
#include <PhysicEngine\SphereTreeCollisionHull.h>
#include <PhysicEngine\ClusterBuilder.h>

namespace loader
{
	/*
	*	- Particle list for Softbody
	*	- VData for RenderCache
	*	- data texture
	*	- update func
	*/

	class SoftBodyModelWrapper
	{
	private:
		int findOrAdd(glm::vec3 vert);
		void addAdjazent(int source, int dest1, int dest2);

	public:
		//VAO
		SoftBodyVertexData *vao;
		render::RenderObject *stdro;
		//DATA for init Softbody
		std::vector<glm::vec3> data;
		physics::CollisionHull *hull;
		physics::ClusterMesh *clusters;

		SoftBodyModelWrapper(render::Shader *s, IModel *imod);
		//void setup(render::RenderCache *rc);
		//void update(physics::SoftBody *soft);
		~SoftBodyModelWrapper();
	};
}