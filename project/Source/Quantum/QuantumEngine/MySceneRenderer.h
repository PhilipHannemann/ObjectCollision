#pragma once
#include <EngineCore/AScene.h>
#include <opengl_include.h>
#include "SoftBodyModelWrapper.h"
#include "RigidBodyModelWrapper.h"
namespace render
{
	class Camera;
	class CameraLookAt;
	class LiniarObjectList;
	class RenderObject;
	class IVertexData;
	class Shader;
	class Texture;
	class RenderCache;
}

namespace physics
{
	class DynamicsWorld;
	class SBSphereTreeRayIntersectionAlgorithm;
}

namespace scenegraph
{
	class SceneGraph;
	class PhysicTransformationNode;
}

namespace nanogui
{
	class FormHelper;
	class AWindow;
	class DebugWindow;
	class SoftBodyWindow;
}


#include "DefaultShader.h"

class MySceneRenderer: public core::AScene
{
private:

	//Renderer Parts
	render::RenderCache *cache;

	render::CameraLookAt *cam;

	render::DefaultShader *default_shader;
	render::Shader *softbody_shader;

	scenegraph::SceneGraph *scene_graph;
	physics::DynamicsWorld *physics;

	struct
	{
		loader::RigidBodyModelWrapper *cube;
		loader::SoftBodyModelWrapper *monkey;
		loader::SoftBodyModelWrapper *softcube;
		loader::SoftBodyModelWrapper *softball;

	} models;

	physics::CollisionBody *picked;

	physics::SBSphereTreeRayIntersectionAlgorithm *sbstria;

	bool mouse_down = false;
	float push_strength = 0.0f;

public:
	nanogui::FormHelper *gui;
	nanogui::AWindow *config;
	nanogui::DebugWindow *debug;
	nanogui::SoftBodyWindow *sbwindow;

public:
	MySceneRenderer();
	~MySceneRenderer() {/*uninit should be called*/ }

	physics::DynamicsWorld* getPhysicWorld() { return physics; }

	void init() override;
	void uninit() override;
	void reset(int reset);
	void tick() override;
	void windowChanged(int width, int height) override;
	void render() override;

	virtual bool rawSDLEvent(SDL_Event* event) override;


private:
	void init_renderer();
	void init_gui();

	void create_scene(int num);
	void clear_scene();

	void uninit_renderer();
	void uninit_gui();



	void loadModels();

};

