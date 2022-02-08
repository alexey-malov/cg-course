
// SimpleView.cpp : implementation of the CSimpleView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "sample04_simple_mfc_application.h"
#endif

#include "SimpleDoc.h"
#include "SimpleView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSimpleView

IMPLEMENT_DYNCREATE(CSimpleView, CView)

BEGIN_MESSAGE_MAP(CSimpleView, CView)
END_MESSAGE_MAP()

// CSimpleView construction/destruction

CSimpleView::CSimpleView() noexcept
{
	// TODO: add construction code here

}

CSimpleView::~CSimpleView()
{
}

BOOL CSimpleView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CSimpleView drawing

void CSimpleView::OnDraw(CDC* pDC)
{
	CSimpleDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// создаем перо и кисть с использованием классов библиотеки MFC
	CPen pen(PS_SOLID, 2, RGB(255, 0, 0));
	CBrush brush(RGB(255, 255, 0));

	// Выбираем их в контекст устройства
	CPen* pOldPen = pDC->SelectObject(&pen);
	CBrush* pOldBrush = pDC->SelectObject(&brush);

	// Рисуем эллипс
	pDC->Ellipse(100, 50, 250, 150);

	// Выбираем прежние объекты
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);

	// деструкторы CPen и CBrush автоматически уничтожат объекты
	// pen и brush при выходе из данной функции
}


// CSimpleView diagnostics

#ifdef _DEBUG
void CSimpleView::AssertValid() const
{
	CView::AssertValid();
}

void CSimpleView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSimpleDoc* CSimpleView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSimpleDoc)));
	return (CSimpleDoc*)m_pDocument;
}
#endif //_DEBUG


// CSimpleView message handlers
