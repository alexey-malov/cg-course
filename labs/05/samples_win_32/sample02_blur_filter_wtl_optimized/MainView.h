// sample01_blur_filter_wtlView.h : interface of the CMainView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CMainView : public CWindowImpl<CMainView>
{
public:
	CMainView();

	DECLARE_WND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CMainView)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
private:
	// Исходное изображение
	std::auto_ptr<Gdiplus::Bitmap> m_pSrcPicture;

	// Обработанное изображение
	std::auto_ptr<Gdiplus::Bitmap> m_pDstPicture;

	

};
