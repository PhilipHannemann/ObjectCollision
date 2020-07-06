#pragma once
#include "GeometryNode.h"
#include "RigidBodyModelWrapper.h"
#include <PhysicEngine\PhysicsWorld.h>

namespace scenegraph
{

	class StaticRigidBodyNode : public DefaultGeometryNode
	{
	private:
		physics::RigidBody *rigidbody;
		int sbdid;
		physics::DynamicsWorld *cache_world;
		loader::RigidBodyModelWrapper *rbmw;
		bool added;

	public:
		StaticRigidBodyNode(loader::RigidBodyModelWrapper *ci, physics::DynamicsWorld *world, glm::mat4 ch_transform);
		~StaticRigidBodyNode();

		virtual void addedToSceneGraph() override;
		virtual void update(StateBuffer *state) override;

		void resetTransformDebug(glm::mat4 ch_transform);
	};

}