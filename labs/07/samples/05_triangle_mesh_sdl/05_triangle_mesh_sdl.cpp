// 02_raytrace_plane_sdl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <SDL.h>
#include "Renderer.h"
#include "FrameBuffer.h"
#include "RenderContext.h"
#include "Scene.h"
#include "CheckerShader.h"
#include "Plane.h"
#include "SceneObject.h"
#include "Sphere.h"
#include "SimpleMaterial.h"
#include "SimpleDiffuseShader.h"
#include "OmniLightSource.h"
#include "ConicCylinder.h"
#include "VectorMath.h"
#include "TriangleMesh.h"

using namespace boost::interprocess::ipcdetail;

class CApplication
{
public:
	CApplication()
		:m_frameBuffer(800, 600)
		,m_pMainSurface(NULL)
		,m_timerId(NULL)
		,m_mainSurfaceUpdated(0)
	{
		// Инициализация SDL (таймер и видео)
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
		SDL_WM_SetCaption("Raytracing example", NULL);

		// Сохраняем ссылку на поверхность, связанную с окном приложения
		m_pMainSurface = SDL_SetVideoMode(800, 600, 32, SDL_SWSURFACE | SDL_DOUBLEBUF);	//-V112


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
			0, 1, 0
			);
		m_context.SetModelViewMatrix(modelView);
	}

	// Запуск главного цикла приложения
	void MainLoop()
	{
		// Инициализация приложения
		Initialize();

		// Обновляем изначальное содержимое окна
		UpdateMainSurface();

		// Цикл обработки сообщений, продолжающийся пока не будет
		// получен запрос на завершение работы
		SDL_Event evt;
		while (SDL_WaitEvent(&evt) && evt.type != SDL_QUIT)
		{
			switch (evt.type)
			{
			case SDL_VIDEOEXPOSE:
				// Обновляем содержимое главного окна,
				// если оно нуждается в перерисовке
				UpdateMainSurface();
				break;
			}
		}

		// Деинициализация приложения
		Uninitialize();
	}

	~CApplication()
	{
		// Завершаем работу всех подсистем SDL
		SDL_Quit();
	}

