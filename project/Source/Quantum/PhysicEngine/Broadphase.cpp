#include "stdafx.h"
#include "Broadphase.h"

namespace physics
{
	Broadphase::Broadphase(unsigned int contactLimit)
	{
		this->contactLimit = contactLimit;
		root = NULL;
	}

	Broadphase::~Broadphase()
	{

	}

	void Broadphase::addBody(CollisionBody* body) 
	{ 
		if (root == NULL)
		{
			root = new BVHNode(body, body->recalculateAABB(), NULL);
			root->isRoot = true;
			//body->broadphaseAABB.owner = root;
		}
		else
		{
			root->insertBody(body, body->recalculateAABB());
		}
	}

	void Broadphase::removeBody(CollisionBody* body)
	{
		BVHNode* containingNode = NULL;
		containingNode = search(body, root);

		if (containingNode == root)
			root = NULL;

		if (containingNode != NULL) delete containingNode;
	}

	void Broadphase::getPotentialIntersectsWithRay(Ray ray, std::vector<PickResult> &result)
	{
		if (root != NULL)
			root->getPotentialIntersectsWithRay(ray, result);
	}

	void Broadphase::update()
	{
		if (root != NULL)
		{
			std::vector<BVHNode*> faults;
			root->updateNode(faults);
		}
	}

	BVHNode* Broadphase::search(CollisionBody* body, BVHNode* node)
	{
		if (node != NULL)
		{
			if (node->body == body) return node;
			else
			{
				BVHNode* temp = search(body, node->children[0]);
				if (temp == NULL) temp = search(body, node->children[1]);
				return temp;
			}
		}
		else return NULL;
	}

	void Broadphase::test(BVHNode * node)
	{
		if (node->children[0] != nullptr)
		{
			if (node->children[0]->parent != node)
			{
				throw std::runtime_error("Parent was wrong!");
			}
			test(node->children[0]);
		}
		if (node->children[1] != nullptr)
		{
			if (node->children[1]->parent != node)
			{
				throw std::runtime_error("Parent was wrong!");
			}
			test(node->children[1]);
		}
	}
}