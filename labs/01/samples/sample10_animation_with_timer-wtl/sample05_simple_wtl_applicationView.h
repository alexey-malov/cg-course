// sample05_simple_wtl_applicationView.h : interface of the CSample05_simple_wtl_applicationView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CSample05_simple_wtl_applicationView : public CWindowImpl<CSample05_simple_wtl_applicationView>
{
	enum
	{
		ANIMATION_TIMER_ID = 1
	};

	static const double ROTATION_SPEED;
public:
	DECLARE_WND_CLASS(NULL)

	CSample05_simple_wtl_applicationView();

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CSample05_simple_wtl_applicationView)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
private:
	void Animate(void);

	double m_rotationAngle;
	DWORD m_lastTick;
	CPen m_pen;
};
