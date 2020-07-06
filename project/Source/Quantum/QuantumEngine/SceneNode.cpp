#include "stdafx.h"
#include "SceneNode.h"
#include <algorithm>

namespace scenegraph
{

	SceneNode::SceneNode() { }

	SceneNode::~SceneNode() 
	{ 
		if (parent != nullptr)
			parent->removeChild(this);

		for (std::vector<SceneNode*>::iterator i = children.begin(); i != children.end(); i++)
		{
			(*i)->parent = nullptr;	//no need to organize the waste
			delete (*i);
		}

		//automatic dealloc
		//m_lstChildren.clear();
	}

	// update our scene node
	void SceneNode::update(StateBuffer *state)
	{
		for (std::vector<SceneNode*>::iterator i = children.begin(); i != children.end(); i++)
		{
			(*i)->update(state);
		}
	}

	void SceneNode::addChild(SceneNode* pNode) 
	{
		children.push_back(pNode);
	}

	void SceneNode::removeChild(SceneNode * node)
	{
		children.erase(std::remove(children.begin(), children.end(), node), children.end());
	}

}