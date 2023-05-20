#pragma once

#include "common_inc.h"

class CAnimationController
{
public:
	CAnimationController();

	// Выполняет сброс контроллера
	void Reset();
	// Данный метод следует вызывать в начале каждого кадра
	void Tick();
	// Возвращает промежуток времени (в мс), прошедший с момента
	// предыдущего кадра
	ULONGLONG GetTimeDelta() const;

private:
	ULONGLONG m_lastTick;
	ULONGLONG m_delta;
};
