#pragma once
#include <vector>

namespace scenegraph
{
	class SceneGraph;
	class StateBuffer;

	class SceneNode 
	{
	private:
		friend class SceneGraph;
		SceneGraph *related;
		
	protected:
		// list of children
		std::vector<SceneNode*> children;
		SceneNode *parent;

	public:
		//constructor
		SceneNode();
		
		SceneGraph * getRelatedGraph() { return related; }

		virtual ~SceneNode();

		/*Run after Physic tick*/
		virtual void update(StateBuffer *state);

		void addChild(SceneNode* node);

		void removeChild(SceneNode *node);

		virtual void addedToSceneGraph() {}

		SceneNode* getParent() { return parent; };

	};

}