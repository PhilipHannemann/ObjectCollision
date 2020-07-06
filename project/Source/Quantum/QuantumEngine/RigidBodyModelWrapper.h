#pragma once
#include <EngineCore\IModel.h>
#include "RenderObject.h"
#include "StaticVertexData.h"
#include <PhysicEngine\CollisionHull.h>

namespace loader
{

	class RigidBodyModelWrapper
	{
	public:
		//VAO
		StaticVertexData * vao;
		render::RenderObject *stdro;
		physics::CollisionHull *hull;

		RigidBodyModelWrapper(render::Shader *s, IModel *imod);
		~RigidBodyModelWrapper();

		physics::CollisionHull* getTransformedCollisionHullCopy(glm::mat4 transform);
	};

}
