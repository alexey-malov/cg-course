// sample05_simple_wtl_applicationView.h : interface of the CSample05_simple_wtl_applicationView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CSample05_simple_wtl_applicationView : public CWindowImpl<CSample05_simple_wtl_applicationView>
{
public:
	DECLARE_WND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CSample05_simple_wtl_applicationView)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};
