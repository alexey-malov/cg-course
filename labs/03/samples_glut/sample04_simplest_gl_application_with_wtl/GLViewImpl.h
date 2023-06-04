#pragma once

#include "OpenGL.h"

template <class T, class TBase = CWindow, class TWinTraits = CControlWinTraits>
class ATL_NO_VTABLE CGLViewImpl : public ATL::CWindowImpl<T, TBase, TWinTraits>
{
public:
	
	CGLViewImpl(bool needDepth = true, bool needStencil = false)
		:m_needDepth(needDepth)	// Запоминаем параметры буфера кадра
		,m_needStencil(needStencil)
	{

	}

	DECLARE_WND_CLASS(NULL);

	// Обрабатываем минимум необходимых сообщений
	BEGIN_MSG_MAP(CGLViewImpl)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	END_MSG_MAP()
protected:
	// Данный метод должен быть перегружен в классах-наследниках
	virtual void DrawScene() = 0;

	virtual void AfterCreate()
	{
		// Ничего не делаем - можно перегрузить в классе наследнике
	}

	virtual void BeforeDestroy()
	{
		// Ничего не делаем - можно перегрузить в классе наследнике
	}
private:
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CPaintDC dc(*this);

		// Если OpenGL был проинициализирован, то вызываем метод рисования сцены
		if (m_pGL.get())
		{
			DrawScene();
			m_pGL->SwapBuffers();
		}
		return 0;
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// Инициализируем OpenGL
		m_pGL.reset(new COpenGL(*this, m_needDepth, m_needStencil));
		AfterCreate();
		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		BeforeDestroy();
		// Деинициализируем OpenGL
		if (m_pGL.get())
		{
			m_pGL.release();
		}
		return 0;
	}

	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// ничего не делаем, сообщая системе о том, что очистка
		// клиентской области окна проведена
		return 0;
	}
	
private:

	std::auto_ptr<COpenGL> m_pGL;
	bool m_needDepth;
	bool m_needStencil;
};
