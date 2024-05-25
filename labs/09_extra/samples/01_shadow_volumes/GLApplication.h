#pragma once

#include "IApplication.h"
#include "IApplicationListener.h"

class CGLApplication 
	: public CEventDispatcherImpl<IApplication, IApplicationListener>
	, private CApplicationListener
{
public:
	void MainLoop(void);

protected:
	CGLApplication(
		const char * title, 
		int width = 0, 
		int height = 0, 
		bool needDepth = true, 
		bool needStencil = false
		);
	virtual ~CGLApplication(void);

	virtual void OnInit();
	// Данный метод всегда должен быть перегружен
	virtual void OnDisplay() = 0;	

	// Инициирует перерисовку изображения в окне
	void PostRedisplay();

	// Установка обработчика таймера
	typedef void (*TimerProc)(int value);
	void SetTimer(int milliseconds, TimerProc proc, int value);
private:
	void InitEventHandlers();

	static void DisplayHandler();
	static void ReshapeHandler(int width, int height);
	static void KeyboardHandler(unsigned char key, int x, int y);
	static void SpecialKeyHandler(int key, int x, int y);
	static void MouseHandler(int button, int state, int x, int y);
	static void MotionHandler(int x, int y);
	static void IdleHandler();
	static CGLApplication * m_pApplication;
};
