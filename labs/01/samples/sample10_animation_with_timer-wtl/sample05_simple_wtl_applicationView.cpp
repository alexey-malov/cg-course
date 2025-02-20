// sample05_simple_wtl_applicationView.cpp : implementation of the CSample05_simple_wtl_applicationView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "sample05_simple_wtl_applicationView.h"

static const double M_PI = 3.14159265;

const double CSample05_simple_wtl_applicationView::ROTATION_SPEED = 30;

CSample05_simple_wtl_applicationView::CSample05_simple_wtl_applicationView()
:m_rotationAngle(0)
{
	m_pen.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
}

BOOL CSample05_simple_wtl_applicationView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

LRESULT CSample05_simple_wtl_applicationView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	CPenHandle oldPen = dc.SelectPen(m_pen);

	CRect rc;
	GetClientRect(rc);

	int centerX = rc.Width() / 2;
	int centerY = rc.Height() / 2;

	double angleInRadians = m_rotationAngle * M_PI / 180.0;
	
	dc.MoveTo(centerX, centerY);
	dc.LineTo(
		int(centerX + 300 * cos(angleInRadians)), 
		int(centerY + 300 * sin(angleInRadians))
		);

	dc.Ellipse(
		int(centerX + 300 * cos(angleInRadians)) - 230,
		int(centerY + 300 * sin(angleInRadians)) - 230,
		int(centerX + 300 * cos(angleInRadians)) + 230,
		int(centerY + 300 * sin(angleInRadians)) + 230
		);
	

	dc.SelectPen(oldPen);

	return 0;
}

LRESULT CSample05_simple_wtl_applicationView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_lastTick = GetTickCount();
	SetTimer(ANIMATION_TIMER_ID, 20, NULL);
	return 0;
}

LRESULT CSample05_simple_wtl_applicationView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	KillTimer(ANIMATION_TIMER_ID);
	return 0;
}


LRESULT CSample05_simple_wtl_applicationView::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	UINT_PTR timerId = wParam;
	switch (timerId)
	{
	case ANIMATION_TIMER_ID:
		Animate();
		break;
	}
	return 0;
}

void CSample05_simple_wtl_applicationView::Animate(void)
{
	DWORD currentTick = GetTickCount();
	double delta = (currentTick - m_lastTick) * 0.001;
	m_lastTick = currentTick;

	m_rotationAngle += ROTATION_SPEED * delta;
	m_rotationAngle = fmod(m_rotationAngle, 360);

	Invalidate();
	UpdateWindow();
}
