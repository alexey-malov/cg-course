#pragma once

#include "IApplicationListener.h"
#include "IEventDispatcher.h"

class IApplication : public IEventDispatcher<IApplicationListener>
{
public:
	virtual void MainLoop() = 0;

	virtual ~IApplication() {}
};
