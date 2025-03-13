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
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		COMMAND_ID_HANDLER_EX(ID_ANIMATION_DONOTANIMATE, OnAnimationDonotanimate)
		COMMAND_ID_HANDLER_EX(ID_ANIMATION_ANIMATE, OnAnimationAnimate)
	END_MSG_MAP()

private:
	using Clock = std::chrono::high_resolution_clock;

	struct DragState
	{
		CPoint prevMousePos;
	};

	std::optional<DragState> m_dragState;

	int OnCreate(LPCREATESTRUCT)
	{
		SetTimer(1, 10);
		return 0;
	}

	void OnAnimationDonotanimate(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
	{
		m_animate = false;
	}

	void OnAnimationAnimate(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
	{
		m_animate = true;
	}

	void OnMouseMove(UINT /*nFlags*/, CPoint point)
	{
		if (m_dragState)
		{
			CRect rc;
			ATLVERIFY(GetClientRect(&rc));
			const auto offset = point - m_dragState->prevMousePos;

			const double xAngle = offset.cy * std::numbers::pi / rc.Height();
			const double yAngle = offset.cx * std::numbers::pi / rc.Width();

			RotateObject(xAngle, yAngle);

			m_dragState->prevMousePos = point;

			Invalidate();
		}
	}

	void RotateObject(double xAngle, double yAngle)
	{
		auto invCubeTransform = glm::inverse(m_cubeTransform);
		glm::dvec3 xAxis = invCubeTransform[0];
		glm::dvec3 yAxis = invCubeTransform[1];

		m_cubeTransform = glm::rotate(m_cubeTransform, xAngle, xAxis);
		m_cubeTransform = glm::rotate(m_cubeTransform, yAngle, yAxis);

		const auto normalizedMatrix = glm::orthonormalize(glm::dmat3{ m_cubeTransform });
		m_cubeTransform = glm::dmat4{
			glm::dvec4{ normalizedMatrix[0], 0.0 },
			glm::dvec4{ normalizedMatrix[1], 0.0 },
			glm::dvec4{ normalizedMatrix[2], 0.0 },
			m_cubeTransform[3]
		};
	}

	void OnLButtonDown(UINT /*nFlags*/, CPoint point)
	{
		m_dragState = DragState{ .prevMousePos = point };
		SetCapture();
	}

	void OnLButtonUp(UINT /*nFlags*/, CPoint /*point*/)
	{
		m_dragState.reset();
		ReleaseCapture();
	}

	void OnTimer(UINT_PTR /*nIDEvent*/)
	{
		using namespace std::chrono_literals;

		const auto now = Clock::now();
		const auto duration = now - m_lastTick;
		if (m_animate)
		{
			m_cubeTransform = glm::rotate(
				m_cubeTransform,
				duration * std::numbers::pi * 2 / 5s,
				{ 0.f, 1.f, 0.f });
			Invalidate();
		}
		m_lastTick = now;
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
			glm::dvec3(0, 0, 3), // Положение камеры
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
	bool m_animate = false;
};
