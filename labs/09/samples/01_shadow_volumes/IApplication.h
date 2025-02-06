#pragma once

#include "IEventDispatcher.h"

class IApplicationListener;

class IApplication : public IEventDispatcher<IApplicationListener>
{
public:
	virtual void MainLoop(void) = 0;

	virtual ~IApplication(void){}
};
