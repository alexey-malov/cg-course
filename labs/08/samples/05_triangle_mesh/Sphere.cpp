#include "stdafx.h"
#include "Sphere.h"
#include "Ray.h"
#include "Intersection.h"

CSphere::CSphere(double radius, CVector3d const& center, CMatrix4d const& transform)
:CGeometryObjectWithInitialTransformImpl(transform)
{
	// Сфера заданного радиуса и с центром в заданной точке получается
	// путем масштабирования и переносы базовой сферы (сфера радиуса 1 с центром в начале координат)
	CMatrix4d initialTransform;
	initialTransform.Translate(center.x, center.y, center.z);
	initialTransform.Scale(radius, radius, radius);

	// Задаем начальную трансформацию базовой сферы
	SetInitialTransform(initialTransform);
}

bool CSphere::Hit(CRay const& ray, CIntersection & intersection)const
{
	// Вместо преобразования сферы выполняем обратное преобразование луча
	// Результат будет тот же самый, но вычислить его будет проще
	CRay invRay = Transform(ray, GetInverseTransform());

	/*
	Базовая сфера (сфера радиуса 1 с центром в начале координат) имеет уравнение:
	  x^2 + y^2 + z^2 - 1^2 = 0
	Для вычисления точки пересечения луча с данной сферой необходимо подставить
	точку, лежащую на луче (P=S+c*t) в уравнение сферы
	и решить его относительно t:
	  |c|^2*t^2 + 2(S*c)*t + (|S|^2 - 1) = 0
	То же самое, но в классическом виде:
	  At^2 + 2Bt + C = 0, где:
	  A=|c|^2
	  B=S*c
	  C=|S|^2 - 1

	Решение имеет следующие корни
		 -B - sqrt(B^2 - AC)
	t0 = -------------------
				 A

		 -B + sqrt(B^2 - AC)
	t1 = -------------------
				 A
	*/

	/*
	Вычисляем параметры уравнения сферы
	*/
	double a = Dot(invRay.GetDirection(), invRay.GetDirection());
	double b = Dot(invRay.GetStart(), invRay.GetDirection());
	double c = Dot(invRay.GetStart(), invRay.GetStart()) - 1;
	// Вычисляем дискриминант (для случая четного коэффициента при x)
	double disc = b * b - a * c;

	if (disc < 0)
	{
		// нет корней - нет и точек пересечения (луч не проходит сквозь сферу)
		return false;
	}

	// Время, которое луч проходит из точки испускания, не испытывая столкновения
	// Нужно для того, чтобы отраженный/преломленный луч мог оторваться от границы объекта после столкновения
	static constexpr double HIT_TIME_EPSILON = 1e-8;

	/*
	т.к. коэффициент A=|c|^2 положителен и sqrt(B^2 - AC) тоже положителен, то
	t0 <= t1.
	Нам важно упорядочить точки пересечения в порядке возрастания времени столкновения,
	поэтому сначала будет добавлена точке в момент времени t0, а потом - в момент времени t1
	*/
	double invA = 1 / a;
	double discRoot = sqrt(disc);

	auto addHit = [&ray, &invRay, this, &intersection](double t) {
		if (t > HIT_TIME_EPSILON)
		{
			// Вычисляем координаты точки столкновения в мировой системе координат и системе координат объекта
			CVector3d hitPoint0 = ray.GetPointAtTime(t);
			CVector3d hitPoint0InObjectSpace = invRay.GetPointAtTime(t);
			// Координаты нормали к точке единичной сферы с центром в начале координат
			// совпадают с координатами данной точки, поэтому просто используем ссылку
			CVector3d const& hitNormal0InObjectSpace = hitPoint0InObjectSpace;
			// Вычисляем нормаль в точке соударения
			CVector3d hitNormal0 = GetNormalMatrix() * hitNormal0InObjectSpace;

			// Создаем и добавляем новую точку пересечения
			CHitInfo hit0(
				t, *this,
				hitPoint0, hitPoint0InObjectSpace,
				hitNormal0, hitNormal0InObjectSpace);

			intersection.AddHit(hit0);
		}
	};

	addHit((-b - discRoot) * invA);
	addHit((-b + discRoot) * invA);

	// Было ли хотя бы одно столкновение луча со сферой в положительном времени?
	return intersection.GetHitsCount() > 0;
}
