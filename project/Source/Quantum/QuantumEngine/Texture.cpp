#include "stdafx.h"
#include "Texture.h"
#include <fstream>
#include <EngineCore/Logger.h>
#include <EngineCore/BaseExceptions.h>

namespace render
{

	Texture::Texture(std::istream *is, GLuint warp, GLuint mag, GLuint min)
	{
		// Create one OpenGL texture
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &texid);
		glBindTexture(GL_TEXTURE_2D, texid);

		loadDDS(is);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, warp);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, warp);

		//Unbind the texture
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture::Texture(unsigned int w, unsigned int h, GLuint warp, GLuint mag, GLuint min)
	{
		// Create one OpenGL texture
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &texid);
		glBindTexture(GL_TEXTURE_2D, texid);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, warp);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, warp);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		width = w;
		height = h;
		mipmaps = 0;

		//Unbind the texture
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &texid);
	}

	#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
	#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
	#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

	void Texture::loadDDS(std::istream *is)
	{
		//Logger::getInstance()->write(StringUtils::format("Loading DDS Image %s", filename));

		unsigned char header[124];

		/* verify the type of file */
		char filecode[4];

		is->read(filecode, 4);
		if (strncmp(filecode, "DDS ", 4) != 0)
		{
			core::Logger::error("Failed to load Image: Not an DDS file!");
			throw TextureLoadingException("Failed to load Image: Not an DDS file!");
			return;
		}

		/* get the surface desc */
		is->read((char*)header, 124);

		height = *(unsigned int*)&(header[8]);
		width = *(unsigned int*)&(header[12]);
		unsigned int linearSize = *(unsigned int*)&(header[16]);
		unsigned int mipMapCount = *(unsigned int*)&(header[24]);
		unsigned int fourCC = *(unsigned int*)&(header[80]);

		unsigned char * buffer;
		unsigned int bufsize;

		/* how big is it going to be including all mipmaps? */
		bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
		buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));

		is->read((char*)buffer, bufsize);

		/* close the file pointer */
		unsigned int format;
		unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;

		switch (fourCC)
		{
			case FOURCC_DXT1:
				format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				break;
			case FOURCC_DXT3:
				format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				break;
			case FOURCC_DXT5:
				format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				break;
			default:
				free(buffer);
				core::Logger::error("Failed to load Image: dds file format not supported (supported formats: DXT1, DXT3, DXT5)");
				throw TextureLoadingException("Failed to load Image: dds file format not supported (supported formats: DXT1, DXT3, DXT5)");
				return;
		}

		// "Bind" the newly created texture : all future texture functions will modify this texture
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
		unsigned int offset = 0;

		/* load the mipmaps */
		for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
		{
			unsigned int size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
			glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, buffer + offset);

			offset += size;
			width /= 2;
			height /= 2;
		}

		free(buffer);

		//Logger::getInstance()->write(StringUtils::format("Loaded DDS Image %s", filename));
	}

}