#include "stdafx.h"
#include "EditorParts.h"
#include "RigidBodyModelWrapper.h"
#include "TinyObjModel.h"
#include <EngineCore\ResourceManager.h>
#include <EngineCore\FileManager.h>

namespace editor
{
	EditorParts::EditorParts(render::Shader *s)
	{
		float *buffer;

		for (int i = 0; i < _PartTypeCount; i++)
		{
			auto m = new loader::TinyObjModel(RM_FILE_BASE->getDirPath("content/editor/parts"), "p" + std::to_string(i) + ".obj");
			parts[i] = new loader::RigidBodyModelWrapper(s, m);
			delete m;
		}
	}

	EditorParts::~EditorParts()
	{
		for (int i = 0; i < _PartTypeCount; i++)
		{
			delete parts[i];
		}
	}

	loader::RigidBodyModelWrapper * EditorParts::getModel(PartType pt)
	{
		return parts[pt];
	}

}