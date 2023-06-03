#pragma once

#include "IEventDispatcher.h"

class IApplicationListener;

class IApplication : public IEventDispatcher<IApplicationListener>
{
public:
	virtual void MainLoop() = 0;

	virtual ~IApplication() {}
};
