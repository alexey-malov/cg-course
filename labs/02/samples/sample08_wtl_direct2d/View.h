// View.h : interface of the CView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Particle.h"

class View : public ATL::CWindowImpl<View>
{
public:
	DECLARE_WND_CLASS(NULL)

	View();

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(View)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_SIZE(OnSize)
		MSG_WM_TIMER(OnTimer)
	END_MSG_MAP()

private:
	using Clock = std::chrono::high_resolution_clock;

	void CreateDeviceResources();
	void DiscardDeviceResources();
	void DrawWorld() const;

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnPaint(WTL::CDCHandle unused);
	void OnSize(UINT nType, CSize size);
	void OnTimer(UINT_PTR nIDEvent);
	void DrawFPS() const;

	ATL::CComPtr<ID2D1Factory> m_d2dFactory;
	ATL::CComPtr<IDWriteFactory> m_dwriteFactory;
	ATL::CComPtr<IWICImagingFactory> m_WICFactory;
	ATL::CComPtr<IDWriteTextFormat> m_textFormat;
	ATL::CComPtr<IDWriteTextFormat> m_textFormat1;
	ATL::CComPtr<IDWriteTextFormat> m_textFormat2;
	
	ATL::CComPtr<ID2D1HwndRenderTarget> m_renderTarget;
	ATL::CComPtr<ID2D1SolidColorBrush> m_brush;
	ATL::CComPtr<ID2D1SolidColorBrush> m_textBrush;

	World m_world{ 1000, { .width = 800, .height = 500 }, { 150, 150 } };

	Clock::time_point m_lastTick = Clock::now();
	Clock::time_point m_lastFpsTick = m_lastTick;
	int m_frameCount = 0;
	double m_fps = 0;
};
