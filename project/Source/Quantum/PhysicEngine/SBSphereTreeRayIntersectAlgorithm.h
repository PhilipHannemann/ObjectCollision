#pragma once
#include "ARayIntersectionAlgorithm.h"
#include "SoftBody.h"

namespace physics
{
	class SBSphereTreeRayIntersectionAlgorithm : public ARayIntersectionAlgorithm
	{
	public:
		int64_t simulation_tick_counter;
		bool debug_render;
		
		SBSphereTreeRayIntersectionAlgorithm() { simulation_tick_counter = 0; }
		virtual ~SBSphereTreeRayIntersectionAlgorithm() {}
		virtual bool testRay(Ray ray, PickResult &data) override;

	private:
		bool testNodes(SoftBody * body, SphereTreeNode * node, Ray & ray, PickResult & data, float &lowest);
		void updateNode(SoftBody * body, SphereTreeNode * node);
		void prepareTraversal(SoftBody * body);
		bool intersects(SphereTreeNode * nodeA, Ray & ray);
		bool checkParticles(SoftBody * body, SphereTreeNode * node, Ray & ray, PickResult & data, float &lowest);
		float calcANormLinear(SoftBody * body);
		float calcDeviationLinear(SoftBody * body);
	};
}