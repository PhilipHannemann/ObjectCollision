#include "stdafx.h"
#include "MySceneRenderer.h"

#include <EngineCore/ResourceManager.h>
#include <EngineCore/FileManager.h>
#include <EngineCore/SDLCore.h>
#include <EngineCore/Logger.h>

#include "TinyObjVertexData.h"
#include "RenderObject.h"
#include "CameraLookAt.h"
#include "ConstructShaders.h"
#include "RenderCache.h"

#include "SceneGraph.h"
#include "DebugRender.h"

#include "ConfigWindow.h"
#include "DebugWindow.h"
#include "SoftBodyWindow.h"

#include "TransformationNode.h"
#include "GeometryNode.h"

#include <cstdlib>
#include <ctime>

#include "LightNode.h"

#include <PhysicEngine\PhysicsWorld.h>

#include <EngineCore\IDebugRender.h>
#include "DebugRender.h"

#include <PhysicEngine\Broadphase.h>
#include <PhysicEngine\Dispatcher.h>
#include <PhysicEngine\SoftBodySolver.h>
#include "SoftBodyCompactNode.h"
#include <PhysicEngine\SBSphereTreeCollisionAlgorithm.h>
#include <PhysicEngine\SBRBCollisionAlgorithm.h>
#include <PhysicEngine\SBSphereTreeRayIntersectAlgorithm.h>
#include <PhysicEngine\RigidBodyAABBRayIntersectAlgorithm.h>

#include "StaticRigidBodyNode.h"

#include <PhysicEngine\RayHelper.h>
#include <PhysicEngine\BodyIndex.h>

MySceneRenderer::MySceneRenderer()
{
	cam = nullptr;
	default_shader = nullptr;
	softbody_shader = nullptr;
	physics = nullptr;
	picked = nullptr;
}


