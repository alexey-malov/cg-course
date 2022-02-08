
// SimpleView.h : interface of the CSimpleView class
//

#pragma once


class CSimpleView : public CView
{
protected: // create from serialization only
	CSimpleView() noexcept;
	DECLARE_DYNCREATE(CSimpleView)

// Attributes
public:
	CSimpleDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CSimpleView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SimpleView.cpp
inline CSimpleDoc* CSimpleView::GetDocument() const
   { return reinterpret_cast<CSimpleDoc*>(m_pDocument); }
#endif

