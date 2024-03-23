#include "stdafx.h"
#include "ConicCylinder.h"
#include "Ray.h"
#include "Intersection.h"

CConicCylinder::CConicCylinder(
	double const & height,		// Высота цилиндра (>0)
	double const& baseRadius,	// Радиус основания (>0)
	double const& capRadius,	// Радиус крышки (>=0, но не превышает радиуса основания)
	CMatrix4d const& tranform)
:CGeometryObjectWithInitialTransformImpl(tranform)
{
	assert(height >= 0);
	assert(baseRadius >= 0);
	assert(capRadius >= 0);

	/*
	Для упрощения задачи нахождения точек пересечения поиск пересечения будет производиться
	с базовым коническим цилиндром высотой 1, радиусом основания 1 и радиусом крышки m_smallRadius.

	Поэтому вычислим параметр m_smallRadius базового конического цилиндра, а также рассчитаем
	матрицу начального преобразования, переводящую базовый конический цилиндр в требуемый
	*/
	m_smallRadius = (capRadius < baseRadius) ? (capRadius / baseRadius) : (baseRadius / capRadius);

	double maxRadius = (baseRadius > capRadius) ? baseRadius : capRadius;

	/*
	Задаем матрицу преобразования
	*/
	CMatrix4d initialTransform;
	// Задаем параметры масштабирования
	initialTransform.Scale(maxRadius, maxRadius, height);
	if (baseRadius < capRadius)
	{
		// В случае когда радиус крышки больше радиуса основания необходимо предварительно
		// зеркально отобразить базовый конический цилиндр относительно плоскости z=0 и
		// передвинуть на 1 вдоль оси z
		initialTransform.Translate(0, 0, 1);
		initialTransform.Scale(1, 1, -1);
	}
	SetInitialTransform(initialTransform);
}

