/*
*	SceneGraph.h 
*	class scenegraph::SceneGraph: render::IObjectList;
*
*	Container für ScenenGraph-Knoten
*	Kann als IObjectList für Renderer benutzt werden
*
*	Basisevents:
*		update()					-	nach WorldTick
*		IObjectList::renderAll()	-	Renderaufforderung für einen bestimmten DrawPass
*/

#pragma once
#include "SceneNode.h"

//forward
namespace render 
{ 
	class DrawPass;
	class RenderCache;
}

namespace core
{
	class AScene;
}

namespace scenegraph
{

	class StateBuffer
	{
	public:
		render::RenderCache *rcache;
		glm::mat4x4 transform;
		bool isTransformIdentity;

		StateBuffer(render::RenderCache *rc) { rcache = rc; }
	};


	class SceneGraph
	{
	private:
		//Link zur übergeordneten Scene
		core::AScene *scene_access;
		//Link zum Wurzelknoten 
		SceneNode *root;

	public:
		SceneGraph(core::AScene *scene);
		~SceneGraph();

		core::AScene* getScene() { return scene_access; }

		//Füge neuen Knoten <pNode> in Graph bei <parent> ein. <parent>=nullptr zum einfügen am Wurzelknoten
		void addChild(SceneNode* parent, SceneNode* pNode) 
		{ 
			if (parent == nullptr) parent = root; 
			parent->addChild(pNode); 
			pNode->related = this; 
			pNode->parent = parent;
			pNode->addedToSceneGraph(); 
		}

		//update tick
		virtual void update(render::RenderCache *rcache)
		{
			StateBuffer *sb = new StateBuffer(rcache);
			sb->transform = glm::mat4(1);
			sb->isTransformIdentity = true;
			root->update(sb); 
			delete sb;
		}
	};

}
