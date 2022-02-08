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

LRESULT CSample05_simple_wtl_applicationView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	//TODO: Add your drawing code here

	return 0;
}
