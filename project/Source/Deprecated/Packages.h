#pragma once
#include <string>
#include "PackageProvider.h"

struct Packages 
{
	static PackageProvider *earlystartup;




	static void init(PackageProvider *pp, std::string filename);

};
