#include "stdafx.h"
#include "IDebugRender.h"
#include "ResourceManager.h"
#include "SDLCore.h"

namespace render 
{

	IDebugRender * render::IDebugRender::getDebugRender()
	{
		return ResourceManager::getInstance()->getSDLCore()->getDebugRender();
	}

	void IDebugRender::setDebugRender(IDebugRender * idr)
	{
		ResourceManager::getInstance()->getSDLCore()->setDebugRender(idr);
	}

}