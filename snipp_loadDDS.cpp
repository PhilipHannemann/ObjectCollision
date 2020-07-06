GLuint loadDDS(std::istream *is)
{
	//Logger::getInstance()->write(StringUtils::format("Loading DDS Image %s", filename));

	unsigned char header[124];

	//FILE *fp;

	/* try to open the file */
	/*fp = fopen(filename, "rb");
	if (fp == NULL)
	{
		Logger::("Failed to load Image: could not open the file");
		return 0;
	}*/

	/* verify the type of file */
	char filecode[4];
	//fread(filecode, 1, 4, fp);
	is->read(filecode, 4);
	if (strncmp(filecode, "DDS ", 4) != 0)
	{
		//fclose(fp);
		//Logger::getInstance()->write("Failed to load Image: not a direct draw surface file");
		return 0;
	}

	/* get the surface desc */
	is->read((char*)header, 124);

	unsigned int height = *(unsigned int*)&(header[8]);
	unsigned int width = *(unsigned int*)&(header[12]);
	unsigned int linearSize = *(unsigned int*)&(header[16]);
	unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC = *(unsigned int*)&(header[80]);

	unsigned char * buffer;
	unsigned int bufsize;

	/* how big is it going to be including all mipmaps? */
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
	buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
	//fread(buffer, 1, bufsize, fp);

	is->read((char*)buffer, bufsize);

	/* close the file pointer */
	//fclose(fp);
	unsigned int format;
	/*unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
	
	switch (fourCC)
	{
	case FOURCC_DXT1:
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case FOURCC_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case FOURCC_DXT5:*/
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
	//	break;
	//default:
		//free(buffer);
		//Logger::getInstance()->write("Failed to load Image: dds file format not supported (supported formats: DXT1, DXT3, DXT5)");
		//return 0;
	//}

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;

	/* load the mipmaps */
	//for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
	//{
	unsigned int size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
	glCompressedTexImage2D(GL_TEXTURE_2D, 0, format, width, height,0, size, buffer + offset);

	//	offset += size;
	//	width /= 2;
	//	height /= 2;
	//}

	free(buffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//Unbind the texture
	glBindTexture(GL_TEXTURE_2D, 0);

	//Logger::getInstance()->write(StringUtils::format("Loaded DDS Image %s", filename));

	return textureID;
}
