#pragma once
#include <string>
#include <opengl_include.h>

namespace render
{

	class ShaderFragment
	{
	public:
		struct ConstructionInfo
		{
			std::string src;
			GLenum type;
			ConstructionInfo(std::string s, GLenum t) : src(s), type(t) {}
		};

	private:
		const GLchar* readShader(std::string src);
		void logShaderErrors(GLuint id, std::string filename);

	public:
		GLuint id;

	public:
		ShaderFragment(ConstructionInfo &ci) : ShaderFragment(ci.src, ci.type) {}
		ShaderFragment(std::string src, GLenum type);
		~ShaderFragment();

		GLuint getShaderId() { return id; }
	};

	typedef ShaderFragment::ConstructionInfo SCInfo;
}
