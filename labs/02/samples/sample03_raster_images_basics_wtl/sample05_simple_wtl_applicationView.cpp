// sample05_simple_wtl_applicationView.cpp : implementation of the CSample05_simple_wtl_applicationView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "sample05_simple_wtl_applicationView.h"



BOOL CSample05_simple_wtl_applicationView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

using namespace Gdiplus;

CSample05_simple_wtl_applicationView::CSample05_simple_wtl_applicationView()
{
	Image img(L"car.jpg");

	if (img.GetLastStatus() != Ok)
	{
		throw std::runtime_error("Failed to open image");
	}

	// создаем Bitmap необходимых размеров
	m_pBitmap = std::auto_ptr<Bitmap>(new Bitmap(
		img.GetWidth(), img.GetHeight()));

	// рисуем в Bitmap содержимое img
	Graphics g(m_pBitmap.get());
	g.DrawImage(&img, 0, 0);
}

LRESULT CSample05_simple_wtl_applicationView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	// создаем Graphics для рисования в контексте устройства
	Graphics g(dc);
	
	// рисуем изображение
	g.DrawImage(m_pBitmap.get(), 20, 20);

	return 0;
}
