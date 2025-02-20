#pragma once
#include <atlcrack.h>

class CView : public ATL::CWindowImpl<CView>
{
public:
	DECLARE_WND_CLASS(NULL)

	CView()
	{
		ATLENSURE_SUCCEEDED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_d2dFactory));
	}

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CView)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_SIZE(OnSize)
	END_MSG_MAP()
private:
	struct DeviceResources
	{
		DeviceResources() = default;
		DeviceResources(ID2D1Factory& factory, HWND hWnd)
		{
			CRect clientRect;
			ATLENSURE(::GetClientRect(hWnd, &clientRect));

			auto size = D2D1::SizeU(clientRect.Width(), clientRect.Height());
			ATLENSURE_SUCCEEDED(factory.CreateHwndRenderTarget(
				D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hWnd, size),
				&renderTarget));
		}
		explicit operator bool() const { return renderTarget; }
		ATL::CComPtr<ID2D1HwndRenderTarget> renderTarget;
	};

	void OnPaint(WTL::CDCHandle /*dc*/) noexcept
	{
		ValidateRect(nullptr);
		try
		{

			for (;;)
			{
				CreateDeviceResources();

				auto& rt = m_deviceResources.renderTarget;
				rt->BeginDraw();
				rt->Clear(D2D1::ColorF(D2D1::ColorF::Red));

				if (HRESULT hr = rt->EndDraw(); hr != D2DERR_RECREATE_TARGET)
				{
					break;
				}
				DiscardDeviceResources();
			}
		}
		catch (...)
		{
		}
	}

	void OnSize(UINT /*nType*/, CSize size)
	{
		if (auto& rt = m_deviceResources.renderTarget)
		{
			ATLENSURE_SUCCEEDED(rt->Resize(D2D1::SizeU(size.cx, size.cy)));
		}
	}

	void CreateDeviceResources()
	{
		if (m_deviceResources)
		{
			return;
		}
		m_deviceResources = { *m_d2dFactory, m_hWnd };
	}

	void DiscardDeviceResources() noexcept
	{
		m_deviceResources = {};
	}

	ATL::CComPtr<ID2D1Factory> m_d2dFactory;
	DeviceResources m_deviceResources;
};
