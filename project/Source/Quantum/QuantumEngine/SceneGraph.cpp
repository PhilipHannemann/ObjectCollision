#include "stdafx.h"
#include "SceneGraph.h"
#include "BranchGroupeNode.h"

namespace scenegraph
{


	SceneGraph::SceneGraph(core::AScene *scene)
	{
		scene_access = scene;
		root = new BranchGroupeNode();
		root->parent = nullptr;
	}


	SceneGraph::~SceneGraph()
	{
		delete root;
	}



}