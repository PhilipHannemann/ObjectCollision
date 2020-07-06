#include "stdafx.h"
#include "ConstructShaders.h"
#include <EngineCore/ResourceManager.h>
#include <EngineCore/FileManager.h>
#include "SoftBodyShader.h"

namespace render
{
	namespace construct
	{

		DefaultShader * make_DefaultShader(RenderCache *cache)
		{
			SCInfo sci[2] = { render::SCInfo(RM_FILE_BASE->getFilePath("default.vs.glsl"), GL_VERTEX_SHADER), render::SCInfo(RM_FILE_BASE->getFilePath("default.fs.glsl"), GL_FRAGMENT_SHADER) };
			return new render::DefaultShader(cache, 2, sci);
		}

		Shader * make_SoftBodyShader(RenderCache *cache)
		{
			SCInfo sci[3] = { render::SCInfo(RM_FILE_BASE->getFilePath("softbody.vs.glsl"), GL_VERTEX_SHADER), render::SCInfo(RM_FILE_BASE->getFilePath("softbody.gs.glsl"), GL_GEOMETRY_SHADER), render::SCInfo(RM_FILE_BASE->getFilePath("default.fs.glsl"), GL_FRAGMENT_SHADER) };
			return new render::SoftBodyShader(cache, 3, sci);
		}

		Shader * make_PointShader(RenderCache *cache)
		{
			SCInfo sci[2] = { render::SCInfo(RM_FILE_BASE->getFilePath("point.vs.glsl"), GL_VERTEX_SHADER), render::SCInfo(RM_FILE_BASE->getFilePath("point.fs.glsl"), GL_FRAGMENT_SHADER) };
			return new render::Shader(cache, 2, sci);
		}

	}
}