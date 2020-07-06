#include "stdafx.h"
#include "PhysicsWorld.h"
#include <EngineCore\Logger.h>

namespace physics
{
	/*** Collision World ***/

	int64_t CollisionWorld::tick_counter;

	CollisionWorld::CollisionWorld(Broadphase * broadphase, Dispatcher * dispatcher)
	{
		storageIndexCounter = 0;
		tick_counter = 0;

		this->broadphase = broadphase;
		this->dispatcher = dispatcher;

		core::events::Event<CollsionBodyRemovedEventData>::setDeleteEventData(true);
	}

	CollisionWorld::~CollisionWorld()
	{
		// delete broadphase and BVH tree
		delete broadphase;

		// delete dispatcher
		delete dispatcher;

		// delete bodies
		for (int i = 0; i < bodies.size(); i++)
			delete bodies[i];
	}

	// Adds a collision body to the simulation.
	void CollisionWorld::addCollisionBody(CollisionBody* body)
	{
		body->storageIndexID = storageIndexCounter++;

		bodies.push_back(body);

		broadphase->addBody(body);
	}

	// Removes a collision body from the simulation.
	void CollisionWorld::removeCollisionBody(CollisionBody* body)
	{
		core::events::Event<CollsionBodyRemovedEventData>::fireEvent(new CollsionBodyRemovedEventData(body));

		unsigned int erasedID = body->storageIndexID;

		for (int i = 0; i < bodies.size(); ++i)
		{
			if (bodies[i] == body)
			{
				broadphase->removeBody(body);
				delete bodies[i];
				bodies.erase(bodies.begin() + i);
			}

			if (i > erasedID)
				--bodies[i]->storageIndexID;
		}
		--storageIndexCounter;
	}

	PickResult CollisionWorld::getFirstPickResult(Ray ray)
	{
		std::vector<PickResult> result;

		//test Broadphase
		broadphase->getPotentialIntersectsWithRay(ray, result);

		//sort by Distance
		std::sort(result.begin(), result.end(), [](PickResult &a, PickResult &b) {return a.distance < b.distance; });

		//test Object
		for (int i = 0; i < result.size(); i++)
		{
			if (dispatcher->generateRayIntersection(ray, result[i]))
			{
				return result[i];
			}
		}

		return PickResult();	//no hit
	}

	void CollisionWorld::updateData()
	{
		tick_counter++;
		broadphase->update();
	}




	/*** Dynamics World ***/

	DynamicsWorld::DynamicsWorld(Broadphase* broadphase, Dispatcher* dispatcher, SoftBodySolver* sb_solver, glm::vec3 gravity, float air_drag) 
		: CollisionWorld(broadphase, dispatcher)
	{
		this->gravity = gravity;
		this->air_drag = air_drag;
		this->sb_solver = sb_solver;
	}

	DynamicsWorld::~DynamicsWorld()
	{
		// delete solver
		delete sb_solver;
	}

	// simulation tick
	void DynamicsWorld::stepWorld(float duration)
	{
		// siehe docs/PhysicsAllgemein.pdf
		for (int i = 0; i < bodies.size(); i++)
		{
			if (bodies[i]->type == body_index::SOFTBODY)
			{
				SoftBody* body = static_cast<SoftBody*>(bodies[i]);
				sb_solver->intermediatePositions(body, gravity, duration);
			}	
		}

		broadphase->update();
		// Broadphase, Erkennung überlappender Paare
		broadphase->generatePotentialContacts();

		// Dispatcher, Paare übergeben, Narrowphase ausführen
		dispatcher->generateCollisions(broadphase->potentialContacts, gravity, air_drag, duration, tick_counter);
		// Solver, Kollisionen verarbeiten
		// flags / sleeping state? -> Optimierung
		//solver->processCollisions(...);
		for (int i = 0; i < bodies.size(); i++)
		{
			if (bodies[i]->type == body_index::SOFTBODY)
			{
				SoftBody* body = static_cast<SoftBody*>(bodies[i]);
				sb_solver->processCollision(body, gravity, duration);
			}
			else if (bodies[i]->type == body_index::RIGIDBODY_DYNAMIC)
			{
				// Solve collisions for dynamic rigid bodies (kinematic rigid bodies may register collisions, but do not react to forces)
				// RigidBody* body = static_cast<RigidBody*>(bodies[i]);
				// rb_solver->processCollision(body, duration);
			}
			else
			{
				// weitere Moeglichkeiten
			}
		}
		// TODO: Performance verbessern durch hash map! Hash-Funktion:
		// int64_t hash = ((int64_t)(body))^((int64_t)(body))
		broadphase->potentialContacts.clear();

		//broadphase->update();

		++tick_counter;
	}

	void DynamicsWorld::renderDebugInfos()
	{
		for (int i = 0; i < bodies.size(); ++i)
		{
			if (bodies[i]->type == body_index::SOFTBODY)
			{
				auto sb = static_cast<SoftBody*>(bodies[i]);

				float* ddata = new float[sb->particles.size() * 3];
				

				for (int i = 0; i < sb->particles.size(); i++)
				{
					ddata[i * 3 + 0] = sb->particles[i]->goal_pos.x;
					ddata[i * 3 + 1] = sb->particles[i]->goal_pos.y;
					ddata[i * 3 + 2] = sb->particles[i]->goal_pos.z;

				}

				glPointSize(4.0f);
				glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
				glEnable(GL_POINT_SMOOTH);
				render::IDebugRender::getDebugRender()->drawData(ddata, sb->particles.size(), GL_POINTS, 0, sb->particles.size(), glm::vec4(1.0, 0.0, 0.0, 1.0));
				

				delete[] ddata;

				float comdata[3];

				comdata[0] = sb->current_com.x;
				comdata[1] = sb->current_com.y;
				comdata[2] = sb->current_com.z;
				
				glDisable(GL_DEPTH_TEST);

				render::IDebugRender::getDebugRender()->drawData(comdata, 3, GL_POINTS, 0, 3, glm::vec4(1.0, 1.0, 0.0, 1.0));

				glEnable(GL_DEPTH_TEST);

				

				glDisable(GL_POINT_SMOOTH);
			}
		}
	}

	void DynamicsWorld::applyForce(CollisionBody * body, glm::vec3 dir, float value)
	{
		SoftBody * sb = static_cast<SoftBody *>(body);

		for (int i = 0; i < sb->particles.size(); i++)
		{
			sb->particles[i]->applied_force += (dir * value) * sb->particles[i]->mass;
		}
	}
}