void MySceneRenderer::init()
{
	sbstria = new physics::SBSphereTreeRayIntersectionAlgorithm();
	sbstria->debug_render = false;
	physics::Dispatcher *dis = new physics::Dispatcher();
	physics = new physics::DynamicsWorld(new physics::Broadphase(), dis, new physics::SoftBodySolver());

	dis->addCollisionAlgorithm(physics::body_index::SOFTBODY, physics::body_index::SOFTBODY, new physics::SBSphereTreeCollisionAlgorithm());
	dis->addCollisionAlgorithm(physics::body_index::RIGIDBODY_KINEMATIC, physics::body_index::SOFTBODY, new physics::SBRBCollisionAlgorithm());
	dis->addRayIntersectionAlgorithm(physics::body_index::SOFTBODY, sbstria);
	dis->addRayIntersectionAlgorithm(physics::body_index::RIGIDBODY_KINEMATIC, new physics::RigidBodyAABBRayIntersectionAlgorithm());

	init_renderer();

	auto dr = new render::DebugRender();
	dr->setCamera(cam);
	render::IDebugRender::setDebugRender(dr);

	init_gui();
	create_scene(1);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void MySceneRenderer::uninit()
{
	clear_scene();

	delete render::IDebugRender::getDebugRender();
	render::IDebugRender::setDebugRender(nullptr);

	uninit_renderer();
	uninit_gui();

	delete physics;
}

void MySceneRenderer::reset(int reset)
{
	clear_scene();
	create_scene(reset);
}

void MySceneRenderer::init_renderer()
{
	int64_t num = 0;
	glGetInteger64v(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &num);
	core::Logger::debug(to_string(num));


	//3. GRAPHICS - INIT
	cache = new render::RenderCache();

	//3.1. SHADER
	default_shader = render::construct::make_DefaultShader(cache);
	softbody_shader = render::construct::make_SoftBodyShader(cache);

	//3.3. CAMERA
	cam = new render::CameraLookAt();
	cam->setAspect(getAspect());
	cam->setFoV(glm::radians(45.0f));
	cam->setDistance(1.0f, 1000.0f);
	cam->setVectors(glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
	cam->setAll(glm::vec3(0, 0, 0), (float) M_PI / 2.0f + 0.2f, 1.0f, 40);
	cam->setupPickRayHelper(window_size.x, window_size.y);

	loadModels();

	//ResourceManager::getInstance()->getSDLCore()->getDebugRender()->setCamera(cam);
}

void MySceneRenderer::uninit_renderer()
{
	delete models.cube;
	delete models.monkey;
	delete models.softcube;
	delete models.softball;

	delete cam;
	delete default_shader;
	delete softbody_shader;
	delete cache;
}


void MySceneRenderer::init_gui()
{
	config = new ConfigWindow(gui_screen);
	debug = new nanogui::DebugWindow(gui_screen);
	sbwindow = new nanogui::SoftBodyWindow(gui_screen);

	gui_screen->setVisible(true);
	gui_screen->performLayout();

	sbwindow->window->setVisible(false);
}

void MySceneRenderer::uninit_gui()
{
	delete debug;
	delete config;
	delete sbwindow;
}


void MySceneRenderer::loadModels()
{
	loader::TinyObjModel *m = nullptr;
	
	m = new loader::TinyObjModel(RM_FILE_BASE->getDirPath("content"), "cube.obj");
	models.cube = new loader::RigidBodyModelWrapper(default_shader, m);
	delete m;

	m = new loader::TinyObjModel(RM_FILE_BASE->getDirPath("content"), "cube2.obj");
	models.softcube = new loader::SoftBodyModelWrapper(softbody_shader, m);
	delete m;

	m = new loader::TinyObjModel(RM_FILE_BASE->getDirPath("content"), "sphere.obj");
	models.softball = new loader::SoftBodyModelWrapper(softbody_shader, m);
	delete m;

	m = new loader::TinyObjModel(RM_FILE_BASE->getDirPath("content"), "monkey.obj");
	models.monkey = new loader::SoftBodyModelWrapper(softbody_shader, m);
	delete m;

}


void MySceneRenderer::create_scene(int num)
{
	scene_graph = new scenegraph::SceneGraph(this);

	//Ground
	{
		glm::mat4 trans = glm::scale(glm::translate(glm::mat4(1), glm::vec3(0, -1, 0)), glm::vec3(40, 1, 40));
		auto tr = new scenegraph::TransformationNode(trans);
		scene_graph->addChild(nullptr, tr);
		auto dgn = new scenegraph::StaticRigidBodyNode(models.cube, getPhysicWorld(), trans);
		scene_graph->addChild(tr, dgn);
		dgn->setColor(glm::vec4(0.8f, 0.8f, 0.8f, 1.0));
	}

	if (num == 1)
	{
		auto  sbn = new scenegraph::SoftBodyCompactNode(getPhysicWorld(), models.softcube, 100.0f, glm::scale(glm::translate(glm::mat4(1), glm::vec3(0, 10.0F, -12.0f)), glm::vec3(1.0, 1.0, 1.0)), physics::deformation_mode::LINEAR);
		scene_graph->addChild(nullptr, sbn);
		sbn->setColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0));
		sbn->softbody->alpha = 1.0f;
		sbn->softbody->beta = 0.0f;

		sbn = new scenegraph::SoftBodyCompactNode(getPhysicWorld(), models.softcube, 100.0f, glm::scale(glm::translate(glm::mat4(1), glm::vec3(0, 10.0F, -6.0f)), glm::vec3(1.0, 1.0, 1.0)), physics::deformation_mode::LINEAR);
		scene_graph->addChild(nullptr, sbn);
		sbn->setColor(glm::vec4(1.0f, 0.4f, 1.0f, 1.0));
		sbn->softbody->alpha = 1.0f;
		sbn->softbody->beta = 0.4f;

		sbn = new scenegraph::SoftBodyCompactNode(getPhysicWorld(), models.softcube, 100.0f, glm::scale(glm::translate(glm::mat4(1), glm::vec3(0, 10.0F, 0)), glm::vec3(1.0, 1.0, 1.0)), physics::deformation_mode::LINEAR);
		scene_graph->addChild(nullptr, sbn);
		sbn->setColor(glm::vec4(0.6f, 0.4f, 1.0f, 1.0));
		sbn->softbody->alpha = 0.6f;
		sbn->softbody->beta = 0.4f;

		sbn = new scenegraph::SoftBodyCompactNode(getPhysicWorld(), models.softcube, 100.0f, glm::scale(glm::translate(glm::mat4(1), glm::vec3(0, 10.0F, 6.0f)), glm::vec3(1.0, 1.0, 1.0)), physics::deformation_mode::LINEAR);
		scene_graph->addChild(nullptr, sbn);
		sbn->setColor(glm::vec4(0.75f, 0.8f, 1.0f, 1.0));
		sbn->softbody->alpha = 0.75f;
		sbn->softbody->beta = 0.8f;

		sbn = new scenegraph::SoftBodyCompactNode(getPhysicWorld(), models.softcube, 100.0f, glm::scale(glm::translate(glm::mat4(1), glm::vec3(0, 10.0F, 12.0f)), glm::vec3(1.0, 1.0, 1.0)), physics::deformation_mode::LINEAR);
		scene_graph->addChild(nullptr, sbn);
		sbn->setColor(glm::vec4(0.5f, 0.8f, 1.0f, 1.0));
		sbn->softbody->alpha = 0.5f;
		sbn->softbody->beta = 0.8f;
	}

	if (num == 2)
	{
		auto  sbn = new scenegraph::SoftBodyCompactNode(getPhysicWorld(), models.monkey, 600.0f, glm::scale(glm::translate(glm::mat4(1), glm::vec3(0, 16.0F, 0.0f)), glm::vec3(1.6, 1.6, 1.6)), physics::deformation_mode::LINEAR);
		scene_graph->addChild(nullptr, sbn);
		sbn->setColor(glm::vec4(0.7f, 0.6f, 1.0f, 1.0));
		sbn->softbody->alpha = 0.7f;
		sbn->softbody->beta = 0.6f;

	}

	if (num == 3)
	{

		//Plate
		{
			glm::mat4 trans = glm::scale(glm::translate(glm::mat4(1), glm::vec3(0, 4.5, 0)), glm::vec3(9, 0.3, 6));
			auto tr = new scenegraph::TransformationNode(trans);
			scene_graph->addChild(nullptr, tr);
			auto dgn = new scenegraph::StaticRigidBodyNode(models.cube, getPhysicWorld(), trans);
			scene_graph->addChild(tr, dgn);
			dgn->setColor(glm::vec4(0.2f, 0.6f, 0.3f, 1.0));
		}

		//Border L
		{
			glm::mat4 trans = glm::scale(glm::translate(glm::mat4(1), glm::vec3(0, 4.75, -6.3)), glm::vec3(9, 0.8, 0.3));
			auto tr = new scenegraph::TransformationNode(trans);
			scene_graph->addChild(nullptr, tr);
			auto dgn = new scenegraph::StaticRigidBodyNode(models.cube, getPhysicWorld(), trans);
			scene_graph->addChild(tr, dgn);
			dgn->setColor(glm::vec4(0.4f, 0.2f, 0.15f, 1.0));
		}
		//Border R
		{
			glm::mat4 trans = glm::scale(glm::translate(glm::mat4(1), glm::vec3(0, 4.75, 6.3)), glm::vec3(9, 0.8, 0.3));
			auto tr = new scenegraph::TransformationNode(trans);
			scene_graph->addChild(nullptr, tr);
			auto dgn = new scenegraph::StaticRigidBodyNode(models.cube, getPhysicWorld(), trans);
			scene_graph->addChild(tr, dgn);
			dgn->setColor(glm::vec4(0.4f, 0.2f, 0.15f, 1.0));
		}
		//Border O
		{
			glm::mat4 trans = glm::scale(glm::translate(glm::mat4(1), glm::vec3(-9.3, 4.75, 0)), glm::vec3(0.3, 0.8, 6));
			auto tr = new scenegraph::TransformationNode(trans);
			scene_graph->addChild(nullptr, tr);
			auto dgn = new scenegraph::StaticRigidBodyNode(models.cube, getPhysicWorld(), trans);
			scene_graph->addChild(tr, dgn);
			dgn->setColor(glm::vec4(0.4f, 0.2f, 0.15f, 1.0));
		}
		//Border U
		{
			glm::mat4 trans = glm::scale(glm::translate(glm::mat4(1), glm::vec3(9.3, 4.75, 0)), glm::vec3(0.3, 0.8, 6));
			auto tr = new scenegraph::TransformationNode(trans);
			scene_graph->addChild(nullptr, tr);
			auto dgn = new scenegraph::StaticRigidBodyNode(models.cube, getPhysicWorld(), trans);
			scene_graph->addChild(tr, dgn);
			dgn->setColor(glm::vec4(0.4f, 0.2f, 0.15f, 1.0));
		}
		//Pilar 1
		{
			glm::mat4 trans = glm::scale(glm::translate(glm::mat4(1), glm::vec3(9.3, (4.75 + 0.8) / 2.0, 6.3)), glm::vec3(0.3, (4.75 + 0.8) / 2.0, 0.3));
			auto tr = new scenegraph::TransformationNode(trans);
			scene_graph->addChild(nullptr, tr);
			auto dgn = new scenegraph::StaticRigidBodyNode(models.cube, getPhysicWorld(), trans);
			scene_graph->addChild(tr, dgn);
			dgn->setColor(glm::vec4(0.4f, 0.4f, 0.4f, 1.0));
		}
		//Pilar 2
		{
			glm::mat4 trans = glm::scale(glm::translate(glm::mat4(1), glm::vec3(-9.3, (4.75 + 0.8) / 2.0, 6.3)), glm::vec3(0.3, (4.75 + 0.8) / 2.0, 0.3));
			auto tr = new scenegraph::TransformationNode(trans);
			scene_graph->addChild(nullptr, tr);
			auto dgn = new scenegraph::StaticRigidBodyNode(models.cube, getPhysicWorld(), trans);
			scene_graph->addChild(tr, dgn);
			dgn->setColor(glm::vec4(0.4f, 0.4f, 0.4f, 1.0));
		}
		//Pilar 3
		{
			glm::mat4 trans = glm::scale(glm::translate(glm::mat4(1), glm::vec3(9.3, (4.75 + 0.8) / 2.0, -6.3)), glm::vec3(0.3, (4.75 + 0.8) / 2.0, 0.3));
			auto tr = new scenegraph::TransformationNode(trans);
			scene_graph->addChild(nullptr, tr);
			auto dgn = new scenegraph::StaticRigidBodyNode(models.cube, getPhysicWorld(), trans);
			scene_graph->addChild(tr, dgn);
			dgn->setColor(glm::vec4(0.4f, 0.4f, 0.4f, 1.0));
		}
		//Pilar 4
		{
			glm::mat4 trans = glm::scale(glm::translate(glm::mat4(1), glm::vec3(-9.3, (4.75 + 0.8) / 2.0, -6.3)), glm::vec3(0.3, (4.75 + 0.8) / 2.0, 0.3));
			auto tr = new scenegraph::TransformationNode(trans);
			scene_graph->addChild(nullptr, tr);
			auto dgn = new scenegraph::StaticRigidBodyNode(models.cube, getPhysicWorld(), trans);
			scene_graph->addChild(tr, dgn);
			dgn->setColor(glm::vec4(0.4f, 0.4f, 0.4f, 1.0));
		}


		{

			auto sbn = new scenegraph::SoftBodyCompactNode(getPhysicWorld(), models.softcube, 100.0f, glm::scale(glm::translate(glm::mat4(1), glm::vec3(0, 6.0F, 0)), glm::vec3(0.4, 0.4, 0.4)), physics::deformation_mode::LINEAR);
			scene_graph->addChild(nullptr, sbn);
			sbn->setColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0));
			sbn->softbody->alpha = 1.0f;
			sbn->softbody->beta = 0.0f;

			sbn = new scenegraph::SoftBodyCompactNode(getPhysicWorld(), models.softcube, 100.0f, glm::scale(glm::translate(glm::mat4(1), glm::vec3(2.0F, 6.0F, 2.0F)), glm::vec3(0.4, 0.4, 0.4)), physics::deformation_mode::LINEAR);
			scene_graph->addChild(nullptr, sbn);
			sbn->setColor(glm::vec4(0.8f, 0.0f, 1.0f, 1.0));
			sbn->softbody->alpha = 0.8f;
			sbn->softbody->beta = 0.0f;

			sbn = new scenegraph::SoftBodyCompactNode(getPhysicWorld(), models.softcube, 100.0f, glm::scale(glm::translate(glm::mat4(1), glm::vec3(-2.0F, 6.0F, 2.0F)), glm::vec3(0.4, 0.4, 0.4)), physics::deformation_mode::LINEAR);
			scene_graph->addChild(nullptr, sbn);
			sbn->setColor(glm::vec4(1.0f, 0.4f, 1.0f, 1.0));
			sbn->softbody->alpha = 1.0f;
			sbn->softbody->beta = 0.4f;

			sbn = new scenegraph::SoftBodyCompactNode(getPhysicWorld(), models.softcube, 100.0f, glm::scale(glm::translate(glm::mat4(1), glm::vec3(2.0F, 6.0F, -2.0F)), glm::vec3(0.4, 0.4, 0.4)), physics::deformation_mode::LINEAR);
			scene_graph->addChild(nullptr, sbn);
			sbn->setColor(glm::vec4(1.0f, 0.6f, 1.0f, 1.0));
			sbn->softbody->alpha = 1.0f;
			sbn->softbody->beta = 0.6f;

			sbn = new scenegraph::SoftBodyCompactNode(getPhysicWorld(), models.softcube, 100.0f, glm::scale(glm::translate(glm::mat4(1), glm::vec3(-2.0F, 6.0F, -2.0F)), glm::vec3(0.4, 0.4, 0.4)), physics::deformation_mode::LINEAR);
			scene_graph->addChild(nullptr, sbn);
			sbn->setColor(glm::vec4(1.0f, 0.8f, 1.0f, 1.0));
			sbn->softbody->alpha = 1.0f;
			sbn->softbody->beta = 0.8f;

			//

			sbn = new scenegraph::SoftBodyCompactNode(getPhysicWorld(), models.softball, 300.0f, glm::scale(glm::translate(glm::mat4(1), glm::vec3(0.0F, 6.0F, 4.0F)), glm::vec3(0.4, 0.4, 0.4)), physics::deformation_mode::LINEAR);
			scene_graph->addChild(nullptr, sbn);
			sbn->setColor(glm::vec4(0.8f, 0.0f, 0.0f, 1.0));
			sbn->softbody->alpha = 0.8f;
			sbn->softbody->beta = 0.0f;

			sbn = new scenegraph::SoftBodyCompactNode(getPhysicWorld(), models.softball, 300.0f, glm::scale(glm::translate(glm::mat4(1), glm::vec3(-4.0F, 6.0F, 0.0F)), glm::vec3(0.4, 0.4, 0.4)), physics::deformation_mode::LINEAR);
			scene_graph->addChild(nullptr, sbn);
			sbn->setColor(glm::vec4(1.0f, 0.4f, 0.0f, 1.0));
			sbn->softbody->alpha = 1.0f;
			sbn->softbody->beta = 0.4f;

			sbn = new scenegraph::SoftBodyCompactNode(getPhysicWorld(), models.softball, 300.0f, glm::scale(glm::translate(glm::mat4(1), glm::vec3(0.0F, 6.0F, -4.0F)), glm::vec3(0.4, 0.4, 0.4)), physics::deformation_mode::LINEAR);
			scene_graph->addChild(nullptr, sbn);
			sbn->setColor(glm::vec4(1.0f, 0.6f, 0.0f, 1.0));
			sbn->softbody->alpha = 1.0f;
			sbn->softbody->beta = 0.6f;

			sbn = new scenegraph::SoftBodyCompactNode(getPhysicWorld(), models.softball, 300.0f, glm::scale(glm::translate(glm::mat4(1), glm::vec3(4.0F, 6.0F, 0.0F)), glm::vec3(0.4, 0.4, 0.4)), physics::deformation_mode::LINEAR);
			scene_graph->addChild(nullptr, sbn);
			sbn->setColor(glm::vec4(1.0f, 0.8f, 0.0f, 1.0));
			sbn->softbody->alpha = 1.0f;
			sbn->softbody->beta = 0.8f;

		}

	}

	
	auto tr = new scenegraph::TransformationNode(glm::translate(glm::mat4(1), glm::vec3(6.0, 7.0, 0.0)));
	scene_graph->addChild(nullptr, tr);
	auto l = new scenegraph::LightNode();
	l->ligth.ambient = glm::vec4(0.1, 0.1, 0.1, 1.0);
	l->ligth.diffuse = glm::vec4(1.0, 1.0, 1.0, 1.0);
	scene_graph->addChild(tr, l);

	tr = new scenegraph::TransformationNode(glm::translate(glm::mat4(1), glm::vec3(-6.0, 7.0, 0.0)));
	scene_graph->addChild(nullptr, tr);
	l = new scenegraph::LightNode();
	l->ligth.ambient = glm::vec4(0.0, 0.0, 0.0, 1.0);
	l->ligth.diffuse = glm::vec4(1.0, 1.0, 1.0, 1.0);
	scene_graph->addChild(tr, l);
	


}

