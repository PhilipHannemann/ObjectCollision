#pragma once
#include <EngineCore/AScene.h>

namespace render
{
	class Camera;
	class CameraLookAt;
	class RenderObject;
	class IVertexData;
	class Shader;
	class RenderCache;
}

namespace physics
{
	class CollisionWorld;
}

namespace editor
{
	class ShaderParts;
	class EditorModelManager;
	class PartEntry;
	class EditorParts;
}

namespace editor
{

	class SceneQuantumEditor : public core::AScene
	{
	public:
		SceneQuantumEditor();
		virtual ~SceneQuantumEditor() {/*uninit should be called*/ }

	protected:
		//Renderer Parts
		render::RenderCache *cache;
		render::CameraLookAt *cam;
		ShaderParts *shader_parts;
		ShaderParts *shader_marking;
		EditorParts *marking_parts;

		void windowChanged(int width, int height) override;

		void init() override;
		void uninit() override;

		void tick() override;
		void render() override;

		virtual bool rawSDLEvent(SDL_Event* event) override;


		//virtual physics::DynamicsWorld* getPhysicWorld() override { return nullptr; }
		physics::CollisionWorld *physics;


		EditorModelManager *model;
		int selected_id;

	};

}