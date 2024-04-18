#include "stdafx.h"
#include "TriangleMesh.h"
#include "VectorMath.h"
#include "Intersection.h"
#include "Ray.h"

// Конструирует треугольник, вычисляет ряд вспомогательных параметров
// для ускорения нахождения точки пересечения с лучом
CTriangle::CTriangle(Vertex const& vertex0, Vertex const& vertex1, Vertex const& vertex2, bool flatShaded)
:m_pVertex0(&vertex0)
,m_pVertex1(&vertex1)
,m_pVertex2(&vertex2)
,m_flatShaded(flatShaded)
{
	const double EPSILON = 1e-10;

	// Координаты вершин
	CVector3d const& p0 = vertex0.position;
	CVector3d const& p1 = vertex1.position;
	CVector3d const& p2 = vertex2.position;

	// Ребра
	CVector3d const e01 = p1 - p0;
	CVector3d const e12 = p2 - p1;
	CVector3d const e20 = p0 - p2;

	// Нормаль к плоскости треугольника и уравнение его плоскости
	CVector3d const normal = Cross(e20, e01);
	m_planeEquation = CVector4d(normal, -Dot(normal, p0));

	// Вычисляем единичные нормали к ребрам, лежащие в плоскости треугольника,
	// с использованием векторного произведения
	CVector3d edge01Normal = Normalize(Cross(normal, e01));
	CVector3d edge12Normal = Normalize(Cross(normal, e12));
	CVector3d edge20Normal = Normalize(Cross(normal, e20));

	// Квадраты длин ребер
	double edge01Square = Dot(e01, e01);
	double edge12Square = Dot(e12, e12);
	double edge20Square = Dot(e20, e20);

	// Квадраты длин высот, опущенных на соответствующие ребра трегольника
	double edge01PerpSquare = 
		(edge01Square > EPSILON) 
			? Max(0.0, (edge01Square * edge20Square - Sqr(Dot(e01, e20))) / edge01Square)
			: 0.0;
	double edge12PerpSquare = 
		(edge12Square > EPSILON) 
			? Max(0.0, (edge12Square * edge01Square - Sqr(Dot(e12, e01))) / edge12Square)
			: 0.0;
	double edge20PerpSquare = 
		(edge20Square > EPSILON) 
			? Max(0.0, (edge20Square * edge12Square - Sqr(Dot(e20, e12))) / edge20Square)
			: 0.0;

	// Векторы перпендикуляров (высот), направленных от ребра к противолежащей вершине треугольника
	m_edge01Perp = edge01Normal * sqrt(edge01PerpSquare);
	m_edge12Perp = edge12Normal * sqrt(edge12PerpSquare);
	m_edge20Perp = edge20Normal * sqrt(edge20PerpSquare);

	// Обратные квадраты длин перпендикуляров
	m_invEdge01PerpSquare = (edge01PerpSquare > EPSILON) ? (1.0 / edge01PerpSquare) : 0.0;
	m_invEdge12PerpSquare = (edge12PerpSquare > EPSILON) ? (1.0 / edge12PerpSquare) : 0.0;
	m_invEdge20PerpSquare = (edge20PerpSquare > EPSILON) ? (1.0 / edge20PerpSquare) : 0.0;
}

