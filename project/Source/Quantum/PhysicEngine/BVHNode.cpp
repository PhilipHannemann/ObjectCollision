#include "stdafx.h"
#include "BVHNode.h"
#include "BroadphaseAABB.h"
#include <EngineCore\Logger.h>
#include "PhysicsWorld.h"
#include <EngineCore\BaseExceptions.h>

namespace physics
{
	BVHNode::BVHNode(CollisionBody* body, BroadphaseAABB &boundingVolume, BVHNode* parent)
	{
		children[0] = children[1] = NULL;
		this->parent = parent;
		this->boundingVolume = boundingVolume;		
		//this->boundingVolume.owner = this;
		this->body = body;
		original_volume = boundingVolume.enclosedVolume;
	}

	BVHNode::~BVHNode()
	{
		if (isRoot)
			parent = NULL;

		if (parent != NULL)
		{
			removeUnsafe();
			parent->recalcBoundingVolume();
		}

		if (children[0] != NULL)
		{
			children[0]->parent = NULL;
			delete children[0];
		}

		if (children[1] != NULL)
		{
			children[1]->parent = NULL;
			delete children[1];
		}
	}

	unsigned int BVHNode::getPotentialContacts(std::vector<PotentialContact>& contacts, const unsigned int limit)
	{
		if (isLeaf() || limit == 0) return 0;

		return children[0]->getPotentialContactsWith(children[1], contacts, limit);
	}

	unsigned int BVHNode::getPotentialContactsWith(const BVHNode* other, std::vector<PotentialContact>& contacts, const unsigned int limit)
	{
		if (limit == 0)
			return 0;

		if (isLeaf() && other->isLeaf())
		{
			PotentialContact potentialContact;

			potentialContact.body[0] = body;
			potentialContact.body[1] = other->body;

			contacts.push_back(potentialContact);

			return 1;
		}

		int count = 0;

		if (overlaps(other))
		{

			if (other->isLeaf() || !isLeaf() && boundingVolume.enclosedVolume >= other->boundingVolume.enclosedVolume)
			{
				count += children[0]->getPotentialContactsWith(other, contacts, limit);

				if (limit > count)
				{
					count += children[1]->getPotentialContactsWith(other, contacts, limit - count);
				}
				else
				{
					return count;
				}
			}
			else
			{
				count += getPotentialContactsWith(other->children[0], contacts, limit);

				if (limit > count)
				{
					count += getPotentialContactsWith(other->children[1], contacts, limit - count);
				}
				else
				{
					return count;
				}
			}

		}

		if (/*!overlaps(other)*/true)
		{
			
			if (!isLeaf())
				count += children[0]->getPotentialContactsWith(children[1], contacts, limit);

			if (!other->isLeaf())
				count += other->children[0]->getPotentialContactsWith(other->children[1], contacts, limit);

			return count;
		}
	}

	void BVHNode::insertBody(CollisionBody* newBody, BroadphaseAABB& newVolume)
	{
		if (isLeaf())
		{
			children[0] = new BVHNode(body, boundingVolume, this);
			//body->broadphaseAABB.owner = children[0];

			children[1] = new BVHNode(newBody, newVolume, this);
			//newBody->broadphaseAABB.owner = children[1];

			this->body = NULL;

			recalcBoundingVolume(false);

			original_volume = boundingVolume.enclosedVolume;
		}
		else
		{
			if (children[0]->boundingVolume.getGrowth(newVolume) < children[1]->boundingVolume.getGrowth(newVolume))
				children[0]->insertBody(newBody, newVolume);
			else
				children[1]->insertBody(newBody, newVolume);		

			recalcBoundingVolume(false);
			original_volume = boundingVolume.enclosedVolume;
		}
		
	}

	void BVHNode::drawDebugData()
	{
		drawBoundingVolume();
		
		if (children[0])
			children[0]->drawDebugData();

		if (children[1])
			children[1]->drawDebugData();
	}

