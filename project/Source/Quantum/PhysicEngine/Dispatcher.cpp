#include "stdafx.h"
#include "Dispatcher.h"
#include <EngineCore\Logger.h>
#include <EngineCore\Gobals.h>

namespace physics
{

	Dispatcher::Dispatcher()
	{
		int count = body_index::getBodyIndexCount();

		collision_algorithms.resize(count);

		for (int i = 0; i < count; i++)
		{
			collision_algorithms[i].resize(count);

			for (int j = 0; j < count; j++)
			{
				collision_algorithms[i][j] = nullptr;
			}
		}


		rayintersect_algorithms.resize(count);

		for (int i = 0; i < count; i++)
		{
			rayintersect_algorithms[i] = nullptr;
		}
	}

	Dispatcher::~Dispatcher()
	{
		for (int i = 0; i < collision_algorithms.size(); i++)
		{
			for (int j = 0; j < collision_algorithms[i].size(); j++)
			{
				SAFEDELETE(collision_algorithms[i][j]);
				collision_algorithms[i][j] = nullptr;
				collision_algorithms[j][i] = nullptr;
			}
		}

		for (int i = 0; i < rayintersect_algorithms.size(); i++)
		{
			SAFEDELETE(rayintersect_algorithms[i]);
			rayintersect_algorithms[i] = nullptr;
		}
	}

	void Dispatcher::addCollisionAlgorithm(BodyIndex a, BodyIndex b, ACollisionAlgorithm * algo)
	{
		if (a >= collision_algorithms.size() || b >= collision_algorithms.size())
		{
			core::Logger::error("Failed to add Algorithm for: " + std::to_string(a) + " vs " + std::to_string(b));
			return;
		}

		collision_algorithms[a][b] = algo;
		collision_algorithms[b][a] = algo;
	}

	void Dispatcher::addRayIntersectionAlgorithm(BodyIndex a, ARayIntersectionAlgorithm * algo)
	{
		if (a >= rayintersect_algorithms.size())
		{
			core::Logger::error("Failed to add Algorithm for: " + std::to_string(a));
			return;
		}

		rayintersect_algorithms[a] = algo;
	}

	void Dispatcher::generateCollisions(std::vector<PotentialContact>& potentialContacts, const glm::vec3 &gravity,const float &air_drag, const float &time_delta, int64_t simulation_tick_counter)
	{
		//hier potenzielle Parallelisierung möglich
		for (int i = 0; i < potentialContacts.size(); i++)
		{
			BodyIndex a = potentialContacts[i].body[0]->type;
			BodyIndex b = potentialContacts[i].body[1]->type;

			if (a >= collision_algorithms.size() || b >= collision_algorithms.size())
			{
				core::Logger::debug("No Collision Algorithm for: " + std::to_string(a) + " vs " + std::to_string(b));
				continue;
			}

			ACollisionAlgorithm *algo = collision_algorithms[a][b];

			if (algo != nullptr)
			{
				algo->testPair(potentialContacts[i].body[0], potentialContacts[i].body[1], gravity, air_drag, time_delta);
			}
			else
			{
				core::Logger::debug("No Collision Algorithm for: " + std::to_string(a) + " vs " + std::to_string(b));
			}
		}
	}

	bool Dispatcher::generateRayIntersection(Ray ray, PickResult & broadphaseResult)
	{
		if (broadphaseResult.body->type >= rayintersect_algorithms.size())
		{
			core::Logger::debug("No Ray Intersect Algorithm for: " + std::to_string(broadphaseResult.body->type));
			return false;
		}

		ARayIntersectionAlgorithm *algo = rayintersect_algorithms[broadphaseResult.body->type];

		if (algo != nullptr)
		{
			return algo->testRay(ray, broadphaseResult);
		}
		else
		{
			core::Logger::debug("No Ray Intersect Algorithm for: " + std::to_string(broadphaseResult.body->type));
		}

		return false;
	}
}
