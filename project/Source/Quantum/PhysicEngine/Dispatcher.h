#pragma once
#include <vector>
#include "AlgorithmBase.h"
#include "PotentialContact.h"
#include "ARayIntersectionAlgorithm.h"
#include "SBRBCollisionAlgorithm.h"
#include "SBSphereTreeCollisionAlgorithm.h"

namespace physics
{
	class Dispatcher
	{
	private:
		std::vector< std::vector<ACollisionAlgorithm*> > collision_algorithms;
		std::vector<ARayIntersectionAlgorithm*> rayintersect_algorithms;

	public:
		Dispatcher();
		~Dispatcher();

		void addCollisionAlgorithm(BodyIndex a, BodyIndex b, ACollisionAlgorithm* algo);
		void addRayIntersectionAlgorithm(BodyIndex a, ARayIntersectionAlgorithm* algo);

		void generateCollisions(std::vector<PotentialContact>& potentialContacts, const glm::vec3 &gravity, const float &air_drag, const float &time_delta, int64_t simulation_tick_counter);
		bool generateRayIntersection(Ray ray, PickResult &broadphaseResult);
	};
}