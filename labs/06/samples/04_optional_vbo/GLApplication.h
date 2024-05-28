#pragma once

class CGLApplication
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
	// Методы-обработчики событий могут быть перегружены в классе-наследнике
	virtual void OnDisplay() = 0;	// данный метод всегда должен быть перегружен
	virtual void OnReshape(int width, int height);
	virtual void OnKeyboard(unsigned char key, int x, int y);
	virtual void OnMouse(int button, int state, int x, int y);
	virtual void OnMotion(int x, int y);
	virtual void OnSpaceballMotion(int x, int y, int z);
	virtual void OnSpaceballRotation(int x, int y, int z);
	virtual void OnSpaceballButton(int button, int state);
	virtual void OnIdle();

	// Инициирует перерисовку изображения в окне
	void PostRedisplay();

	// Установка обарботчика таймера
	typedef void (*TimerProc)(int value);
	void SetTimer(int milliseconds, TimerProc proc, int value);
private:
	void InitEventHandlers();
	static void DisplayHandler();
	static void ReshapeHandler(int width, int height);
	static void KeyboardHandler(unsigned char key, int x, int y);
	static void MouseHandler(int button, int state, int x, int y);
	static void MotionHandler(int x, int y);
	static void IdleHandler();
	static void SpaceballMotionHandler(int x, int y, int z);
	static void SpaceballRotateHandler(int x, int y, int z);
	static void SpaceballButtonHandler(int button, int state);
	static CGLApplication * m_pApplication;
};