bool CConicCylinder::Hit(CRay const& ray, CIntersection & intersection)const
{
	// Вычисляем обратно преобразованный луч (вместо вполнения прямого преобразования объекта)
	CRay invRay = Transform(ray, GetInverseTransform());

	/*
	Уравнение боковой стенки базового конического цилиндра имеет вид:
	G(x, y, z) = x^2 + y^2 - (1 + (s - 1)*z)^2 = 0, 
	где:
		0 <= z <= 1,
		r(0) = 1:	радиус основания
		r(1) = s:	радиус крышки (не превышает радиуса основания)

	Подставляя координаты точки луча P = S + c*t  (S - точка испускания луча, c - направление луча)
	в уравнение G, получаем уравнение боковой стенки
	A*t^2 + 2*B*t + C = 0, где:
		A = cx^2 + cy^2 - q^2
		B = Sx*cx + Sy*cy - F*q
		C = Sx^2 + Sy^2 - F^2, где
			q = (s-1) * cz
			F = 1 + (s-1) * Sz
	Если дискриминант B^2 - AC отрицателен, то с боковой стенкой пересечений нет
	Если дискриминант не отрицателен, то нужно проверить, что координата z точки соударения находится в диапазоне от 0 до 1

	Столкновение с основанием требует нахождения точки пересечения с плоскостью z=0.
	Столкновение луча с основанием происходит при 
		x^2 + y^2 <= 1

	Столкновение с крышкой конического цилиндра - 'nj столковение с плоскостью z=1
	Столкновение луча с крышкой происходит при выполнении условия
		x^2 + y^2 < s^2
	*/


	/*
	Идентификаторы поверхностей конического цилиндра
	*/
	enum HitSurface
	{
		HIT_SIDE,
		HIT_BASE,
		HIT_CAP,
	};

	/*
	Краткая информация о пересечении луча с поверхностями конического цилиндра.
	*/
	struct HitPoint
	{
		double hitTime;
		HitSurface hitSurface;
	};

	unsigned numHits = 0;		// Количество точек пересечения
	HitPoint hitPoints[2];		// С выпуклым объектом максимально возможны 2 точки пересечения луча

	/*
	Начало и направление луча
	*/
	CVector3d const& start = invRay.GetStart();
	CVector3d const& dir = invRay.GetDirection();

	/*
	Вспомогательные переменные для упрощения вычислений
	*/
	double q = m_smallRadius - 1;
	double k = q * dir.z;
	double f = 1 + q * start.z;

	/*
	Коэффициенты квадратного уравнения боковой стенки конического цилиндра
	*/
	double a = Sqr(dir.x) + Sqr(dir.y) - Sqr(k);
	double b = (start.x * dir.x) + (start.y * dir.y) - (f * k);
	double c = Sqr(start.x) + Sqr(start.y) - Sqr(f);

	// Дискриминант квадратного уравнения
	double discr = b * b - a * c;

	// Время, которое луч проходит из точки испускания, не испытывая столкновения
	// Нужно для того, чтобы отраженный/преломленный луч мог оторваться от границы объекта после столкновения
	static const double HIT_TIME_EPSILON = 1e-6;

	/*
	Если дискриминант неотрицательный, то есть точки пересечения луча с боковой стенкой
	*/
	if (discr > 0)
	{
		double invA = 1.0 / a;
		double discRoot = sqrt(discr);

		// Первый корень квадратного уравнения - время столкновения с боковой стенкой
		double t = (-b - discRoot) * invA;
		// Нас не интересуют пересечения, происходящие "в прошлом" луча
		if (t > HIT_TIME_EPSILON)				
		{
			// Проверяем координату z точки пересечения. Она не должна выходить за пределы
			// диапазона 0..1
			double hitZ = start.z + dir.z * t;
			if (hitZ >= 0 && hitZ <= 1)
			{
				// Добавляем краткую информацию о точке пересечения
				HitPoint hit = {t, HIT_SIDE};
				hitPoints[numHits++] = hit;
			}
		}

		// Первый корень квадратного уравнения - время столкновения с боковой стенкой
		t = (-b + discRoot) * invA;
		// Выполняем аналогичные проверки
		if (t > HIT_TIME_EPSILON)
		{
			double hitZ = start.z + dir.z * t;
			if (hitZ >= 0 && hitZ <= 1)
			{
				HitPoint hit = {t, HIT_SIDE};
				hitPoints[numHits++] = hit;
			}
		}
	}

	// Длина проекции луча на ось z, меньше которой считается, что пересечений с основанием и крышкой нет
	static const double EPSILON = 1e-8;

	/*
	Если два возможных столкновения луча с объектом уже были найдены, нет смысла искать третье
	*/
	if ((numHits < 2) && 
		(std::abs(dir.z) > EPSILON))
	{
		double invDirZ = 1.0 / dir.z;

		// ищем время точки пересечения луча с основанием
		double t = -start.z * invDirZ;

		/*
		Игнорируем пересечения, произошедшие в отрицательное время, вычисляем точку 
		пересечения с плоскостью z=0 и проверяем, находится ли точка внутри основания
		*/
		if (t >= HIT_TIME_EPSILON)
		{
			double hitX = start.x + dir.x * t;
			double hitY = start.y + dir.y * t;
			if (Sqr(hitX) + Sqr(hitY) < 1)
			{
				HitPoint hit = {t, HIT_BASE};
				hitPoints[numHits++] = hit;
			}
		}

		if (numHits < 2)
		{
			// ищем время точки пересечения луча с крышкой
			t = (1 - start.z) * invDirZ;

			// Игнорируем пересечения, произошедшие в отрицательное время, вычисляем точку 
			// пересечения с плоскостью z=1 и проверяем, находится ли точка внутри основания
			if (t > HIT_TIME_EPSILON)
			{
				double hitX = start.x + dir.x * t;
				double hitY = start.y + dir.y * t;
				if (Sqr(hitX) + Sqr(hitY) < Sqr(m_smallRadius))
				{
					HitPoint hit = {t, HIT_CAP};
					hitPoints[numHits++] = hit;
				}
			}
		}
	}

	// Если ни одного пересечения не было найдено, дальнейшие вычисления проводить бессмысленно
	if (numHits == 0)
	{
		return false;
	}

	/*
	Упорядочиваем события столкновения в порядке возрастания времени столкновения 
	*/
	if (numHits == 2)
	{
		if (hitPoints[0].hitTime > hitPoints[1].hitTime)
		{
			// std::swap не получается использовать из-за особенностей gcc
			// в котором шаблон не имеет доступа к типам, объявленным внутри функции
			HitPoint tmp(hitPoints[0]);
			hitPoints[0] = hitPoints[1];
			hitPoints[1] = tmp;
		}
	}

	// Для всех найденных точек пересечения собираем полную информацию и 
	// добавляем ее в объект intersection
	for (unsigned i = 0; i < numHits; ++i)
	{
		HitPoint const& h = hitPoints[i];

		double const& hitTime = h.hitTime;

		/*
		Вычисляем координаты точки пересечения
		*/
		CVector3d hitPoint = ray.GetPointAtTime(hitTime);
		CVector3d hitPointInObjectSpace = invRay.GetPointAtTime(hitTime);
		CVector3d hitNormalInObjectSpace;

		/*
		В зависимости от типа пересекаемой поверхности выполняем вычисление вектора нормали
		*/
		if (h.hitSurface == HIT_SIDE)
		{
			hitNormalInObjectSpace = CVector3d(hitPointInObjectSpace.x, hitPointInObjectSpace.y, -q * (1 + q * hitPointInObjectSpace.z));
		}
		else if (h.hitSurface == HIT_BASE)
		{
			hitNormalInObjectSpace = CVector3d(0, 0, -1);
		}
		else
		{
			hitNormalInObjectSpace = CVector3d(0, 0, 1);
		}

		/*
		Собираем информацию о точке столкновения
		*/
		CVector3d hitNormal = GetNormalMatrix() * hitNormalInObjectSpace;
		
		CHitInfo hit(
			hitTime, *this,
			hitPoint, hitPointInObjectSpace,
			hitNormal, hitNormalInObjectSpace
			);

		// Добавляем точку столкновения в список
		intersection.AddHit(hit);
	}

	// Возвращаем true, если было найдено хотя бы одно пересечение
	return intersection.GetHitsCount() > 0;
}