#pragma once
#include <vector>
#include "stdafx.h"

namespace physics
{
	struct Cluster
	{
		std::vector<unsigned int> id;
	};

	struct ClusterMesh
	{
		std::vector<Cluster> mesh;

		ClusterMesh* copy() 
		{
			return new ClusterMesh(*this);
		}
	};

}