void MySceneRenderer::clear_scene()
{
	delete scene_graph;
	sbwindow->window->setVisible(false);
}


void MySceneRenderer::tick()
{
	if (mouse_down)
		push_strength += 6.0f;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	if (debug->showSphereTree())
	{
		sbstria->debug_render = true;
		physics::Ray ray = cam->getPickRayTarget(cursor_pos.x, cursor_pos.y);
		physics::PickResult pr = getPhysicWorld()->getFirstPickResult(ray);
	}
	else
	{
		sbstria->debug_render = false;
	}

	//getPhysicWorld()->getFirstPickResult(physics::Ray(glm::normalize(glm::vec3(0,0,1)), glm::vec3(0.1, 0.2, -10)));

	int reset = debug->shouldReset(false);

	if (reset > 0)
	{
		this->reset(reset);
	}

	if (!debug->isFrozen() || debug->shouldTick(false))
	{
		physics->stepWorld(1.0f / 60.0f);
	}

	//real update
	cache->globals->data.numligths = 0;
	scene_graph->update(cache);
}

void MySceneRenderer::render()
{
	cache->globals->data.projectionMatrix = cam->getProjectionMatrix();
	cache->globals->data.viewMatrix = cam->getViewMatrix();

	//Transform; use later Compute Shader
	for (int i = 0; i < MAX_LIGHT; i++)
	{
		cache->globals->data.lights[i].position = cam->getViewMatrix() * cache->globals->data.lights[i].position;
	}


	default_shader->ssbo->updateRange(0, 999);
	softbody_shader->ssbo->updateRange(0, 999);
	
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	if (debug->doWireframe())
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	cache->render();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (debug->showGoalpos())
		getPhysicWorld()->renderDebugInfos();

	//render::IDebugRender::getDebugRender()->drawSphere(glm::vec3(0,1,2), 0.05, glm::vec4(1, 1, 1, 1));
}


