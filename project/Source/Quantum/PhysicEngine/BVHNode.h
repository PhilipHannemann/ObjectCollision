#pragma once

#include "BroadphaseAABB.h"
#include "CollisionBody.h"
#include "PotentialContact.h"
#include <EngineCore\IDebugRender.h>
#include "RayHelper.h"

namespace physics
{
	class BroadphaseAABB;

	class BVHNode
	{
	private:
		struct RIDResult
		{
			float tmin;
			float tmax;
			bool hit;

			RIDResult(bool h, float min, float max) : hit(h), tmin(min), tmax(max) {}
		};

	public:
		BVHNode* children[2];
		BVHNode* parent;
		BroadphaseAABB boundingVolume;
		CollisionBody* body;
		bool isRoot;
		
		float original_volume;

	public:
		BVHNode(CollisionBody* body, BroadphaseAABB &boundingVolume, BVHNode* parent);
		~BVHNode();

		bool isLeaf() const { return (body != NULL); }

		bool overlaps(const BVHNode* other) const { return boundingVolume.overlaps(other->boundingVolume); }

		unsigned int getPotentialContacts(std::vector<PotentialContact>& contacts, const unsigned int limit);
		unsigned int getPotentialContactsWith(const BVHNode* other, std::vector<PotentialContact>& contacts, const unsigned int limit);
		
		void insertBody(CollisionBody* newBody, BroadphaseAABB& newVolume);

		void drawDebugData();

		void getPotentialIntersectsWithRay(Ray &ray, std::vector<PickResult> &result);

		void recalcBoundingVolume(bool recursive = true);

		bool updateNode(std::vector<BVHNode *> &faults);

		void removeFromParent();

		void trace(std::string str);

	private:	
		void drawBoundingVolume();
		RIDResult getRayIntersectDistance(Ray ray);
		inline void removeUnsafe();
		bool isNodeFaulty();
		BVHNode* getRootNode();
	};
}