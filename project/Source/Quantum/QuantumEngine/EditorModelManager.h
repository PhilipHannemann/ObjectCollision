#pragma once
#include <vector>
#include "RenderInstance.h"
#include "ShaderParts.h"
#include "EditorParts.h"
#include "RenderCache.h"
#include "EditorParts.h"
#include <PhysicEngine\PhysicsWorld.h>

namespace editor
{
	class PartEntry
	{
	public:
		render::RenderInstance<ShaderParts::ShaderPartsSSBO> *ri;
		PartType type;
		glm::ivec3 pos;
		glm::vec4 color;
		physics::CollisionBody *collision_body;
	};


	class EditorModelManager
	{
	private:
		EditorParts *templates;
		physics::CollisionWorld *world;
		std::vector<PartEntry> parts;

	public:
		EditorModelManager(render::Shader *s, physics::CollisionWorld *physics);
		~EditorModelManager();

		void addPart(PartType type, glm::ivec3 pos);//TODO
		void removePart(int id);

		void prepareRender(render::RenderCache *cache);

		//never store result!!!
		PartEntry* getPartRef(int id) { return &parts[id]; }
	
	
	};




}