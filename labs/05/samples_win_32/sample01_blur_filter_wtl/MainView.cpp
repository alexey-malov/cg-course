// sample01_blur_filter_wtlView.cpp : implementation of the CMainView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "MainView.h"
#include "BlurFilter.h"

using namespace Gdiplus;

CMainView::CMainView()
{
	Image img(L"car.jpg");
	if (img.GetLastStatus() == Ok)
	{
		m_pSrcPicture.reset(new Bitmap(img.GetWidth(), img.GetHeight(), img.GetPixelFormat()));

		Graphics g(m_pSrcPicture.get());
		g.DrawImage(&img, 0, 0, img.GetWidth(), img.GetHeight());

		CBlurFilter blur;

		m_pDstPicture = blur.ApplyFilter(*m_pSrcPicture.get());
	}
}

BOOL CMainView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

LRESULT CMainView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	// Если исходное и обработанное изображения заданы, то нарисуем их рядом друг с другом
	if ((m_pSrcPicture.get() != NULL) && (m_pDstPicture.get() != NULL))
	{
		Graphics g(dc);
		g.DrawImage(m_pSrcPicture.get(), 0, 0, m_pSrcPicture->GetWidth(), m_pSrcPicture->GetHeight());

		g.DrawImage(m_pDstPicture.get(), m_pSrcPicture->GetWidth(), 0, m_pDstPicture->GetWidth(), m_pDstPicture->GetHeight());
	}

	return 0;
}