bool CTriangle::HitTest(CVector3d const& rayStart, CVector3d const& rayDirection, double & hitTime, CVector3d & hitPoint, double & vertex0Weight, double & vertex1Weight, double & vertex2Weight, double const& EPSILON)const
{
	//////////////////////////////////////////////////////////////////////////
	// Проверка на пересечение луча с плоскостью треугольника
	//////////////////////////////////////////////////////////////////////////
	{
		// Скалярное произведение вектора нормали к треугольнику и направления луча
		double normalDotDirection = 
			m_planeEquation.x * rayDirection.x +
			m_planeEquation.y * rayDirection.y +
			m_planeEquation.z * rayDirection.z;

		// Проверка на параллельность луча и плоскости треугольника
		if (abs(normalDotDirection) < EPSILON)
		{
			return false;
		}

		// Подставляем начальную точку в уравнение луча
		double playEquationAtRayStart = 
			m_planeEquation.x * rayStart.x + 
			m_planeEquation.y * rayStart.y +
			m_planeEquation.z * rayStart.z +
			m_planeEquation.w;

		// Вычисляем время соударения луча с плоскостью треугольника
		hitTime = -playEquationAtRayStart / normalDotDirection;

		// Если время соударения в прошлом, то столкновения нет
		if (hitTime < EPSILON)	
		{
			return false;
		}

		// Точка пересечения луча с плоскостью треугольника
		hitPoint = rayStart + hitTime * rayDirection;
	}

	//////////////////////////////////////////////////////////////////////////
	// Проверка принадлежности точки пересечения внутренней области треугольника
	//////////////////////////////////////////////////////////////////////////
	{
		// Вектор из 0 вершины в точку соударения
		CVector3d p0Hit = hitPoint - m_pVertex0->position;

		// Проверяем, что точка соударения лежит по ту же сторону от ребра 01, что и перпендикуляр к ребру
		double p0HitDotEdge01Perp = Dot(p0Hit, m_edge01Perp);
		if (p0HitDotEdge01Perp < 0)
		{
			return false;
		}

		// Проверяем, что точка соударения лежит по ту же сторону от ребра 20, что и перпендикуляр к ребру
		double p0HitDotEdge20Perp = Dot(p0Hit, m_edge20Perp);
		if (p0HitDotEdge20Perp < 0)
		{
			return false;
		}

		// Вектор из 1 вершины в точку соударения
		CVector3d p1Hit = hitPoint - m_pVertex1->position;

		// Проверяем, что точка соударения лежит по ту же сторону от ребра 12, что и перпендикуляр к ребру
		double p1HitDotEdge12Perp = Dot(p1Hit, m_edge12Perp);
		if (p1HitDotEdge12Perp < 0)
		{
			return false;
		}

		// Весовые коэффициенты для интерполяции атрибутов вершин
		vertex0Weight = p1HitDotEdge12Perp * m_invEdge12PerpSquare;
		vertex1Weight = p0HitDotEdge20Perp * m_invEdge20PerpSquare;
		vertex2Weight = 1 - vertex0Weight - vertex1Weight;
	}

	// Точка лежит внутри треугольника, если все 3 весовых коэффициента
	// находятся в диапазоне [0; 1], а их сумма равна 1.
	// Для выполнения этого условия осталось проверить весовой коэффициент
	// при вершине №2 на неотрицательность
	return vertex2Weight >= 0;
}

/*
Конструируем данныен полигональной сетки на основе переданной информации о ее вершинах и гранях
*/
CTriangleMeshData::CTriangleMeshData(std::vector<Vertex> const& vertices, std::vector<Face> const& faces, bool normalize)
:m_vertices(vertices)
{
	size_t const numVertices = m_vertices.size();
	if (normalize)
	{
		// При необходимости выполняем нормализацию нормалей в вершинах
		for (size_t i = 0; i < numVertices; ++i)
		{
			m_vertices[i].normal.Normalize();
		}
	}

	// Выделяем память под хранение всех треугольных граней
	m_triangles.reserve(faces.size());

	// Заполняем массив треугольных граней
	size_t const numFaces = faces.size();
	for (size_t i = 0; i < numFaces; ++i)
	{
		Face const& face = faces[i];

		size_t i0 = size_t(face.vertex0);
		size_t i1 = size_t(face.vertex1);
		size_t i2 = size_t(face.vertex2);
		assert(i0 <numVertices && i1 < numVertices && i2 < numVertices);

		CTriangle triangle(m_vertices[i0], m_vertices[i1], m_vertices[i2], face.isFlat);
		m_triangles.push_back(triangle);
	}
}


CTriangleMesh::CTriangleMesh(CTriangleMeshData const * pMeshData, CMatrix4d const& transform)
:CGeometryObjectImpl(transform)
,m_pMeshData(pMeshData)
{
}

