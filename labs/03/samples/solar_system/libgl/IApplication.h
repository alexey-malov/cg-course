#pragma once

#include "IEventDispatcher.h"
#include "IApplicationListener.h"

class IApplication : public IEventDispatcher<IApplicationListener>
{
public:
	virtual void MainLoop() = 0;

	virtual ~IApplication() {}
};
