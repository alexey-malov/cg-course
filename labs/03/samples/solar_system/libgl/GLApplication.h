#pragma once

#include "IApplication.h"
#include "IApplicationListener.h"
#include "common_inc.h"

class CGLApplication
	: public CEventDispatcherImpl<IApplication, IApplicationListener>
	, private CApplicationListener
{
public:
	void MainLoop() override;

protected:
	CGLApplication(
		const char* title,
		int width = 0,
		int height = 0,
		bool needDepth = true,
		bool needStencil = false);
	virtual ~CGLApplication(void);

	virtual void OnInit();
	// Данный метод всегда должен быть перегружен
	virtual void OnDisplay() = 0;

	// Инициирует перерисовку изображения в окне
	void PostRedisplay();

	// Установка обарботчика таймера
	typedef void (*TimerProc)(int value);
	void SetTimer(int milliseconds, TimerProc proc, int value);
	int GetWindowWidth() const { return m_width; }
	int GetWindowHeight() const { return m_height; }

private:
	void InitEventHandlers();
	void UpdateWindowSize(int width, int height);

	static void DisplayHandler();
	static void ReshapeHandler(int width, int height);
	static void KeyboardHandler(unsigned char key, int x, int y);
	static void SpecialKeyHandler(int key, int x, int y);
	static void MouseHandler(int button, int state, int x, int y);
	static void MotionHandler(int x, int y);
	static void IdleHandler();

private:
	static CGLApplication* m_pApplication;
	int m_width;
	int m_height;
};
