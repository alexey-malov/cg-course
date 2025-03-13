// View.h : interface of the CView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Cube.h"
#include "Renderer.h"

class CView : public CWindowImpl<CView>
{
public:
	DECLARE_WND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CView)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_SIZE(OnSize)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_TIMER(OnTimer)
	END_MSG_MAP()

private:
	using Clock = std::chrono::high_resolution_clock;

	int OnCreate(LPCREATESTRUCT)
	{
		SetTimer(1, 10);
		return 0;
	}

	void OnTimer(UINT_PTR /*nIDEvent*/)
	{
		using namespace std::chrono_literals;

		auto now = Clock::now();
		auto duration = now - m_lastTick;
		m_cubeTransform = glm::rotate(
			m_cubeTransform,
			duration * std::numbers::pi * 2 / 5s,
			{ 0.f, 1.f, 0.f });
		m_lastTick = now;
		Invalidate();
	}

	void OnSize(UINT /*nType*/, CSize size)
	{
		if (size.cx == 0 && size.cy == 0)
			return;
		m_renderer.SetViewPort({
			.left = 0,
			.top = 0,
			.width = size.cx,
			.height = size.cy,
		});

		m_renderer.SetProjectionMatrix(glm::perspective(60.0 * std::numbers::pi / 180.0,
			static_cast<double>(size.cx) / size.cy, 0.1, 10.0));
	}

	void OnPaint(WTL::CDCHandle /*unused*/)
	{
		WTL::CPaintDC dc(m_hWnd);

		auto cameraMatrix = glm::lookAt(
			glm::dvec3(0, 4, 3), // Положение камеры
			glm::dvec3(0, 0, 0), // Точка, в которую направлена камера
			glm::dvec3(0, 1, 0) // Направление "вверх"
		);
		m_renderer.SetModelViewMatrix(cameraMatrix * m_cubeTransform);
		RendererContext context(dc.m_hDC, m_renderer);
		m_cube.Draw(context);
	}

	Renderer m_renderer;

	Clock::time_point m_lastTick = Clock::now();
	glm::dmat4 m_cubeTransform{ 1.0 };
	Cube m_cube{ 1.0f, glm::vec4{ 0.f, 0.f, 0.f, 1.f } };
};
