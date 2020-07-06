#pragma once
#include "SphereTreeCollisionHull.h"

namespace physics
{
	class SphereTreeBuilder
	{
	public:
		//3 indices from indexed_mesh represent 1 Triangle of the mesh. vertices hold the vertex data (x,y,z) matching the indices from indexed_mesh. vertices has the same length and order like the Softbody particle list.
		static SphereTreeCollisionHull* generateCollisionHull(const std::vector<unsigned int> &indexed_mesh, const std::vector<glm::vec3> &vertices);

	private:
		static void coverSurface(const std::vector<unsigned int> &indexed_mesh, const std::vector<glm::vec3> &vertices, std::vector<SphereTreeNode*> &leaf_nodes);

		static SphereTreeNode* constructTree(std::vector<SphereTreeNode*> &leaves);

		static void insertNodes(SphereTreeNode* &root, SphereTreeNode* node, std::vector<SphereTreeNode*> &leaves);

		static void refitSpheres(SphereTreeNode* root, const std::vector<glm::vec3> &vertices);

		static void recalcSphere(SphereTreeNode* node);

		static float getGrowth(SphereTreeNode* node, SphereTreeNode* leaf);

		static SphereTreeNode* constructLayers(const std::vector<SphereTreeNode*> &origin_set, bool &left_subset_used_last);

		static void refitLayers(const std::vector<glm::vec3> &vertices, SphereTreeNode* node);

		static void generateBoundingBox(const std::vector<SphereTreeNode*> &nodes, glm::vec3 &min, glm::vec3 &max);

		static void findLongestAxis(const glm::vec3 &min, const glm::vec3 &max, int &longest_dim);

		static void generateSphereFromChildren(SphereTreeNode* left_child, SphereTreeNode* right_child, glm::vec3 &center, float &radius);

		static void generateSphereFromLeaves(SphereTreeNode* left_child, SphereTreeNode* right_child, glm::vec3 &center, float &radius);

		static void findLeafSpheres(SphereTreeNode* left_child, SphereTreeNode* right_child, std::vector<SphereTreeNode*> &leaves);
	};
}