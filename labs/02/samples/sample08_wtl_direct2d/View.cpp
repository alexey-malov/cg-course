// View.cpp : implementation of the CView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "View.h"

View::View()
{
	ATLENSURE_SUCCEEDED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_d2dFactory));
	ATLENSURE_SUCCEEDED(CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		reinterpret_cast<LPVOID*>(&m_WICFactory)));
	ATLENSURE_SUCCEEDED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(m_dwriteFactory),
		reinterpret_cast<IUnknown**>(&m_dwriteFactory)));
	ATLENSURE_SUCCEEDED(m_dwriteFactory->CreateTextFormat(
		L"Verdana",
		nullptr, // Font collection
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		18.0f, // Font size
		L"", // locale
		&m_textFormat));
}

BOOL View::PreTranslateMessage([[maybe_unused]] MSG* pMsg)
{
	return FALSE;
}

int View::OnCreate([[maybe_unused]] LPCREATESTRUCT lpCreateStruct)
{
	CreateDeviceResources();

	SetTimer(1, 0);

	return 0;
}

void View::OnPaint([[maybe_unused]] WTL::CDCHandle unused)
{
	// Помечаем всю область окна как валидную (не нужно вызывать WM_PAINT)
	ValidateRect(nullptr);
	++m_frameCount;

	try
	{
		CreateDeviceResources();

		m_renderTarget->BeginDraw();
		m_renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		DrawWorld();
		DrawFPS();

		if (HRESULT hr = m_renderTarget->EndDraw(); hr == D2DERR_RECREATE_TARGET)
		{
			DiscardDeviceResources();
		}
	}
	catch (...)
	{
	}
}

void View::CreateDeviceResources()
{
	if (m_renderTarget)
	{
		return;
	}

	CRect clientRect;
	GetClientRect(&clientRect);

	auto size = D2D1::SizeU(clientRect.Width(), clientRect.Height());
	ATLENSURE_SUCCEEDED(m_d2dFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hWnd, size),
		&m_renderTarget));

	ATLENSURE_SUCCEEDED(m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &m_brush));
	ATLENSURE_SUCCEEDED(m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_textBrush));
}

void View::OnSize([[maybe_unused]] UINT nType, CSize size)
{
	try
	{
		if (m_renderTarget)
		{
			std::ignore = m_renderTarget->Resize(D2D1::SizeU(size.cx, size.cy));
		}
	}
	catch (...)
	{
	}
}

void View::OnTimer([[maybe_unused]] UINT_PTR nIDEvent)
{
	using namespace std::literals;
	using DoubleSeconds = std::chrono::duration<double>;

	const auto currentTick = Clock::now();

	{
		const DoubleSeconds delta = currentTick - m_lastTick;
		m_world.Update(delta.count(), { 0, 30.0 });
	}

	if (const auto delta = currentTick - m_lastFpsTick; delta > 1s)
	{
		m_fps = m_frameCount / DoubleSeconds{delta}.count();
		m_lastFpsTick = currentTick;
		m_frameCount = 0;
	}

	m_lastTick = currentTick;

	Invalidate(/*erase = */ FALSE);
}

void View::DrawFPS() const
{
	CRect clientRect;
	GetClientRect(&clientRect);
	std::wstring fpsStr = std::format(L"FPS: {:.2f}", m_fps);
	m_renderTarget->DrawText(fpsStr.c_str(), static_cast<int>(fpsStr.length()), m_textFormat,
		D2D1::RectF(10.0f, 10.0f, 310.0, 30), m_textBrush);
}

void View::DrawWorld() const
{
	m_renderTarget->DrawRectangle(
		D2D1::RectF(0, 0, static_cast<float>(m_world.GetSize().width), static_cast<float>(m_world.GetSize().height)),
		m_brush);

	for (const auto& particle : m_world.GetParticles())
	{
		m_renderTarget->FillEllipse(
			D2D1::Ellipse({ static_cast<float>(particle.position.x), static_cast<float>(particle.position.y) }, 5, 5),
			m_brush);
	}
}

void View::DiscardDeviceResources()
{
	m_renderTarget.Release();
	m_brush.Release();
	m_textBrush.Release();
}
