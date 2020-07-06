#pragma once
#include "Shader.h"
#include "DefaultShader.h"

namespace render
{
	class RenderCache;

	namespace construct
	{
		DefaultShader* make_DefaultShader(RenderCache *cache = nullptr);
		Shader* make_SoftBodyShader(RenderCache *cache = nullptr);
		Shader* make_PointShader(RenderCache *cache = nullptr);
	}
}