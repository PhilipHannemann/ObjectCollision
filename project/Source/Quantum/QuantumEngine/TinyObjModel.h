#pragma once
#include <EngineCore/IModel.h>
#include <string>
#include <EngineCore/tiny_obj_loader.h>

namespace loader
{

	class TinyObjModel : public IModel
	{
	protected:
		size_t num_s_vert;
		tinyobj::attrib_t *attrib;
		std::vector<tinyobj::shape_t> *shapes;
		std::vector<tinyobj::material_t> *materials;

	public:
		TinyObjModel(std::string dir, std::string name);
		virtual ~TinyObjModel();

	public:
		virtual std::vector<glm::vec3> getPointCloud() override;

		virtual std::vector<glm::vec3> getUniquePointCloud() override;

		virtual std::vector<glm::vec3> getTriangles() override;

		virtual IndexedTriagnles* getIndexedTriagnles() override;

		virtual void createRenderData(render::IVertexData* vd) override;

		//virtual btAlignedObjectArray<btVector3> getUniquePointCloudBT() override { return btAlignedObjectArray<btVector3>(); }
	};

}