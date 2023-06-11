#include "pch.h"
#include "AnimationController.h"

CAnimationController::CAnimationController(void)
{
	Reset();
}

CAnimationController::~CAnimationController(void)
{
}

void CAnimationController::Reset()
{
	m_lastTick = GetTickCount64();
	m_delta = 0;
}

void CAnimationController::Tick()
{
	ULONGLONG currentTick = GetTickCount64();
	m_delta = currentTick - m_lastTick;
	m_lastTick = currentTick;
}

ULONGLONG CAnimationController::GetTimeDelta()const
{
	return m_delta;
}