	BVHNode::RIDResult BVHNode::getRayIntersectDistance(Ray ray)
	{
		glm::vec3 inv = 1.0f / ray.dir;

		glm::vec3 t1 = (boundingVolume.min - ray.start)*inv;
		glm::vec3 t2 = (boundingVolume.max - ray.start)*inv;

		float tmin = glm::min(t1.x, t2.x);
		float tmax = glm::max(t1.x, t2.x);

		tmin = glm::max(tmin, glm::min(t1.y, t2.y));
		tmax = glm::min(tmax, glm::max(t1.y, t2.y));

		tmin = glm::max(tmin, glm::min(t1.z, t2.z));
		tmax = glm::min(tmax, glm::max(t1.z, t2.z));

		return RIDResult(tmax >= tmin, tmin, tmax);
	}

	void BVHNode::getPotentialIntersectsWithRay(Ray &ray, std::vector<PickResult>& result)
	{
		RIDResult f = getRayIntersectDistance(ray);

		if (!f.hit || (f.tmin < 0.0f && f.tmax < 0.0f))
			return;

		if (isLeaf() && f.tmin >= 0.0f)
			result.push_back(PickResult(f.tmin, body));
		else
		{
			if (children[0] != nullptr) children[0]->getPotentialIntersectsWithRay(ray, result);
			if (children[1] != nullptr) children[1]->getPotentialIntersectsWithRay(ray, result);
		}
	}

	void BVHNode::recalcBoundingVolume(bool recursive)
	{
		if (!isLeaf())
		{
			float minX = children[0]->boundingVolume.min.x < children[1]->boundingVolume.min.x ? children[0]->boundingVolume.min.x : children[1]->boundingVolume.min.x;
			float minY = children[0]->boundingVolume.min.y < children[1]->boundingVolume.min.y ? children[0]->boundingVolume.min.y : children[1]->boundingVolume.min.y;
			float minZ = children[0]->boundingVolume.min.z < children[1]->boundingVolume.min.z ? children[0]->boundingVolume.min.z : children[1]->boundingVolume.min.z;

			float maxX = children[0]->boundingVolume.max.x > children[1]->boundingVolume.max.x ? children[0]->boundingVolume.max.x : children[1]->boundingVolume.max.x;
			float maxY = children[0]->boundingVolume.max.y > children[1]->boundingVolume.max.y ? children[0]->boundingVolume.max.y : children[1]->boundingVolume.max.y;
			float maxZ = children[0]->boundingVolume.max.z > children[1]->boundingVolume.max.z ? children[0]->boundingVolume.max.z : children[1]->boundingVolume.max.z;

			glm::vec3 min(minX, minY, minZ);
			glm::vec3 max(maxX, maxY, maxZ);

			this->boundingVolume.recalculateAABB(min, max);
		}
		else
		{
			this->boundingVolume = this->body->recalculateAABB();
		}

		if (recursive && parent != NULL)
			parent->recalcBoundingVolume();
	}

	bool BVHNode::updateNode(std::vector<BVHNode *> &faults)
	{

		bool update = false;

		if (isLeaf())
		{
			if (body->last_change >= DynamicsWorld::getCurrentTick())
			{
				recalcBoundingVolume(false);
				return true;
			}
			return false;
		}

		if (children[0] != nullptr) 
			update = children[0]->updateNode(faults) || update;
		if (children[1] != nullptr) 
			update = children[1]->updateNode(faults) || update;

		if (update)
		{
			if (children[0]->isLeaf() || children[1]->isLeaf())
			{
				if (isNodeFaulty())
				{
					//faults++;
					//core::Logger::info("Fault with: " + std::to_string(boundingVolume.enclosedVolume) + " : " + std::to_string(original_volume));

					BVHNode *nn = children[0]->isLeaf() ? children[0] : children[1];

					nn->removeFromParent();

					faults.push_back(nn);
				}
			}
			
			
			recalcBoundingVolume(false);
		}

		if (isRoot)
		{
			//core::Logger::info("Faulty total: " + std::to_string(faults));

			for (int i = 0; i < faults.size(); i++)
			{
				insertBody(faults[i]->body, faults[i]->boundingVolume);
				delete faults[i];
			}

			faults.clear();
		}


		return update;
	}

