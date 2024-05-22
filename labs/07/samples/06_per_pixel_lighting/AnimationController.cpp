#include "StdAfx.h"
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
	m_lastTick = GetTickCount();
	m_delta = 0;
}

void CAnimationController::Tick()
{
	DWORD currentTick = GetTickCount();
	m_delta = currentTick - m_lastTick;
	m_lastTick = currentTick;
}

DWORD CAnimationController::GetTimeDelta()const
{
	return m_delta;
}
