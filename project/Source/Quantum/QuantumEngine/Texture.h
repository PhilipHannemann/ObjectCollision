#pragma once
#include <opengl_include.h>
#include <fstream>

namespace render
{

	class Texture
	{
	private:
		unsigned int width;
		unsigned int height;
		unsigned int mipmaps;
		GLuint texid;

	public:

		GLuint getTexId() 
		{
			return texid;
		}

		GLuint bind(GLuint slot)
		{
			glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(GL_TEXTURE_2D, texid);
			return slot;
		}

		Texture::Texture(std::istream *is, GLuint warp = GL_REPEAT, GLuint mag = GL_LINEAR, GLuint min = GL_LINEAR_MIPMAP_LINEAR);
		Texture::Texture(unsigned int w, unsigned int h, GLuint warp = GL_CLAMP_TO_EDGE, GLuint mag = GL_NEAREST, GLuint min = GL_NEAREST);
		~Texture();
		void Texture::loadDDS(std::istream *is);
	};

}
