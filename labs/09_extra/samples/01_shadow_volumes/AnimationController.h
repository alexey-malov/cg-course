#pragma once

class CAnimationController
{
public:
	CAnimationController(void);
	~CAnimationController(void);
	// Выполняет сброс контроллера
	void Reset();
	// Данный метод следует вызывать в начале каждого кадра
	void Tick();
	// Возвращает промежуток времени (в мс), прошедший с момента
	// предыдущего кадра
	DWORD GetTimeDelta()const;
private:
	DWORD m_lastTick;
	DWORD m_delta;
};
