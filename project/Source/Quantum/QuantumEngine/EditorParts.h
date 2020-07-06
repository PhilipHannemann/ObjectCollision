#pragma once

namespace render
{
	class Shader;
}

namespace loader 
{
	class RigidBodyModelWrapper;
}

namespace editor
{
	const int _PartTypeCount = 1;

	enum PartType
	{
		CUBE = 0
	};

	class EditorParts
	{
	private:
		loader::RigidBodyModelWrapper *parts[_PartTypeCount];

	public:
		EditorParts(render::Shader *s);
		~EditorParts();
	
		loader::RigidBodyModelWrapper *getModel(PartType pt);
	
	};

}