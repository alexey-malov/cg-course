#pragma once
#include "BaseWindow.h"
#include "Cube.h"

class Window : public BaseWindow
{
public:
	Window(int w, int h, const char* title);

private:
	void OnResize(int width, int height) override;

	void OnRunStart() override;

	void Draw(int width, int height) override;

	static void SetupProjectionMatrix(int width, int height);

	Cube m_cube;
};