bool CTriangleMesh::Hit(CRay const& ray, CIntersection & intersection)const
{
	// Вычисляем обратно преобразованный луч (вместо вполнения прямого преобразования объекта)
	CRay invRay = Transform(ray, GetInverseTransform());
	CVector3d const& invRayStart = invRay.GetStart();
	CVector3d const& invRayDirection = invRay.GetDirection();

	//////////////////////////////////////////////////////////////////////////
	// Здесь следует выполнить проверку на пересечение луча с ограничивающим
	// объемом (bounding volume) полигональной сетки.
	// При отсутствии такого пересечения луч гарантированно не будет пересекать
	// ни одну из граней сетки, что позволит избежать лишних проверок:
	// if (!ray_intesects_bounding_volume)
	//     return false;
	//////////////////////////////////////////////////////////////////////////
	
	// Получаем информацию о массиве треугольников сетки
	CTriangle const* const triangles = m_pMeshData->GetTriangles();
	const size_t numTriangles = m_pMeshData->GetTriangleCount();

	// Информация о пересечении луча с гранью сетки
	struct FaceHit
	{
		CVector3d hitPointInObjectSpace;// Точка пересечения
		double w0, w1, w2;				// Весовые коэффициенты вершин в точке пересечения
		double hitTime;					// Время пересечения
		size_t faceIndex;				// Индекс грани
	};

	// Массив найденных пересечений луча с гранями сетки.
	std::vector<FaceHit> faceHits;

	//////////////////////////////////////////////////////////////////////////
	// Используется поиск пересечения луча со всеми гранями сетки.
	// Данный подход является очень неэффективным уже на полигональных сетках,
	// содержащих более нескольких десятков граней, а, тем более, сотни и тысячи граней.
	//
	// Для эффективного поиска столкновений следует представить полигональную сетку
	// не в виде массива граней, а в виде древовидной структуры (Oct-Tree или BSP-Tree),
	// что уменьшит вычислительную сложность поиска столкновений с O(N) до O(log N)
	//////////////////////////////////////////////////////////////////////////
	FaceHit hit;
	for (size_t i = 0; i < numTriangles; ++i)
	{
		CTriangle const& triangle = triangles[i];

		// Проверка на пересечение луча с треугольной гранью
		if (triangle.HitTest(invRayStart, invRayDirection, hit.hitTime, hit.hitPointInObjectSpace, hit.w0, hit.w1, hit.w2))
		{
			// Сохраняем индекс грани и добавляем информацию в массив найденных пересечений
			hit.faceIndex = i;

			if (faceHits.empty())
			{
				// При обнаружени первого пересечения резервируем
				// память сразу под 8 пересечений (для уменьшения количества операций выделения памяти)
				faceHits.reserve(8);
			}
			faceHits.push_back(hit);
		}
	}

	// При отсутствии пересечений выходим
	if (faceHits.empty())
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// Упорядочиваем найденные пересечения по возрастанию времени столкновения
	//////////////////////////////////////////////////////////////////////////
	size_t const numHits = faceHits.size();
	std::vector<FaceHit const *> hitPointers(numHits);
	{
		// Инициализируем массив указателей на точки пересечения
		for (size_t i = 0; i < numHits; ++i)
		{
			hitPointers[i] = &faceHits[i];
		}

		// Сортируем массив указателей по возрастанию времени столкновения
		// Сортируются указатели, а не сами объекты, чтобы сократить
		// издержки на обмен элементов массива:
		// На 32 битной платформе размер структуры FaceHit равен 64 байтам,
		// а размер указателя - всего 4 байта.
		if (numHits > 1)
		{
			struct HitPointerComparator
			{
				bool operator()(FaceHit const * p0, FaceHit const * p1)const
				{
					return (p0->hitTime < p1->hitTime);
				}
			};
			std::sort(hitPointers.begin(), hitPointers.end(), HitPointerComparator());
			// Теперь указатели в массиве hitPointers отсортированы
			// в порядке возрастания времени столкновения луча с гранями
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Возвращаем информацию о найденных пересечениях
	//////////////////////////////////////////////////////////////////////////
	for (size_t i = 0; i < numHits; ++i)
	{
		// Получаем информацию о столкновении
		FaceHit const& faceHit = *hitPointers[i];

		// Точка столкновения в мировой системе координат
		CVector3d hitPoint = ray.GetPointAtTime(faceHit.hitTime);

		// Грань, с которой произошло столкновение
		CTriangle const& triangle = triangles[faceHit.faceIndex];

		// Нормаль "плоской грани" во всех точках столкновения равна нормали самой грани
		CVector3d normalInObjectSpace = triangle.GetPlaneEquation();

		if (!triangle.IsFlatShaded())
		{
			// Для неплоских граней выполняется интерполяция нормалей вершин треугольника
			// с учетом их весовых коэффициентов в точке пересечения
			Vertex const& v0 = triangle.GetVertex0();
			Vertex const& v1 = triangle.GetVertex1();
			Vertex const& v2 = triangle.GetVertex2();

			// Взвешенный вектор нормали
			normalInObjectSpace = 
				faceHit.w0 * v0.normal + 
				faceHit.w1 * v1.normal +
				faceHit.w2 * v2.normal;
		}

		// Нормаль в мировой системе координат
		CVector3d normal = GetNormalMatrix() * normalInObjectSpace;

		// Добавляем информацию о точке пересечения в объект intersection
		intersection.AddHit(
			CHitInfo(
				faceHit.hitTime, *this, 
				hitPoint,
				faceHit.hitPointInObjectSpace,
				normal, normalInObjectSpace
				)
			);
	}

	return true;
}
