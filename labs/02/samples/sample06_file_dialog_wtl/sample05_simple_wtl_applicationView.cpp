// sample05_simple_wtl_applicationView.cpp : implementation of the CSample05_simple_wtl_applicationView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "utils.h"

#include "sample05_simple_wtl_applicationView.h"


using namespace Gdiplus;
using namespace std;

BOOL CSample05_simple_wtl_applicationView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

CSample05_simple_wtl_applicationView::CSample05_simple_wtl_applicationView()
// инициализируем название и размер шрифта, а также его стили
{
}

LRESULT CSample05_simple_wtl_applicationView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	if (m_pPicture.get())
	{
		Graphics g(dc);

		CRect rc;
		GetClientRect(&rc);
		// центрируем картинку
		int x = (rc.Width() - (int)m_pPicture->GetWidth()) / 2;
		int y = (rc.Height() - (int)m_pPicture->GetHeight()) / 2;

		g.DrawImage(m_pPicture.get(), x, y);
	}
	return 0;
}

bool CSample05_simple_wtl_applicationView::OpenPicture(std::wstring const & fileName)
{
	Image img(fileName.c_str());
	if (img.GetLastStatus() == Ok)
	{
		m_pPicture.reset(new Bitmap(img.GetWidth(), img.GetHeight(), PixelFormat32bppARGB));
		
		Graphics g(m_pPicture.get());
		g.DrawImage(&img, 0, 0);

		InvalidateRect(NULL);
		return true;
	}

	return false;
	
}


bool CSample05_simple_wtl_applicationView::SavePicture(std::wstring const& fileName, int quality)
{
	if (m_pPicture.get())
	{
		return CUtils::SaveImage(fileName, *m_pPicture, quality);
	}

	return false;
}

void CSample05_simple_wtl_applicationView::ClosePicture(void)
{
	m_pPicture.release();
	InvalidateRect(NULL);
}
