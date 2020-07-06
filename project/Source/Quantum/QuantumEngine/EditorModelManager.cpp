#include "stdafx.h"
#include "EditorModelManager.h"
#include "RigidBodyModelWrapper.h"

editor::EditorModelManager::EditorModelManager(render::Shader *s, physics::CollisionWorld *physics)
{
	templates = new EditorParts(s);
	world = physics;
	srand(0);
}

editor::EditorModelManager::~EditorModelManager()
{
	delete templates;
}

float rnd()
{
	return static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 0.5f));
}


void editor::EditorModelManager::addPart(PartType type, glm::ivec3 pos)
{
	auto ri = new render::RenderInstance<ShaderParts::ShaderPartsSSBO>(templates->getModel(CUBE)->stdro);

	PartEntry pe;
	pe.ri = ri;
	pe.type = type;
	pe.pos = pos;
	pe.color = glm::vec4(rnd() + 0.5f, rnd() + 0.5f, rnd() + 0.5f, 1.0);
	auto rigidbody = new physics::RigidBody(0, templates->getModel(CUBE)->hull->copyAndTransform(glm::translate(glm::mat4(1), glm::vec3(pos))));
	pe.collision_body = rigidbody;

	parts.push_back(pe);



	rigidbody->userindex = parts.size() - 1;

	world->addCollisionBody(rigidbody);
}

void editor::EditorModelManager::removePart(int id)
{
	int last = parts.size() - 1;
	std::swap(parts[id], parts[last]);

	parts[id].collision_body->userindex = id;

	delete parts[last].collision_body;
	delete parts[last].ri;

	parts.pop_back();
}

void editor::EditorModelManager::prepareRender(render::RenderCache * cache)
{
	for (int i = 0; i < parts.size(); i++)
	{
		auto t = parts[i].ri->getSSBO();
		t->data[parts[i].ri->ssboid].model_matrix = glm::translate(glm::mat4(1), glm::vec3(parts[i].pos));
		t->data[parts[i].ri->ssboid].normal_matrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(t->data[parts[i].ri->ssboid].model_matrix))));
		t->data[parts[i].ri->ssboid].color = parts[i].color;
		cache->enqueueInstance(parts[i].ri);
	}
}
