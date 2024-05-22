// 01_raytrace_baseView.cpp : implementation of the CRaytraceView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RaytraceView.h"
#include "CheckerShader.h"
#include "FrameBuffer.h"
#include "OmniLightSource.h"
#include "Plane.h"
#include "SceneObject.h"
#include "SimpleDiffuseShader.h"
#include "Sphere.h"

CRaytraceView::CRaytraceView()
	: m_pFrameBuffer(std::make_unique<CFrameBuffer>(800, 600))
{
	/*
	Задаем цвет заднего фона сцены
	*/
	m_scene.SetBackdropColor(CVector4f(1, 0, 1, 1));

	auto plane = std::make_shared<CPlane>(0, 1, 0, 1); // Уравнение плоскости y = -1

	auto checkerShader = std::make_shared<CCheckerShader>();
	{
		// Задаем смещение текстурных координат в 1/2 размера шахматного кубика для того чтобы избежать
		// визуальных артефактов при определении цвета клетки, связанных с погрешностями вычислений
		CMatrix4d checkerShaderTransform;
		checkerShaderTransform.Translate(0.25, 0.25, 0.25);
		checkerShader->SetTextureTransform(checkerShaderTransform);
	}

	/*
	Добавляем бесконечную шахматную плоскость на сцену
	*/
	m_scene.AddObject(std::make_shared<CSceneObject>(std::move(plane), std::move(checkerShader)));

	// Создаем и добавляем в сцену сферу, имеющую заданный материал
	{
		/*
		Матрица трансформации сферы 1
		*/
		CMatrix4d sphereTransform;
		sphereTransform.Translate(2, 0, -5);
		auto sphere1 = std::make_shared<CSphere>(1.5); // Создаем сферу радиуса 1.5
		sphere1->SetTransform(sphereTransform);

		/*
		Материал сферы 1
		*/
		CSimpleMaterial material1;
		material1.SetDiffuseColor(CVector4f(1, 1, 0, 1));

		// Шейдер сферы 1
		auto sphere1Shader = std::make_shared<CSimpleDiffuseShader>();
		sphere1Shader->SetMaterial(material1);
		m_scene.AddObject(std::make_shared<CSceneObject>(std::move(sphere1), std::move(sphere1Shader)));
	}

	// Создаем и добавляем в сцену сферу, имеющую заданный материал
	{
		/*
		Матрица трансформации сферы 2
		*/
		CMatrix4d sphereTransform;
		sphereTransform.Translate(0, -0.5, -3);
		sphereTransform.Scale(1, 0.4, 1);
		auto sphere2 = std::make_shared<CSphere>(0.5);
		sphere2->SetTransform(sphereTransform);

		/*
		Материал сферы 2
		*/
		CSimpleMaterial material2;
		material2.SetDiffuseColor(CVector4f(0.3f, 0.5f, 0.4f, 1));

		// Шейдер сферы 2
		auto sphere2Shader = std::make_shared<CSimpleDiffuseShader>();
		sphere2Shader->SetMaterial(material2);
		m_scene.AddObject(std::make_shared<CSceneObject>(std::move(sphere2), std::move(sphere2Shader)));
	}

	// Создаем и добавляем в сцену точечный источник света
	{
		auto light = std::make_shared<COmniLightSource>(CVector3d(-4.0, 4.0, 2.0));
		light->SetDiffuseIntensity(CVector4f(1, 1, 1, 1));
		m_scene.AddLightSource(std::move(light));
	}

	/*
	Задаем параметры видового порта и матрицы проецирования в контексте визуализации
	*/
	m_context.SetViewPort(CViewPort(0, 0, 800, 600));
	CMatrix4d proj;
	proj.LoadPerspective(60, 800.0 / 600.0, 0.1, 10);
	m_context.SetProjectionMatrix(proj);
}

CRaytraceView::~CRaytraceView()
{
	// Необходимо остановить фоновую работу объекта CRenderer до разрушения
	// данных класса CRaytraceView, т.к. CRenderer использует для своей работы
	// другие объекты, в частности, буфер кадра, разрушать которые можно только
	// после остановки CRenderer
	m_renderer.Stop();
}

BOOL CRaytraceView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

LRESULT CRaytraceView::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// Сообщаем системе, что дальнейшая очистка буфера не требуется
	return 1;
}

