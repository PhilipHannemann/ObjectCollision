#include "stdafx.h"
#include "SoftBodyModelWrapper.h"
#include "RenderCache.h"
#include <PhysicEngine\SphereTreeBuilder.h>

namespace loader
{

#define Epsilon 0.0001f 

	int SoftBodyModelWrapper::findOrAdd(glm::vec3 vert)
	{
		int id = -1;

		//find equal vertex
		for (int j = 0; j < data.size(); j++)
		{
			if (abs(vert.x - data[j].x) < Epsilon && abs(vert.y - data[j].y) < Epsilon && abs(vert.z - data[j].z) < Epsilon)
			{
				return j;
			}
		}

		//add it as new vertex
		data.push_back(vert);
		id = (int) data.size() - 1;

		vao->adjlist.push_back(std::vector<int>());

		return id;
	}

	void SoftBodyModelWrapper::addAdjazent(int source, int dest1, int dest2)
	{
		//check a1 is in adjazent list
		/*bool found = false;
		for (int j = 0; j < vdata->adjlist[source].size(); j++)
		{
		if (vdata->adjlist[source][j] == dest)
		{
		return;
		}
		}*/

		vao->adjlist[source].push_back(dest1);
		vao->adjlist[source].push_back(dest2);
	}

	SoftBodyModelWrapper::SoftBodyModelWrapper(render::Shader *s, IModel * imod)
	{
		vao = new SoftBodyVertexData(s);

		std::vector<unsigned int> index;
		std::vector<float> full;

		IndexedTriagnles *tri = imod->getIndexedTriagnles();

		int p = 0;

#ifdef _DEBUG

		for (int i = 0; i < tri->data.vertex_length/3; i++)
		{
			data.push_back(glm::vec3(tri->data.vertex[3 * i + 0], tri->data.vertex[3 * i + 1], tri->data.vertex[3 * i + 2]));

			vao->adjlist.push_back(std::vector<int>());
		}

#endif // DEBUG

		for (int i = 0; i < tri->indices_size*3; i+=3)
		{
			//tinyobj::index_t idx[3] = { shapes->at(s).mesh.indices[v + 0], shapes->at(s).mesh.indices[v + 1], shapes->at(s).mesh.indices[v + 2] };

#ifdef _DEBUG

			int id[3] = { tri->indices[i + 0].vertex, tri->indices[i + 1].vertex, tri->indices[i + 2].vertex };

			addAdjazent(id[0], id[1], id[2]);
			addAdjazent(id[1], id[2], id[0]);
			addAdjazent(id[2], id[0], id[1]);

			for (int t = 0; t < 3; t++)
			{
				index.push_back(id[t]);
				full.push_back(tri->data.texcoord[2 * tri->indices[i + t].vertex + 0]);
				full.push_back(tri->data.texcoord[2 * tri->indices[i + t].vertex + 1]);
			}


#else




			glm::vec3 v1(tri->data.vertex[3 * tri->indices[i + 0].vertex + 0], tri->data.vertex[3 * tri->indices[i + 0].vertex + 1], tri->data.vertex[3 * tri->indices[i + 0].vertex + 2]);
			glm::vec3 v2(tri->data.vertex[3 * tri->indices[i + 1].vertex + 0], tri->data.vertex[3 * tri->indices[i + 1].vertex + 1], tri->data.vertex[3 * tri->indices[i + 1].vertex + 2]);
			glm::vec3 v3(tri->data.vertex[3 * tri->indices[i + 2].vertex + 0], tri->data.vertex[3 * tri->indices[i + 2].vertex + 1], tri->data.vertex[3 * tri->indices[i + 2].vertex + 2]);
			
			glm::vec3 vert[3] = { v1, v2, v3 };
			
			int id[3] = { findOrAdd(v1), findOrAdd(v2), findOrAdd(v3) };

			addAdjazent(id[0], id[1], id[2]);
			addAdjazent(id[1], id[2], id[0]);
			addAdjazent(id[2], id[0], id[1]);

			for (int t = 0; t < 3; t++)
			{
				index.push_back(id[t]);
				full.push_back(tri->data.texcoord[2 * tri->indices[i + t].vertex + 0]);
				full.push_back(tri->data.texcoord[2 * tri->indices[i + t].vertex + 1]);
			}

#endif // DEBUG

			
		}

		//building Collision Hull (SBSphereTree)
		hull = physics::SphereTreeBuilder::generateCollisionHull(index, data);

		//doing convex decomp
		physics::ClusterBuilder *cluster_builder = new physics::ClusterBuilder();
		clusters = cluster_builder->generateClusters(index, data);
		delete cluster_builder;

		/*int *arr = new int[data.size()];

		for (int i = 0; i < data.size(); i++)
		{
			arr[i] = 0;
		}

		for (auto m : clusters->mesh)
		{
			std::string obj;
			obj = "NEW GROUP\no Cube\n";
			for (auto f : m.id)
			{
				arr[f] ++;
				obj += "v " + std::to_string(data[f].x) + " " + std::to_string(data[f].y) + " " + std::to_string(data[f].z) + " \n";
			}
			core::Logger::log(obj);
		}

		for (int j = 0; j < 20; j++)
		{
			int s = 0;
			for (int i = 0; i < data.size(); i++)
			{
				if (arr[i] == j)
					s++;
			}
			core::Logger::log("Coverage of " + std::to_string(j) + " detected for " + std::to_string(s) + " Verticies");
		}

		delete[] arr;*/

		vao->vertex_count = (GLsizei) data.size();

		vao->count = (GLsizei) index.size();
		vao->index_buffer = new GLuint[index.size()];
		memcpy_s(vao->index_buffer, index.size() * sizeof(unsigned int), index.data(), index.size() * sizeof(unsigned int));

		vao->static_buffer = new GLfloat[index.size() * 2];
		memcpy_s(vao->static_buffer, index.size() * 2 * sizeof(float), full.data(), index.size() * 2 * sizeof(float));

		vao->setup();

		stdro = new render::RenderObject(vao, (int)index.size() * 3, 0, 0);

		delete tri;

	}

	SoftBodyModelWrapper::~SoftBodyModelWrapper()
	{
		delete hull;
		delete clusters;
		data.clear();
		delete stdro;
		delete vao;	
	}

}