private:

	// Добавляем бесконечную шахматную плоскость y = 0
	void AddSomePlane()
	{
		// Задаем смещение текстурных координат в 1/2 размера шахматного кубика для того чтобы избежать
		// визуальных артефактов при определении цвета клетки, связанных с погрешностями вычислений
		CMatrix4d checkerShaderTransform;
		checkerShaderTransform.Scale(0.2, 0.2, 0.2);
		checkerShaderTransform.Translate(0.25, 0.25, 0.25);
		AddPlane(CreateCheckerShader(checkerShaderTransform), 0, 1, 0, 0);
	}

	// Добавляем несколько сфер
	void AddSomeSpheres()
	{
		CSimpleMaterial yellow;
		yellow.SetDiffuseColor(CVector4f(1, 1, 0, 1));
		CSimpleDiffuseShader & shader = CreateSimpleDiffuseShader(yellow);
		AddSphere(shader, 1, CVector3d(0, 1, 0));
		AddSphere(shader, 0.5, CVector3d(2, 0, 0));
	}

	// Создаем и добавляем в сцену точечный источник света
	void AddSomeLight()
	{
		COmniLightPtr pLight(new COmniLightSource(CVector3d(-5, 5, 10)));
		pLight->SetDiffuseIntensity(CVector4f(1, 1, 1, 1));
		pLight->SetAttenuation(1, 0, 0.0005);
		m_scene.AddLightSource(pLight);
	}

	// Добавляем несколько конических цилиндров
	void AddSomeConicCylinders()
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
	void AddSomeTetrahedron()
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
		CTriangleMeshData * pMeshData = CreateTriangleMeshData(vertices, faces);

		CMatrix4d transform;
		transform.Translate(3, 0.3, -1);
		transform.Rotate(170, 0, 1, 0);
		CSimpleMaterial blue;
		blue.SetDiffuseColor(CVector4f(0.5f, 0.8f, 1, 1));

		AddTriangleMesh(CreateSimpleDiffuseShader(blue), pMeshData, transform);
	}


	CSceneObject & AddSphere(IShader const& shader, double radius = 1, CVector3d const& center = CVector3d(), CMatrix4d const& transform = CMatrix4d())
	{
		CSphere * pSphere = new CSphere(radius, center, transform);
		m_geometryObjects.push_back(pSphere);

		return AddSceneObject(*pSphere, shader);
	}

	CSceneObject & AddConicCylinder(IShader const& shader, double height = 1, double baseRadius = 1, double capRadius = 0, CMatrix4d const& transform = CMatrix4d())
	{
		CConicCylinder * pConicCylinder = new CConicCylinder(height, baseRadius, capRadius, transform);
		m_geometryObjects.push_back(pConicCylinder);
		return AddSceneObject(*pConicCylinder, shader);
	}

	CSceneObject & AddPlane(IShader const& shader, double a, double b, double c, double d, CMatrix4d const& transform = CMatrix4d())
	{
		CPlane * pPlane = new CPlane(a, b, c, d, transform);
		m_geometryObjects.push_back(pPlane);
		return AddSceneObject(*pPlane, shader);
	}

	CSceneObject & AddTriangleMesh(IShader const& shader, CTriangleMeshData const* pMeshData, CMatrix4d const& transform)
	{
		CTriangleMesh * pMesh = new CTriangleMesh(pMeshData, transform);
		m_geometryObjects.push_back(pMesh);
		return AddSceneObject(*pMesh, shader);
	}

	CSceneObject & AddSceneObject(IGeometryObject const& object, IShader const& shader)
	{
		CSceneObjectPtr pObject(new CSceneObject(object, shader));
		m_scene.AddObject(pObject);
		return *pObject;
	}

	CSimpleDiffuseShader & CreateSimpleDiffuseShader(CSimpleMaterial const& material)
	{
		CSimpleDiffuseShader * pShader = new CSimpleDiffuseShader(material);
		m_shaders.push_back(pShader);
		return *pShader;
	}

	CCheckerShader & CreateCheckerShader(CMatrix4d const& textureTransform)
	{
		CCheckerShader * pShader = new CCheckerShader(textureTransform);
		m_shaders.push_back(pShader);
		return *pShader;
	}

	CTriangleMeshData * CreateTriangleMeshData(std::vector<Vertex> const& vertices, std::vector<Face> const& faces)
	{
		CTriangleMeshData * pMeshData = new CTriangleMeshData(vertices, faces);
		m_triangleMeshDataObjects.push_back(pMeshData);
		return pMeshData;
	}

	void Initialize()
	{
		// Запускаем построение изображения и таймер обновления экрана
		m_renderer.Render(m_scene, m_context, m_frameBuffer);
		m_timerId = SDL_AddTimer(500, &TimerCallback, this);
	}

	void Uninitialize()
	{
		// Останавливаем таймер обновления экрана и построение изображения
		SDL_RemoveTimer(m_timerId);
		m_renderer.Stop();
	}

	// Обновление содержимого окна приложения
	void UpdateMainSurface()
	{
		// Копируем содержимое буфера кадра в теневой буфер окна приложения
		CopyFrameBufferToSDLSurface();

		// Обмениваем теневой буфер и видимый, делая видимым текущее состояние буфера кадра
		SDL_Flip(m_pMainSurface);

		// Устанавливаем флаг: "Поверхность обновлена"
		atomic_write32(&m_mainSurfaceUpdated, 1);
	}

	// Копирование буфера кадра в область главного окна
	void CopyFrameBufferToSDLSurface()
	{
		// Получаем доступ к пикселям поверхности окна приложения
		SDL_LockSurface(m_pMainSurface);

		// В нашем приложении для простоты поддерживается только формат 32 бита на пиксель
		const SDL_PixelFormat * pixelFormat = m_pMainSurface->format;
		if (pixelFormat->BitsPerPixel == 32)
		{
			// Получаем смещения битов красной, зеленой, синей и альфа составляющих
			const Uint8 rShift = pixelFormat->Rshift;
			const Uint8 gShift = pixelFormat->Gshift;
			const Uint8 bShift = pixelFormat->Bshift;
			const Uint8 aShift = pixelFormat->Ashift;
			// Маска альфа-канала (может быть равна нулю, если поверхность не имеет альфа канала)
			const Uint32 aMask = pixelFormat->Amask;

			// Размеры буфера кадра
			const unsigned h = m_frameBuffer.GetHeight();
			const unsigned w = m_frameBuffer.GetWidth();

			// Адрес начала пиксельных данных поверхности
			Uint8 * pixels = reinterpret_cast<Uint8*>(m_pMainSurface->pixels);
			for (unsigned y = 0; 
				y < h; 
				++y, 
				pixels += m_pMainSurface->pitch)	// перемещаемся на адрес следующей строки в конце каждой итерации цикла
			{
				// Получаем адреса начала текущей строки в буфере кадра и поверхости окна
				boost::uint32_t const * srcLine = m_frameBuffer.GetPixels(y);
				Uint32 * dstLine = reinterpret_cast<Uint32*>(pixels);

				// Совпадает ли формат пикселей поверхности с форматом
				// ARGB или XRGB, используемом в буфере кадра?
				if (bShift == 0 && gShift == 8 && rShift == 16)
				{
					// Совпадает, просто копируем пиксели строки как они есть
					memcpy(dstLine, srcLine, w * sizeof(Uint32));
				}
				else
				{
					// Формат не совпадает, придется конвертировать цвет пикселя
					// из формата буфера кадра в формат пикселей поверхности
					for (unsigned x = 0; x < w; ++x)
					{
						boost::uint32_t srcColor = srcLine[x];	//-V108
						Uint32 dstColor =
							((srcColor & 0xff) << bShift) |
							(((srcColor >> 8) & 0xff) << gShift) |
							(((srcColor >> 16) & 0xff) << rShift) |
							((((srcColor >> 24) ) << aShift) & aMask);
						dstLine[x] = dstColor;	//-V108
					}
				}
			}
		}

		// Завершение прямого доступа к пикселям поверхности
		SDL_UnlockSurface(m_pMainSurface);
	}

	// Обработчик таймера, вызываемый SDL
	static Uint32 SDLCALL TimerCallback(Uint32 interval, void *param)
	{
		// Перенаправляем вызов таймера в метод класса OnTimer
		CApplication * pMyApp = reinterpret_cast<CApplication*>(param);
		return pMyApp->OnTimer(interval);
	}

	// Обработчик таймера SDL объекта CApplication
	Uint32 OnTimer(Uint32 interval)
	{
		// Проверяем, завершено ли построение изображения
		unsigned renderedChunks = 0;
		unsigned totalChunks = 0;
		if (m_renderer.GetProgress(renderedChunks, totalChunks))
		{

			// Выводим в stdout время построеия кадра
			std::cout << "Rendering time: " << m_renderer.GetRenderingTime() << "s.\n";

			// Изображение построено, обработчик вернет значение 0 
			// для остановки таймера
			interval = 0;
		}

		// В любом случае помечаем окно как требующее обновления
		InvalidateMainSurface();

		// Выходим из обработчика, сообщая SDL о необходимости повторить
		// вызов данного обработчика через указанное количество миллисекунд
		// Возврат нулевого значения из обработчика приведет к отключению
		// данного таймера
		return interval;
	}

	// Пометка содержимого окна, как нуждающейся в перерисовке
	void InvalidateMainSurface()
	{
		// Окно нуждается в перерисовке, если оно было обновлено
		bool redrawIsNeeded = atomic_read32(&m_mainSurfaceUpdated) == 1;

		if (redrawIsNeeded)
		{
			// Сбрасываем флаг "Поверхность обновлена"
			atomic_write32(&m_mainSurfaceUpdated, 0);

			// Добавляем событие о необходимости перересовки окна
			// в очередь событий SDL
			SDL_Event evt;
			evt.type = SDL_VIDEOEXPOSE;
			SDL_PushEvent(&evt);
		}
	}
private:
	// Буфер кадра
	CFrameBuffer m_frameBuffer;
	// Визуализатор
	CRenderer m_renderer;
	// Контекст визуализации
	CRenderContext m_context;
	CScene	m_scene;

	boost::ptr_vector<IShader> m_shaders;
	boost::ptr_vector<IGeometryObject> m_geometryObjects;
	boost::ptr_vector<CTriangleMeshData> m_triangleMeshDataObjects;

	// Поверхность окна приложения
	SDL_Surface * m_pMainSurface;
	// Идентификатор SDL-таймера
	SDL_TimerID m_timerId;
	// Обновлена ли поверхность окна приложения (1 - да, 0 - нет)
	boost::uint32_t m_mainSurfaceUpdated;
};

#ifdef __cplusplus
extern "C"
#endif
int main(int /*argc*/, char* /*argv*/[])
{
	CApplication app;
	app.MainLoop();
	return 0;
}
