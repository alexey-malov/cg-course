#pragma once
#include "GeometryObjectImpl.h"

/*
Структура, хранящая информацию о вершине полигональной сетки
*/
struct Vertex
{
	// Конструктор по умолчанию
	Vertex() {}

	// Конструктор с заданными параметрами
	Vertex(CVector3d const& p, CVector3d const& n = CVector3d())
		: position(p)
		, normal(n)
	{
	}

	CVector3d position; // Координаты вершины в пространстве
	CVector3d normal; // Координаты вектора нормали
};

/*
Структура, хранящая информацию о треугольной грани полигональонй сетки
*/
struct Face
{
	// Конструктор
	Face(unsigned i0, unsigned i1, unsigned i2, bool flat = true)
		: vertex0(i0)
		, vertex1(i1)
		, vertex2(i2)
		, isFlat(flat)
	{
	}

	// Индексы вершин грани
	unsigned vertex0, vertex1, vertex2;

	// Является ли грань плоской
	// Нормаль в любой точке плоской грани является постоянной
	// и не зависит от векторов нормалей ее вершин
	bool isFlat;
};

/*
Класс "Треугольник". Хранит подробную информацию о треугольной грани
*/
class CTriangle
{
public:
	// Консутруктор
	CTriangle(Vertex const& vertex0, Vertex const& vertex1, Vertex const& vertex2, bool flatShaded = true);

	// Ссылки на вершины треугольника
	Vertex const& GetVertex0() const { return *m_pVertex0; }
	Vertex const& GetVertex1() const { return *m_pVertex1; }
	Vertex const& GetVertex2() const { return *m_pVertex2; }

	// Возвращает коэффициенты уравнения плоскости треугольника
	CVector4d const& GetPlaneEquation() const
	{
		return m_planeEquation;
	}

	// Использует ли треугольник "плоское" освещение или же для
	// вычисления нормали используется интерполяция нормалей его вершин
	bool IsFlatShaded() const { return m_flatShaded; }

	// Проверка на столкновение луча с треугольником
	bool HitTest(
		CVector3d const& rayStart, // Точка испускания луча
		CVector3d const& rayDirection, // Направление луча
		double& hitTime, // Время столкновения луча с треугольником
		CVector3d& hitPoint, // Точка столкновения
		double& vertex0Weight, // Весовой коэффициент 0 вершины в точке столкновения
		double& vertex1Weight, // Весовой коэффициент 1 вершины в точке столкновения
		double& vertex2Weight, // Весовой коэффициент 2 вершины в точке столкновения
		double const& EPSILON = 1e-10 // Допустимая погрешность вычисления точки пересечения
	) const;

private:
	CVector4d m_planeEquation; // Уравшение плоскости

	// Перпендикуляры к ребрам треугольника
	CVector3d m_edge01Perp;
	CVector3d m_edge12Perp;
	CVector3d m_edge20Perp;

	// Обратные квадраты длин перпендикуляров
	double m_invEdge01PerpSquare;
	double m_invEdge12PerpSquare;
	double m_invEdge20PerpSquare;

	// Указатели на вершины треугольника (в массиве вершин)
	Vertex const* m_pVertex0;
	Vertex const* m_pVertex1;
	Vertex const* m_pVertex2;

	// Использует ли треугольник плоское освещение
	bool m_flatShaded;
};

/*
Класс-хранитель данных полигональной сетки (массивы вершин и треугольных граней).

Данные хранятся отдельно от использующих их сеток, что позволяет
экономить память на хранении данных: несколько полигональных сеток
могут ссылаться на одни и те же данные, но иметь разные трансформации.
*/
class CTriangleMeshData
{
public:
	CTriangleMeshData(
		std::vector<Vertex> vertices, // Вершины
		const std::vector<Face>& faces, // Грани
		bool normalize = false // Выполнить ли нормализацию нормалей вершин?
	);

	// Возвращает количество вершин
	size_t GetVertexCount() const { return m_vertices.size(); }
	// Адрес массива вершин
	Vertex const* GetVertices() const { return &m_vertices[0]; }

	// Количество треугольников
	size_t GetTriangleCount() const { return m_triangles.size(); }
	// Адрес массива треугольников
	CTriangle const* GetTriangles() const { return &m_triangles[0]; }

private:
	std::vector<Vertex> m_vertices; // Вершины
	std::vector<CTriangle> m_triangles; // Треугольные грани
};

/*
Класс "Сетка из треугольников"
*/
class CTriangleMesh : public CGeometryObjectImpl
{
public:
	CTriangleMesh(std::shared_ptr<CTriangleMeshData const> pMeshData, CMatrix4d const& transform = CMatrix4d());

	// Поиск пересечения луча с полигональной сеткой
	virtual bool Hit(CRay const& ray, CIntersection& intersection) const;

private:
	// Адрес данных полигональной сетки
	std::shared_ptr<CTriangleMeshData const> m_pMeshData;
};
