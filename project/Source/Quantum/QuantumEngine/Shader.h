#pragma once
#include <opengl_include.h>
#include "ShaderFragment.h"

namespace render
{
	class RenderCache;
	class AShaderStorageBufferObject;

	class Shader
	{
	private:
		void logProgramErrors(GLuint program);

		friend class RenderCache;
		int sid;
		RenderCache * rc;

	protected:		
		void createShaderProgram(ShaderFragment* fragments[], int num);
		void initShaderAndFragments(int n, SCInfo fragment_info[]);
		Shader(RenderCache *cache);

	public:
		AShaderStorageBufferObject *ssbo;
		Shader(RenderCache *cache, int n, SCInfo fragment_info[]);
		virtual ~Shader();
		virtual void useProgram() { glUseProgram(program); }
		inline RenderCache* getAsignedRenderCache() { return sid == -1 ? nullptr : rc; }
		GLuint program;
	};

}