	void BVHNode::drawBoundingVolume()
	{
		glm::vec4 color;
		if (isLeaf())
			color = glm::vec4(1.f, 0.f, 0.f, 1.f);
		else
			color = glm::vec4(0.f, 1.f, 0.f, 1.f);

		float* loop = new float[4 * 3]{
			boundingVolume.min.x, boundingVolume.min.y, boundingVolume.min.z,
			boundingVolume.min.x, boundingVolume.min.y, boundingVolume.max.z,
			boundingVolume.max.x, boundingVolume.min.y, boundingVolume.max.z,
			boundingVolume.max.x, boundingVolume.min.y, boundingVolume.min.z
		};
		
		render::IDebugRender::getDebugRender()->drawData(loop, 4, GL_LINE_LOOP, 0, 4, color);

		loop[0] = boundingVolume.min.x; loop[1] = boundingVolume.max.y; loop[2] = boundingVolume.min.z;
		loop[3] = boundingVolume.min.x; loop[4] = boundingVolume.max.y; loop[5] = boundingVolume.max.z;
		loop[6] = boundingVolume.max.x; loop[7] = boundingVolume.max.y; loop[8] = boundingVolume.max.z;
		loop[9] = boundingVolume.max.x; loop[10] = boundingVolume.max.y; loop[11] = boundingVolume.min.z;

		render::IDebugRender::getDebugRender()->drawData(loop, 4, GL_LINE_LOOP, 0, 4, color);

		float* lines = new float[8 * 3]{
			boundingVolume.min.x, boundingVolume.min.y, boundingVolume.min.z,
			boundingVolume.min.x, boundingVolume.min.y, boundingVolume.max.z,
			boundingVolume.min.x, boundingVolume.max.y, boundingVolume.min.z,
			boundingVolume.min.x, boundingVolume.max.y, boundingVolume.max.z,
			boundingVolume.max.x, boundingVolume.max.y, boundingVolume.min.z,
			boundingVolume.max.x, boundingVolume.max.y, boundingVolume.max.z,
			boundingVolume.max.x, boundingVolume.min.y, boundingVolume.min.z,
			boundingVolume.max.x, boundingVolume.min.y, boundingVolume.max.z
		};

		render::IDebugRender::getDebugRender()->drawData(lines, 8, GL_LINES, 0, 8, color);

		delete[] loop;
		delete[] lines;
	}

	void BVHNode::removeFromParent()
	{
		if (!isLeaf())
		{
			throw FatalException("Only Removing Leafs supported!");
		}
		else if(parent == NULL)
		{
			throw FatalException("No Parent Available!");
		}
		else
		{
			removeUnsafe();
			parent = NULL;
		}
	}

	void BVHNode::trace(std::string str)
	{
		//drawBoundingVolume();
		core::Logger::log(str + std::to_string(boundingVolume.enclosedVolume) + " : " + std::to_string(original_volume) /*+ " : " + (body != NULL ? std::to_string(body->storageIndexID) : " ")*/);
		if (children[0] != nullptr)
			children[0]->trace(str + "          ");
		if (children[1] != nullptr)
			children[1]->trace(str + "          ");
	}

	void BVHNode::removeUnsafe()
	{
		BVHNode* sibling = NULL;

		if (this == parent->children[0])
			sibling = parent->children[1];
		else
			sibling = parent->children[0];

		parent->boundingVolume = sibling->boundingVolume;
		parent->body = sibling->body;
		//parent->boundingVolume.owner = parent;

		parent->children[0] = sibling->children[0];
		if (parent->children[0] != NULL)
			parent->children[0]->parent = parent;

		parent->children[1] = sibling->children[1];
		if (parent->children[1] != NULL)
			parent->children[1]->parent = parent;

		sibling->parent = NULL;
		sibling->body = NULL;
		sibling->children[0] = NULL;
		sibling->children[1] = NULL;
		//sibling->boundingVolume.owner = NULL;
		delete sibling;
	}

	bool BVHNode::isNodeFaulty()
	{
		//core::Logger::warning(std::to_string(children[0]->boundingVolume.getGrowth(children[1]->boundingVolume)));

		return false;

		float delta = boundingVolume.enclosedVolume / original_volume;

		return  delta > 2.0f || delta < 0.5f;
	}

	BVHNode* BVHNode::getRootNode()
	{
		if (parent == NULL)
			return this;

		BVHNode* root = parent;
		while (root->parent != NULL)
			root = root->parent;

		return parent;
	}
}