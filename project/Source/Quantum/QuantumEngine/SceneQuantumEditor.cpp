#include "stdafx.h"
#include "SceneQuantumEditor.h"
#include "CameraLookAt.h"
#include "RenderCache.h"
#include "ShaderParts.h"
#include <EngineCore\FileManager.h>
#include <EngineCore\ResourceManager.h>
#include "EditorModelManager.h"
#include <PhysicEngine\PhysicsWorld.h>

#include "EditorParts.h"
#include "RigidBodyModelWrapper.h"
#include <PhysicEngine\RigidBodyAABBRayIntersectAlgorithm.h>

namespace editor
{

	SceneQuantumEditor::SceneQuantumEditor()
	{
		cache = nullptr;
		cam = nullptr;
		shader_parts = nullptr;
		shader_marking = nullptr;
		model = nullptr;
		marking_parts = nullptr;
		selected_id = -1;
	}

	void SceneQuantumEditor::windowChanged(int width, int height)
	{
		AScene::windowChanged(width, height);

		if (cam != nullptr)
		{
			cam->setAspect(getAspect());
			cam->setupPickRayHelper(width, height);
		}
	}

	render::UBOGlobals::LightStruct *ligth;

	render::RenderInstance<ShaderParts::ShaderPartsSSBO> *ri;

	void SceneQuantumEditor::init()
	{
		physics::Dispatcher *dis = new physics::Dispatcher();
		physics = new physics::CollisionWorld(new physics::Broadphase(), dis);

		//SETUP DISPATCHER
		dis->addRayIntersectionAlgorithm(physics::body_index::RIGIDBODY_KINEMATIC, new physics::RigidBodyAABBRayIntersectionAlgorithm());

		//GRAPHICS - INIT
		cache = new render::RenderCache();

		//SHADER
		render::SCInfo sci1[2] = { render::SCInfo(RM_FILE_BASE->getFilePath("content/editor/shader/parts.vs.glsl"), GL_VERTEX_SHADER), render::SCInfo(RM_FILE_BASE->getFilePath("content/editor/shader/parts.fs.glsl"), GL_FRAGMENT_SHADER) };
		shader_parts = new ShaderParts(cache, 2, sci1);

		render::SCInfo sci2[2] = { render::SCInfo(RM_FILE_BASE->getFilePath("content/editor/shader/marking.vs.glsl"), GL_VERTEX_SHADER), render::SCInfo(RM_FILE_BASE->getFilePath("content/editor/shader/marking.fs.glsl"), GL_FRAGMENT_SHADER) };
		shader_marking = new ShaderParts(cache, 2, sci2);

		//CAMERA
		cam = new render::CameraLookAt();
		cam->setAspect(getAspect());
		cam->setFoV(glm::radians(45.0f));
		cam->setDistance(1.0f, 1000.0f);
		cam->setVectors(glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
		cam->setAll(glm::vec3(0, 0, 0), (float)M_PI / 2.0f + 0.2f, 1.0f, 40);
		cam->setupPickRayHelper(window_size.x, window_size.y);


		//MODELS
		model = new EditorModelManager(shader_parts, physics);
		marking_parts = new EditorParts(shader_marking);
		ri= new render::RenderInstance<ShaderParts::ShaderPartsSSBO> (marking_parts->getModel(CUBE)->stdro);

		/*for (int x = -10; x <= 10; x++)
		{
			for (int y = -10; y <= 10; y++)
			{
				for (int z = -10; z <= 10; z++)
				{
					float l = x * x + y * y + z * z;
					if(l >= 12*12 && l <= 13*13 || l <= 4*4 || l >= 8 * 8 && l <= 9* 9)
						model->addPart(CUBE, glm::ivec3(x,y,z));
				}
			}
		}*/

		model->addPart(CUBE, glm::ivec3(0));

		/*for (int i = 0; i < 999; i++)
		{
			model->addPart(CUBE, glm::ivec3(i / 10, i % 100, 0));
		}*/

		ligth = new render::UBOGlobals::LightStruct();


		ligth->ambient = glm::vec4(0.2, 0.2, 0.2, 1.0);
		ligth->diffuse = glm::vec4(1.0, 1.0, 1.0, 1.0);
		ligth->specular = glm::vec4(0.0, 0.0, 0.0, 1.0);
		ligth->position = glm::vec4(20.0, 20.0, 20.0, 1.0);

		cache->globals->data.numligths = 1;
	}

	void SceneQuantumEditor::uninit()
	{
		delete cam;
		delete shader_parts;
		delete cache;
	}
	
	
	void SceneQuantumEditor::tick()
	{

		ligth = new render::UBOGlobals::LightStruct();


		ligth->ambient = glm::vec4(0.2, 0.2, 0.2, 1.0);
		ligth->diffuse = glm::vec4(1.0, 1.0, 1.0, 1.0);
		ligth->specular = glm::vec4(0.0, 0.0, 0.0, 1.0);
		ligth->position = glm::vec4(10.0, 4.0, 8.0, 1.0);

		cache->globals->data.numligths = 1;

		cache->globals->data.lights[0] = *ligth;

		ligth = new render::UBOGlobals::LightStruct();


		ligth->ambient = glm::vec4(0.2, 0.2, 0.2, 1.0);
		ligth->diffuse = glm::vec4(1.0, 1.0, 1.0, 1.0);
		ligth->specular = glm::vec4(0.0, 0.0, 0.0, 1.0);
		ligth->position = glm::vec4(-10.0, 4.0, -8.0, 1.0);

		cache->globals->data.numligths = 2;

		cache->globals->data.lights[1] = *ligth;

		/*auto t = renderinstance->getSSBO();
		t->data[renderinstance->ssboid].model_matrix = glm::mat4(1);
		t->data[renderinstance->ssboid].normal_matrix = glm::mat4(1);
		t->data[renderinstance->ssboid].color = glm::vec4(1.0, 0.0, 1.0, 1.0);*/

		model->prepareRender(cache);

	//	cache->enqueueInstance(renderinstance);

		physics::Ray ray = cam->getPickRayTarget(cursor_pos.x, cursor_pos.y);
		physics::PickResult pr = physics->getFirstPickResult(ray);

		if (pr.hasHit())
		{
			glm::vec3 point = pr.pos;
			glm::vec3 dir = pr.normal;
			//glm::ivec3 opos = ((PartEntry*)pr.body->userptr)->pos;

			glm::vec3 newpos = point + 0.5f * dir;
			glm::ivec3 npos = glm::round(newpos);

			auto t = ri->getSSBO();
			t->data[ri->ssboid].model_matrix = glm::translate(glm::mat4(1), glm::vec3(npos));
			t->data[ri->ssboid].normal_matrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(t->data[ri->ssboid].model_matrix))));
			t->data[ri->ssboid].color = glm::vec4(0.0, 1.0, 0.0, 1.0);

