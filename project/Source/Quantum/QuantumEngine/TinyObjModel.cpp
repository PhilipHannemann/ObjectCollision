#include "stdafx.h"
#define _SCL_SECURE_NO_WARNINGS

#include "TinyObjModel.h"
#include <string>
#include <vector>
#include <EngineCore/tiny_obj_loader.h>
#include <EngineCore/Logger.h>
#include <EngineCore/BaseExceptions.h>
#include "TinyObjVertexData.h"

namespace loader
{

	TinyObjModel::TinyObjModel(std::string dir, std::string name)
	{
		attrib = new tinyobj::attrib_t();
		shapes = new std::vector<tinyobj::shape_t>();
		materials = new std::vector<tinyobj::material_t>();

		std::string err;
		bool ret = tinyobj::LoadObj(attrib, shapes, materials, &err, (dir + name).c_str(), dir.c_str(), false);

		core::Logger::info("Model loaded: " + err);

		if (!ret)
		{
			throw ModelLoadingException("Reading OBJ file failed: " + err);
		}

		return;
	}

	TinyObjModel::~TinyObjModel()
	{
		delete attrib;
		delete shapes;
		delete materials;
	}

	std::vector<glm::vec3> TinyObjModel::getPointCloud()
	{
		return getTriangles();
	}

	struct Pt { float v[3]; };

	bool f3comp(Pt i, Pt j) { return (i.v[0] < j.v[0]) && (i.v[1] < j.v[1]) && (i.v[2] < j.v[2]); }

	#define Epsilon 0.0001f 

	std::vector<glm::vec3> TinyObjModel::getUniquePointCloud()
	{
		std::vector<glm::vec3> data;

		for (int i = 0; i < attrib->vertices.size(); i += 3)
		{
			bool addit = true;

			for (int j = 0; j < data.size(); j)
			{
				if (abs(attrib->vertices[i + 0] - data[j].x) < Epsilon && abs(attrib->vertices[i + 1] - data[j].y) < Epsilon && abs(attrib->vertices[i + 2] - data[j].z) < Epsilon)
				{
					addit = false;
					break;
				}
			}	

			if (addit)
			{
				data.push_back(glm::vec3(attrib->vertices[i + 0], attrib->vertices[i + 1], attrib->vertices[i + 2]));
			}

		}

		return data;
	}

	std::vector<glm::vec3> TinyObjModel::getTriangles()
	{
		std::vector<glm::vec3> data;

		for (auto sh = shapes->begin(); sh != shapes->end(); sh++)
		{
			for (auto it = sh->mesh.indices.begin(); it != sh->mesh.indices.end(); it++)
			{
				int a = (*(it)).vertex_index;

				data.push_back(glm::vec3(attrib->vertices[3 * a + 0], attrib->vertices[3 * a + 1], attrib->vertices[3 * a + 2]));
			}
		}

		return data;
	}

	IndexedTriagnles * TinyObjModel::getIndexedTriagnles()
	{
		IndexedTriagnles *res = new IndexedTriagnles();

		int is = 0;

		int anz = 0;

		for (auto sh = shapes->begin(); sh != shapes->end(); sh++)
		{
			anz += sh->mesh.indices.size();
		}

		res->indices = new IndexedTriagnles::IndexedTriagnlesIndex[anz];

		for (auto sh = shapes->begin(); sh != shapes->end(); sh++)
		{
			for (int i = 0; i < (*sh).mesh.indices.size(); i++)
			{
				res->indices[is].vertex = (*sh).mesh.indices[i].vertex_index;
				res->indices[is].normal = (*sh).mesh.indices[i].normal_index;
				res->indices[is].texcoord = (*sh).mesh.indices[i].texcoord_index;
				is++;
			}
		}

		res->indices_size = is/3;

		res->data.vertex_length = attrib->vertices.size();
		res->data.vertex = new float[attrib->vertices.size()];
		res->data.normal_length = attrib->normals.size();
		res->data.normal = new float[attrib->normals.size()];
		res->data.texcoord_length = attrib->texcoords.size();
		res->data.texcoord = new float[attrib->texcoords.size()];

		for (int i = 0; i < attrib->vertices.size(); i++)
		{
			res->data.vertex[i] = attrib->vertices[i];
		}

		for (int i = 0; i < attrib->normals.size(); i++)
		{
			res->data.normal[i] = attrib->normals[i];
		}

		for (int i = 0; i < attrib->texcoords.size(); i++)
		{
			res->data.texcoord[i] = attrib->texcoords[i];
		}

		//res->data_size = attrib->vertices.size()/3;

		return res;
	}

	void TinyObjModel::createRenderData(render::IVertexData* vd)
	{
		assert(dynamic_cast<TinyObjVertexData*>(vd) != nullptr);

		TinyObjVertexData *vdata = (TinyObjVertexData*)vd;

		vdata->count = (GLsizei) shapes->at(0).mesh.indices.size();
		vdata->buffer = new float[vdata->count * 8];

		// Loop over shapes
		for (size_t s = 0; s < shapes->size(); s++)
		{
			// Loop over faces(polygon)
			for (size_t v = 0; v < shapes->at(s).mesh.indices.size(); v++)
			{
				//int fv = shapes[s].mesh.num_face_vertices[v];

				tinyobj::index_t idx = shapes->at(s).mesh.indices[v];
				vdata->buffer[8 * v + 0] = attrib->vertices[3 * idx.vertex_index + 0];
				vdata->buffer[8 * v + 1] = attrib->vertices[3 * idx.vertex_index + 1];
				vdata->buffer[8 * v + 2] = attrib->vertices[3 * idx.vertex_index + 2];
				vdata->buffer[8 * v + 3] = attrib->normals[3 * idx.normal_index + 0];
				vdata->buffer[8 * v + 4] = attrib->normals[3 * idx.normal_index + 1];
				vdata->buffer[8 * v + 5] = attrib->normals[3 * idx.normal_index + 2];
				vdata->buffer[8 * v + 6] = attrib->texcoords[2 * idx.texcoord_index + 0];
				vdata->buffer[8 * v + 7] = attrib->texcoords[2 * idx.texcoord_index + 1];
			
				// per-face material
				//shapes[s].mesh.material_ids[f];
			}
		}
		vdata->setup();
	}

}