#pragma once

#include "PotentialContact.h"
#include "BVHNode.h"
#include <EngineCore\IDebugRender.h>
#include "RayHelper.h"

namespace physics
{
	class Broadphase
	{
	public:
		unsigned int contactLimit;
		std::vector<PotentialContact> potentialContacts;

	private:		
		unsigned int contactCount;
		BVHNode* root;

	public:
		Broadphase(unsigned int contactLimit = 2048);
		~Broadphase();

		void addBody(CollisionBody* body);
		void removeBody(CollisionBody* body);

		void generatePotentialContacts() { contactCount = root->getPotentialContacts(potentialContacts, contactLimit); }	

		void drawDebugData() { root->drawDebugData(); }

		void getPotentialIntersectsWithRay(Ray ray, std::vector<PickResult> &result);

		void update();

	private:
		BVHNode* search(CollisionBody* body, BVHNode* node);
		void test(BVHNode * node);
	};
}