bool MySceneRenderer::rawSDLEvent(SDL_Event * event)
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

	if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT)
	{

	}

	if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT)
	{
		physics::Ray ray = cam->getPickRayTarget(cursor_pos.x, cursor_pos.y);
		physics::PickResult pr = getPhysicWorld()->getFirstPickResult(ray);

		if (sbwindow->sb != nullptr)
		{
			auto node = ((scenegraph::SoftBodyCompactNode*)sbwindow->sb->userptr);
			glm::vec4 c = node->getColor();
			c.a = 1.0;
			node->setColor(c);
			sbwindow->update(nullptr);
			sbwindow->window->setVisible(false);
		}

		if (pr.hasHit() && pr.body->type == physics::body_index::SOFTBODY)
		{
			picked = pr.body;
			//sbwindow->alpha = ((physics::SoftBody*) picked)->alpha;
			sbwindow->update(((physics::SoftBody*) picked));
			auto node = ((scenegraph::SoftBodyCompactNode*)picked->userptr);
			glm::vec4 c = node->getColor();
			c.a = 0.0;
			node->setColor(c);
			sbwindow->window->setVisible(true);
		}
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
		
	}

	if (event->type == SDL_KEYDOWN && event->key.keysym.scancode == SDL_SCANCODE_SPACE)
	{
		mouse_down = true;
	}

	if (event->type == SDL_KEYUP && event->key.keysym.scancode == SDL_SCANCODE_SPACE)
	{
		mouse_down = false;

		physics::Ray ray = cam->getPickRayTarget(cursor_pos.x, cursor_pos.y);
		physics::PickResult pr = getPhysicWorld()->getFirstPickResult(ray);

		if (pr.hasHit() && pr.body->type == physics::body_index::SOFTBODY)
		{
			getPhysicWorld()->applyForce(pr.body, ray.dir, push_strength);
			core::Logger::info("Push with: " + std::to_string(push_strength));
		}

		push_strength = 0.0f;
	}

	return false;
}

void MySceneRenderer::windowChanged(int w, int h)
{
	AScene::windowChanged(w, h);

	if (cam != nullptr)
	{
		cam->setAspect(getAspect());
		cam->setupPickRayHelper(w, h);
	}
}








//tex = new render::Texture(800, 600, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);

/* Depth buffer */
/*glGenRenderbuffers(1, &rbo_depth);
glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 800, 600);
glBindRenderbuffer(GL_RENDERBUFFER, 0);*/

/* Framebuffer to link everything together */
/*glGenFramebuffers(1, &fbo);
glBindFramebuffer(GL_FRAMEBUFFER, fbo);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->getTexId(), 0);
glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);*/
/*
GLenum status;
if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
fprintf(stderr, "glCheckFramebufferStatus: error %p", status);
return;
}
glBindFramebuffer(GL_FRAMEBUFFER, 0);*/