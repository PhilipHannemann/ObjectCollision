#include "stdafx.h"
#include "StaticRigidBodyNode.h"
#include <PhysicEngine\RigidBody.h>
#include <EngineCore\BaseExceptions.h>
#include "TransformationNode.h"

namespace scenegraph
{
	StaticRigidBodyNode::StaticRigidBodyNode(loader::RigidBodyModelWrapper * ci, physics::DynamicsWorld *world, glm::mat4 ch_transform):DefaultGeometryNode(ci->stdro)
	{
		added = false;

		rbmw = ci;

		rigidbody = new physics::RigidBody(0, rbmw->hull->copyAndTransform(ch_transform));

		cache_world = world;
	}

	StaticRigidBodyNode::~StaticRigidBodyNode()
	{
		cache_world->removeCollisionBody(rigidbody);
	}

	void StaticRigidBodyNode::addedToSceneGraph()
	{
		if (added)
		{
			throw new SceneGraphConstructingException("PhysicTransformationNode can't be added multiple time! Copy it before adding!");
		}

		cache_world->addCollisionBody(rigidbody);

		added = true;

		DefaultGeometryNode::addedToSceneGraph();
	}

	void StaticRigidBodyNode::update(StateBuffer * state)
	{
		DefaultGeometryNode::update(state);
		//TODO
	}


	void StaticRigidBodyNode::resetTransformDebug(glm::mat4 ch_transform)
	{
		delete rigidbody->collisionHull;
		rigidbody->collisionHull = rbmw->hull->copyAndTransform(ch_transform);
		rigidbody->last_change = physics::DynamicsWorld::getCurrentTick();
		((TransformationNode*)parent)->setTransformation(ch_transform);
	}

}