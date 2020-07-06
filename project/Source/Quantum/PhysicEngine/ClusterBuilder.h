#pragma once
#include "ClusterMesh.h"
#include <glm\glm.hpp>

namespace physics
{
	class ClusterBuilder
	{

	public:
		//3 indices from indexed_mesh represent 1 Triangle of the mesh. vertices hold the vertex data (x,y,z) matching the indices from indexed_mesh. vertices has the same length and order like the Softbody particle list.
		ClusterMesh* generateClusters(const std::vector<unsigned int> &indexed_mesh, const std::vector<glm::vec3> &vertices);

	};
}