LRESULT CRaytraceView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	// Получаем размеры клиентской области окна
	CRect clientRect;
	GetClientRect(clientRect);
	int clientWidth = clientRect.Width();
	int clientHeight = clientRect.Height();

	// Кисть, используемая для очистки окна
	CBrush whiteBrush;
	whiteBrush.CreateSolidBrush(0xffffff);

	// Проверка на случай отсутствия буфера кадра
	if (m_pFrameBuffer.get())
	{
		int w = m_pFrameBuffer->GetWidth();
		int h = m_pFrameBuffer->GetHeight();
		// Рисуем буфер кадра в левом верхнем углу
		// клиентской области окна
		DrawFrameBuffer(dc, 0, 0);

		// Т.к. мы отключили очистку заднего фона экрана,
		// область справа и снизу от изображения придется
		// очистить вручную

		// Очищаем правую часть клиентской области
		if (w < clientWidth)
		{
			dc.FillRect(CRect(w, 0, clientWidth, h), whiteBrush);
		}

		// Очищаем нижную часть клиентской области
		if (h < clientHeight)
		{
			dc.FillRect(CRect(0, h, clientWidth, clientHeight), whiteBrush);
		}
	}
	else // Буфер кадра по каким-то причинам не определен
	{
		ATLASSERT(!"Something bad with the frame buffer");
	}

	return 0;
}

void CRaytraceView::DrawFrameBuffer(CDC& dc, int x, int y)
{
	int w = m_pFrameBuffer->GetWidth();
	int h = m_pFrameBuffer->GetHeight();

	// Заполняем структуру BITMAPINFO (см. MSDN), чтобы
	// перекинуть содержимое буфера кадра на экран без создания дополнительных bitmap-ов
	BITMAPINFO bmpInfo;
	memset(&bmpInfo, 0, sizeof(bmpInfo));
	BITMAPINFOHEADER& hdr = bmpInfo.bmiHeader;
	hdr.biSize = sizeof(hdr);
	hdr.biWidth = w;
	// По умолчанию BMP файл хранится "вверх ногами" (сначала нижний ряд пикселей)
	// Чтобы использовать привычный вариант хранения пикселей высота должна быть отрицательной
	hdr.biHeight = -h;
	hdr.biPlanes = 1; // Количество цветовых плоскостей в изображении
	hdr.biBitCount = sizeof(std::uint32_t) << 3; // Цвет каждого пикселя занимает 32 бита
	hdr.biCompression = BI_RGB;
	hdr.biSizeImage = w * h * static_cast<int>(sizeof(std::uint32_t));

	// Используя функцию SetDIBitsToDevice (см. MSDN) перекидываем массив пикселей
	// из памяти в контекст устройства
	dc.SetDIBitsToDevice(
		x, y, // Координаты вывода внутри контекста устройства
		w, h, // Ширина и высота изображений
		0, 0, // Координаты рисуемой области изображения
		0, h, // Номер начальной строки и количество строк
		m_pFrameBuffer->GetPixels(), // Адрес пикселей
		&bmpInfo, // Адрес информации о пикселях
		DIB_RGB_COLORS); // сигнализируем о том, что значения в таблице
	// bmpInfo.bmiColors интерпретируются как RGB значения,
	// а не индексы логической палитры
}

LRESULT CRaytraceView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// Запускаем процесс визуализации
	m_renderer.Render(m_scene, m_context, *m_pFrameBuffer);

	// Инициализируем таймер для отображения построенного изображения
	SetTimer(FRAMEBUFFER_UPDATE_TIMER, 500);
	return 0;
}

bool CRaytraceView::UpdateFrameBuffer()
{
	// Инициируем перерисовку окна
	RedrawWindow();

	unsigned totalChunks = 0;
	unsigned renderedChunks = 0;

	// Возвращаем true, если изображение в буфере кадра полностью построено
	return m_renderer.GetProgress(renderedChunks, totalChunks);
}

LRESULT CRaytraceView::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	WPARAM timerId = wParam;
	switch (timerId)
	{
	case FRAMEBUFFER_UPDATE_TIMER:
		// Если выяснилось, что изображение построено, выключаем таймер,
		// перекидывающий буфер кадра в окно
		if (UpdateFrameBuffer())
		{
			KillTimer(FRAMEBUFFER_UPDATE_TIMER);
		}
		break;
	}

	return 0;
}