			cache->enqueueInstance(ri);
		}


	}

	void SceneQuantumEditor::render()
	{
		glClearColor(0.005f, 0.005f, 0.005f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cache->globals->data.projectionMatrix = cam->getProjectionMatrix();
		cache->globals->data.viewMatrix = cam->getViewMatrix();
		cache->globals->data.normalMatrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(cam->getViewMatrix()))));

		//Transform; use later Compute Shader
		for (int i = 0; i < MAX_LIGHT; i++)
		{
			cache->globals->data.lights[i].position = cam->getViewMatrix() * cache->globals->data.lights[i].position;
		}

		shader_parts->ssbo->updateRange(0, 999);
		shader_marking->ssbo->updateRange(0, 999);

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		cache->render();
	}

	bool SceneQuantumEditor::rawSDLEvent(SDL_Event * event)
	{
		AScene::rawSDLEvent(event);

		if (event->type == SDL_MOUSEWHEEL)
		{
			cam->addZoom((float)event->wheel.y);
		}

		if (event->type == SDL_MOUSEMOTION && event->motion.state == 4)
		{
			cam->addPitch(-0.01f * (float)event->motion.yrel);
			cam->addYaw(-0.01f * (float)event->motion.xrel);
		}

		if (event->type == SDL_TEXTINPUT)
		{
			if (event->text.text[0] == 'w')
				cam->addCamX(0.2f);
			if (event->text.text[0] == 's')
				cam->addCamX(-0.2f);
			if (event->text.text[0] == 'a')
				cam->addCamZ(0.2f);
			if (event->text.text[0] == 'd')
				cam->addCamZ(-0.2f);
			if (event->text.text[0] == 'q')
				cam->addCenter(glm::vec3(0, 0.2f, 0));
			if (event->text.text[0] == 'e')
				cam->addCenter(glm::vec3(0, -0.2f, 0));

			if (event->text.text[0] == 'r')
			{
				if (selected_id >= 0)
				{
					model->removePart(selected_id);
					selected_id = -1;
				}
			}
		}

		if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT)
		{
			physics::Ray ray = cam->getPickRayTarget(cursor_pos.x, cursor_pos.y);
			physics::PickResult pr = physics->getFirstPickResult(ray);

			if (pr.hasHit())
			{
				glm::vec3 point = pr.pos;
				glm::vec3 dir = pr.normal;
				//glm::ivec3 opos = ((PartEntry*)pr.body->userptr)->pos;

				glm::vec3 newpos = point + 0.5f * dir;
				glm::ivec3 npos = glm::round(newpos);

				model->addPart(CUBE, npos);
			}
		}

		if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_MIDDLE)
		{
			physics::Ray ray = cam->getPickRayTarget(cursor_pos.x, cursor_pos.y);
			physics::PickResult pr = physics->getFirstPickResult(ray);

			if (pr.hasHit())
			{
				if(selected_id >= 0)
					model->getPartRef(selected_id)->color.a = 1.0;

				selected_id = pr.body->userindex;

				model->getPartRef(selected_id)->color.a = 0.0;
			}
		}

		return false;
	}

}


