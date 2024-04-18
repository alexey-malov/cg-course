// 01_raytrace_baseView.cpp : implementation of the CRaytraceView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FrameBuffer.h"
#include "resource.h"

#include "CheckerShader.h"
#include "ConicCylinder.h"
#include "Intersection.h"
#include "OmniLightSource.h"
#include "Plane.h"
#include "Ray.h"
#include "RaytraceView.h"
#include "SceneObject.h"
#include "SimpleDiffuseShader.h"
#include "SimpleMaterial.h"
#include "Sphere.h"
#include "TriangleMesh.h"

CRaytraceView::CRaytraceView()
	: m_pFrameBuffer(std::make_unique<CFrameBuffer>(800, 600))
{
	/*
	Задаем цвет заднего фона сцены
	*/
	m_scene.SetBackdropColor(CVector4f(1, 0, 1, 1));

	AddSomePlane();

	AddSomeSpheres();

	AddSomeConicCylinders();

	AddSomeLight();

	AddSomeTetrahedron();

	/*
	Задаем параметры видового порта и матрицы проецирования в контексте визуализации
	*/
	m_context.SetViewPort(CViewPort(0, 0, 800, 600));
	CMatrix4d proj;
	proj.LoadPerspective(60, 800.0 / 600.0, 0.1, 10);
	m_context.SetProjectionMatrix(proj);

	// Задаем матрицу камеры
	CMatrix4d modelView;
	modelView.LoadLookAtRH(
		0, 3, 7,
		0, 0, 0,
		0, 1, 0);
	m_context.SetModelViewMatrix(modelView);
}

// Добавляем бесконечную шахматную плоскость y = 0
void CRaytraceView::AddSomePlane()
{
	// Задаем смещение текстурных координат в 1/2 размера шахматного кубика для того чтобы избежать
	// визуальных артефактов при определении цвета клетки, связанных с погрешностями вычислений
	CMatrix4d checkerShaderTransform;
	checkerShaderTransform.Scale(0.2, 0.2, 0.2);
	checkerShaderTransform.Translate(0.25, 0.25, 0.25);
	AddPlane(CreateCheckerShader(checkerShaderTransform), 0, 1, 0, 0);
}

// Добавляем несколько сфер
void CRaytraceView::AddSomeSpheres()
{
	CSimpleMaterial yellow;
	yellow.SetDiffuseColor(CVector4f(1, 1, 0, 1));
	CSimpleDiffuseShader& shader = CreateSimpleDiffuseShader(yellow);
	AddSphere(shader, 1, CVector3d(0, 1, 0));
	AddSphere(shader, 0.5, CVector3d(2, 0, 0));
}

// Создаем и добавляем в сцену точечный источник света
void CRaytraceView::AddSomeLight()
{
	COmniLightPtr pLight(new COmniLightSource(CVector3d(-5, 5, 10)));
	pLight->SetDiffuseIntensity(CVector4f(1, 1, 1, 1));
	pLight->SetAttenuation(1, 0, 0.0005);
	m_scene.AddLightSource(pLight);
}

// Добавляем несколько конических цилиндров
void CRaytraceView::AddSomeConicCylinders()
{
	CSimpleMaterial white;
	white.SetDiffuseColor(CVector4f(1, 1, 1, 1));

	CMatrix4d transform;
	transform.Translate(-2.5, 0, 0);
	transform.Rotate(-90, 1, 0, 0);

	AddConicCylinder(CreateSimpleDiffuseShader(white), 2, 1, 1, transform);

	CSimpleMaterial red;
	red.SetDiffuseColor(CVector4f(1, 0, 0, 1));
	CMatrix4d coneTransform;
	coneTransform.Translate(0, 0, 2);
	coneTransform.Rotate(-90, 1, 0, 0);

	AddConicCylinder(CreateSimpleDiffuseShader(red), 1, 1, 0, coneTransform);

	CMatrix4d conicFrustumTransform;
	conicFrustumTransform.Translate(4.0, 0.0, 0.0);
	conicFrustumTransform.Rotate(-90, 1, 0, 0);
	CSimpleMaterial green;
	green.SetDiffuseColor(CVector4f(0, 1, 0, 1));
	AddConicCylinder(CreateSimpleDiffuseShader(green), 1, 0.5, 0.3, conicFrustumTransform);
}

