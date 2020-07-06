#pragma once

#include <glm\glm.hpp>
#include "IVertexData.h"
#include <vector>

namespace loader
{

	struct Triangle
	{
		glm::vec3 a;
		glm::vec3 b;
		glm::vec3 c;

		Triangle(glm::vec3 &_a, glm::vec3 &_b, glm::vec3 &_c): a(_a), b(_b), c(_c) {}
	};

	struct IndexedTriagnles
	{
		struct IndexedTriagnlesIndex
		{
			int vertex;
			int normal;
			int texcoord;
		} *indices;

		int indices_size;

		struct IndexedTriagnlesData
		{
			float *vertex;
			int vertex_length;
			float *normal;
			int normal_length;
			float *texcoord;
			int texcoord_length;
		} data;

		~IndexedTriagnles() 
		{
			delete[] indices;

			delete[] data.vertex;
			delete[] data.texcoord;
			delete[] data.normal;
		}
	};


	/*Interface for a (Vertex)-Model*/
	class IModel 
	{
	public:
		/*returns unordered 3D Points representing the mesh*/
		virtual std::vector<glm::vec3> getPointCloud() = 0;

		/*returns unordered 3D Points representing the mesh, prevents multiple Points on the same position*/
		virtual std::vector<glm::vec3> getUniquePointCloud() = 0;
	
		/*returns all Triangles of the mesh, representet by 3x 3D Points per Triangles*/
		virtual std::vector<glm::vec3> getTriangles() = 0;

		/*returns all Triangles of the mesh, representet by 3 x 3(3D Point) Indices per Triangles and a non-minimal set of the indexed Vertices*/
		virtual IndexedTriagnles* getIndexedTriagnles() = 0;

		/*Stores Vertices(including all vertex attached data) into GPU Buffer(s). Using a not standardized format*/
		virtual void createRenderData(render::IVertexData* vd) = 0;

		
		/*Helper: read's (copy) an Triangle from continues float array*/
		static inline Triangle getTriangle(glm::vec3 *data, int i)
		{ 
			return Triangle(data[i * 3 + 0], data[i * 3 + 1], data[i * 3 + 2]);
		}

		//virtual btAlignedObjectArray<btVector3> getUniquePointCloudBT() = 0;

	};


}