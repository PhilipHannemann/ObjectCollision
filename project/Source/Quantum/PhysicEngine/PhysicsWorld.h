#pragma once

#include"Broadphase.h"
#include "Dispatcher.h"
#include "SoftBodySolver.h"
#include <EngineCore\Event.hpp>

namespace physics
{
	/*World for Collision-Test between static Bodies*/
	class CollisionWorld
	{
	protected:
		//components
		Broadphase * broadphase;
		Dispatcher* dispatcher;

		//body managment
		std::vector<CollisionBody*> bodies;
		unsigned int storageIndexCounter;

		//invalidate tick
		static int64_t tick_counter;

	public:
		CollisionWorld(Broadphase* broadphase, Dispatcher* dispatcher);
		virtual ~CollisionWorld();

	public:
		//manage
		virtual void addCollisionBody(CollisionBody* body);
		virtual void removeCollisionBody(CollisionBody* body);
	
	public:
		//interact
		virtual PickResult getFirstPickResult(Ray ray);

		//Force Update: Increase Tick counter and perform update of marked objects
		virtual void updateData();

	public:
		static int64_t getCurrentTick() { return tick_counter; }
	};


	/*World for simulating dynamic Bodies*/
	class DynamicsWorld : public CollisionWorld
	{
	public:
		// world variables
		glm::vec3 gravity;
		float air_drag;
		
	private:
		//components	
		SoftBodySolver* sb_solver;

	public:
		DynamicsWorld(Broadphase* broadphase, Dispatcher* dispatcher, SoftBodySolver* sb_solver, glm::vec3 gravity = glm::vec3(0.f, -9.8f, 0.f), float air_drag = 0.f);
		~DynamicsWorld();

	public:
		//interact
		void stepWorld(float duration);
		void applyForce(CollisionBody *body, glm::vec3 dir, float value);

	public:
		//deprecated
		void renderDebugInfos();	
	};


	/*Event for CollisionBody removal*/
	class CollsionBodyRemovedEventData : public core::events::EventDataSimple
	{
	public:
		CollisionBody *body;
		CollsionBodyRemovedEventData(CollisionBody *b): body(b) {}
	};
}