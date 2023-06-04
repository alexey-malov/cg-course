#pragma once
#include "glviewimpl.h"

class CMainView : public CGLViewImpl<CMainView>
{
	enum
	{
		ANIMATION_TIMER = 1
	};
public:
	DECLARE_WND_CLASS(NULL)

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CMainView)
		MSG_WM_SIZE(OnSize)
		MSG_WM_TIMER(OnTimer);
		// вызываем карту сообщений родительского окна
		CHAIN_MSG_MAP(CGLViewImpl<CMainView>) 
	END_MSG_MAP()
protected:
	// Обновляем содержимое буфера кадра
	virtual void DrawScene();
	virtual void AfterCreate();
	virtual void BeforeDestroy();
private:
	void OnSize(UINT uMsg, CSize size);
	void OnTimer(UINT_PTR nIDEvent);
	
	DWORD m_lastTick;
};
