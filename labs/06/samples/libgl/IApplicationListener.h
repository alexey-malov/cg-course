#pragma once

// Интерфейс "Слушатель событий от приложения"
class IApplicationListener
{
public:
	// Методы, вызываемые при наступлении определенных событий
	// в приложении
	virtual void OnDisplay() = 0;
	virtual void OnReshape(int width, int height) = 0;
	virtual void OnKeyboard(unsigned char key, int x, int y) = 0;
	virtual void OnSpecialKey(int key, int x, int y) = 0;
	virtual void OnMouse(int button, int state, int x, int y) = 0;
	virtual void OnMotion(int x, int y) = 0;
	virtual void OnIdle() = 0;

	virtual ~IApplicationListener() = default;
};

// Класс, содержащий реализацию методов слушателя, используемую по умолчанию
template <class Base>
class CApplicationListenerImpl : public Base
{
	virtual void OnDisplay()
	{
	}

	virtual void OnReshape(int width, int height)
	{
		height;
	}

	virtual void OnKeyboard(unsigned char key, int x, int y)
	{
		key;
		x;
		y;
	}

	virtual void OnSpecialKey(int key, int x, int y)
	{
		key;
		x;
		y;
	}

	virtual void OnMouse(int button, int state, int x, int y)
	{
		button;
		state;
		x;
		y;
	}

	virtual void OnMotion(int x, int y)
	{
		x;
		y;
	}

	virtual void OnIdle()
	{
	}
};

typedef CApplicationListenerImpl<IApplicationListener> CApplicationListener;
