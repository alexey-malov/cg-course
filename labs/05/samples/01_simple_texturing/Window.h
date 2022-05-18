#pragma once
#include "BaseWindow.h"

class Window : public BaseWindow
{
public:
private:
	void OnRunStart() override;
	void OnRunEnd() override;
	void Draw(int width, int height) override;
};