// Добавляем тетраэдр
void CRaytraceView::AddSomeTetrahedron()
{
	// Вершины
	CVector3d v0(-1, 0, 1);
	CVector3d v1(+1, 0, 1);
	CVector3d v2(0, 0, -1);
	CVector3d v3(0, 2, 0);
	std::vector<Vertex> vertices;
	vertices.push_back(Vertex(v0));
	vertices.push_back(Vertex(v1));
	vertices.push_back(Vertex(v2));
	vertices.push_back(Vertex(v3));

	// Грани
	std::vector<Face> faces;
	faces.push_back(Face(0, 2, 1));
	faces.push_back(Face(3, 0, 1));
	faces.push_back(Face(3, 1, 2));
	faces.push_back(Face(3, 2, 0));

	// Данные полигональной сетки
	CTriangleMeshData* pMeshData = CreateTriangleMeshData(vertices, faces);

	CMatrix4d transform;
	transform.Translate(3, 0.3, -1);
	transform.Rotate(170, 0, 1, 0);
	CSimpleMaterial blue;
	blue.SetDiffuseColor(CVector4f(0.5f, 0.8f, 1, 1));

	AddTriangleMesh(CreateSimpleDiffuseShader(blue), pMeshData, transform);
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
	if (m_statusBar)
	{
		m_statusBar.SetText(0, L"Rendering...");
	}
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

CSceneObject& CRaytraceView::AddSphere(IShader const& shader, double radius, CVector3d const& center, CMatrix4d const& transform)
{
	const auto& sphere = *m_geometryObjects.emplace_back(
		std::make_unique<CSphere>(radius, center, transform));

	return AddSceneObject(sphere, shader);
}

CSceneObject& CRaytraceView::AddConicCylinder(IShader const& shader, double height, double baseRadius, double capRadius, CMatrix4d const& transform)
{
	const auto& conicCylinder = *m_geometryObjects.emplace_back(
		std::make_unique<CConicCylinder>(height, baseRadius, capRadius, transform));
	return AddSceneObject(conicCylinder, shader);
}

CSceneObject& CRaytraceView::AddPlane(IShader const& shader, double a, double b, double c, double d, CMatrix4d const& transform)
{
	const auto& plane = *m_geometryObjects.emplace_back(
		std::make_unique<CPlane>(a, b, c, d, transform));
	return AddSceneObject(plane, shader);
}

CTriangleMeshData* CRaytraceView::CreateTriangleMeshData(std::vector<Vertex> const& vertices, std::vector<Face> const& faces)
{
	auto* meshData = m_triangleMeshDataObjects.emplace_back(std::make_unique<CTriangleMeshData>(vertices, faces)).get();
	return meshData;
}

CSceneObject& CRaytraceView::AddTriangleMesh(IShader const& shader, CTriangleMeshData const* pMeshData, CMatrix4d const& transform)
{
	const auto& mesh = *m_geometryObjects.emplace_back(std::make_unique<CTriangleMesh>(pMeshData, transform));
	return AddSceneObject(mesh, shader);
}

CSceneObject& CRaytraceView::AddSceneObject(IGeometryObject const& object, IShader const& shader)
{
	auto obj = std::make_shared<CSceneObject>(object, shader);
	m_scene.AddObject(obj);
	return *obj;
}

CSimpleDiffuseShader& CRaytraceView::CreateSimpleDiffuseShader(CSimpleMaterial const& material)
{
	auto shader = std::make_unique<CSimpleDiffuseShader>(material);
	auto& shaderRef = *shader;
	m_shaders.emplace_back(std::move(shader));
	return shaderRef;
}

CCheckerShader& CRaytraceView::CreateCheckerShader(CMatrix4d const& textureTransform)
{
	auto shader = std::make_unique<CCheckerShader>(textureTransform);
	auto& shaderRef = *shader;
	m_shaders.emplace_back(std::move(shader));
	return shaderRef;
}

void CRaytraceView::SetStatusBar(HWND statusBar)
{
	m_statusBar.Attach(statusBar);
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
