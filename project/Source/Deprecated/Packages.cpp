#include "stdafx.h"

#include "ResourceManager.h"
#include "Packages.h"
#include "FileManager.h"

using namespace core;

PackageProvider *Packages::earlystartup = nullptr;

void Packages::init(PackageProvider *pp, std::string filename)
{
	if (Packages::earlystartup == nullptr)
		Packages::earlystartup = new PackageProvider(RM_FILE_BASE->getFilePath("content/" + filename));
	Logger::info("Loaded qpak: " + filename);
}