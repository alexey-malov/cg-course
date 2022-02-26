// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "MainFrm.h"

using namespace std;
using namespace Gdiplus;
const float CMainFrame::ACCELERATION = 100;
const float CMainFrame::MAX_SPEED = 500;

CMainFrame::CMainFrame()
{
	m_ballSpeed.X = 50;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	return CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle()
{
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	// Создаем внеэкранный буфер
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	m_pBackBuffer.reset(new Bitmap(screenWidth, screenHeight, PixelFormat32bppARGB));

	// Загружаем изображение мячика
	LoadBall();

	// Инициализируем таймер
	SetTimer(ANIMATION_TIMER, 20);
	m_lastTick = GetTickCount();

	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	KillTimer(ANIMATION_TIMER);

	bHandled = FALSE;
	return 1;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: add code to initialize document

	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainFrame::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	unsigned clientWidth = rcClient.Width();
	unsigned clientHeight = rcClient.Height();

	if (!m_pBackBuffer.get() ||
		(clientWidth > m_pBackBuffer->GetWidth()) ||
		(clientHeight > m_pBackBuffer->GetHeight())
		)
	{
		m_pBackBuffer.reset(new Bitmap(clientWidth, clientHeight, PixelFormat32bppARGB));
	}

	return 0;
}

LRESULT CMainFrame::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// ничего не делаем
	return 0;
}

LRESULT CMainFrame::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(*this);

	RedrawBackBuffer();
	
	Graphics g(dc);
	if (m_pBackBuffer.get())
	{
		// перекидываем внеэкранный буфер на экран
		g.DrawImage(m_pBackBuffer.get(), 0, 0);
	}
	
	return 0;
}

void CMainFrame::RedrawBackBuffer(void)
{
	Graphics g(m_pBackBuffer.get());

	// очищаем внеэкранный буфер
	g.Clear(Color(0xdd, 0xdd, 0xdd));

	g.DrawImage(m_pBall.get(), m_ballPosition);
}

void CMainFrame::LoadBall(void)
{
	Image img(L"ball.png");
	if (img.GetLastStatus() == Ok)
	{
		m_pBall.reset(new Bitmap(img.GetWidth(), img.GetHeight()));
		Graphics g(m_pBall.get());
		g.DrawImage(&img, 0, 0);
	}
}

LRESULT CMainFrame::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	UINT timerId = wParam;
	switch (timerId)
	{
	case ANIMATION_TIMER:
		Animate();
		break;
	}
	
	return 0;
}

void CMainFrame::Animate(void)
{
	DWORD currentTick = GetTickCount();
	float delta = (currentTick - m_lastTick) * 0.001f;
	m_lastTick = currentTick;

	CRect rc;
	GetClientRect(&rc);
	int w = rc.Width();
	int h = rc.Height();

	// размеры шара
	int bw = m_pBall->GetWidth();
	int bh = m_pBall->GetHeight();

	// рассчитываем движение шара по горизонтали
	m_ballPosition.X = min(w - bw, max(0, m_ballPosition.X + m_ballSpeed.X * delta));
	if (
		((m_ballSpeed.X >= 0) && (bw + m_ballPosition.X >= w))
		||
		((m_ballSpeed.X < 0) && (m_ballPosition.X <= 0))
		)
	{
		m_ballSpeed.X = -m_ballSpeed.X;	// отскакиваем от боков
	}

	// рассчитываем движение шара по вертикали
	m_ballSpeed.Y = max(-MAX_SPEED, min(MAX_SPEED, m_ballSpeed.Y + ACCELERATION * delta));
	m_ballPosition.Y = min(h - bh, m_ballPosition.Y + m_ballSpeed.Y * delta);
	if ((m_ballSpeed.Y > 0) && (m_ballPosition.Y >= h - bh))
	{
		m_ballSpeed.Y = -m_ballSpeed.Y;	// отскакиваем от пола
	}

	InvalidateRect(NULL);
}
