#include "MainApp.h"
#include <sstream>
#include <iostream>

const double CMainApp::DEFAULT_SCALE = 100; // 40 пикселей на астрономическую единицу
const double CMainApp::MIN_SCALE = 5;
const double CMainApp::MAX_SCALE = 500;
const double CMainApp::SCALE_FACTOR = 1.02;

const double CMainApp::DEFAULT_TIME_SPEED = 0.1;
const double CMainApp::MIN_TIME_SPEED = -20;
const double CMainApp::MAX_TIME_SPEED = +20;
const double CMainApp::TIME_ADJUSTMENT = 0.02;

double DegreesToRadians(double degrees, double seconds = 0)
{
	return (degrees + seconds / 60) * std::numbers::pi / 180;
}

const CMainApp::PlanetInfo CMainApp::m_planets[] = {
	{ CEllipticOrbitd(0.387, 0.206, 1 / 0.241, DegreesToRadians(7, 0)), 0x5B71FF, 1 },
	{ CEllipticOrbitd(0.723, 0.007, 1 / 0.635, DegreesToRadians(3, 24)), 0xFFDB59, 3 },
	{ CEllipticOrbitd(1.000, 0.017, 1 / 1.000, DegreesToRadians(0)), 0x168EFF, 3 },
	{ CEllipticOrbitd(1.524, 0.093, 1 / 1.881, DegreesToRadians(1, 1)), 0xFF7A68, 2 },
	{ CEllipticOrbitd(5.203, 0.048, 1 / 11.862, DegreesToRadians(1, 18)), 0x897AFF, 6 },
	{ CEllipticOrbitd(6.539, 0.056, 1 / 20.658, DegreesToRadians(2, 29)), 0x47FF81, 5 },
	{ CEllipticOrbitd(19.190, 0.048, 1 / 84.800, DegreesToRadians(0, 45)), 0xFFF463, 5 },
	{ CEllipticOrbitd(30.081, 0.009, 1 / 154.232, DegreesToRadians(1, 47)), 0x70FFFF, 5 },
	{ CEllipticOrbitd(38.525, 0.249, 1 / 247.305, DegreesToRadians(17, 9)), 0x9C63FF, 1 },

	{ CEllipticOrbitd(17.800, 0.967, 1 / 75.300, DegreesToRadians(162, 3)), 0xFFFFFF, 0.5 },
};

CMainApp::CMainApp(const char* title, int width, int height)
	: CGLApplication(title, width, height)
	, m_time(0)
	, m_scale(DEFAULT_SCALE)
	, m_timeSpeed(DEFAULT_TIME_SPEED)
	, m_currentYear(-1)
{
}

CMainApp::~CMainApp(void)
{
}

void CMainApp::OnInit()
{
	m_animationController.Reset();

	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
}

void CMainApp::OnIdle()
{
	PostRedisplay();
}

void CMainApp::OnDisplay()
{
	auto const timeDelta = m_animationController.GetTimeDelta();
	m_animationController.Tick();

	m_time += timeDelta * 0.001 * m_timeSpeed;

	int year = static_cast<int>(std::floor(m_time));
	if (m_currentYear != year)
	{
		m_currentYear = year;
		std::cout << "Year: " << year + 2012 << "\n";
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawSun();
	DrawPlanets();
}

void CMainApp::OnSpecialKey(int key, int x, int y)
{
	x;
	y;

	bool updateTime = false;

	if (key == GLUT_KEY_LEFT)
	{
		m_timeSpeed = m_timeSpeed - TIME_ADJUSTMENT;
		if (m_timeSpeed < MIN_TIME_SPEED)
		{
			m_timeSpeed = MIN_TIME_SPEED;
		}
		updateTime = true;
	}
	else if (key == GLUT_KEY_RIGHT)
	{
		m_timeSpeed = m_timeSpeed + TIME_ADJUSTMENT;
		if (m_timeSpeed > MAX_TIME_SPEED)
		{
			m_timeSpeed = MAX_TIME_SPEED;
		}
		updateTime = true;
	}
	else
	{
		return;
	}

	m_timeSpeed = floor(m_timeSpeed * 1000 + 0.5) / 1000;

	if (updateTime)
	{
		std::stringstream strm;
		strm << "Time speed: " << m_timeSpeed << " year/s";
		glutSetWindowTitle(strm.str().c_str());
	}
}

void CMainApp::OnKeyboard(unsigned char key, int x, int y)
{
	x;
	y;

	if (key == '+')
	{
		m_scale *= SCALE_FACTOR;
		if (m_scale > MAX_SCALE)
		{
			m_scale = MAX_SCALE;
		}
	}
	else if (key == '-')
	{
		m_scale /= SCALE_FACTOR;
		if (m_scale < MIN_SCALE)
		{
			m_scale = MIN_SCALE;
		}
	}

	OnReshape(GetWindowWidth(), GetWindowHeight());

	PostRedisplay();
}

void CMainApp::DrawSun()
{
	glPointSize(static_cast<float>(2 * std::sqrt(m_scale)));
	glBegin(GL_POINTS);
	{
		glColor3f(1, 1, 0);
		glVertex2d(0, 0);
	}
	glEnd();
}

void CMainApp::DrawOrbit(CEllipticOrbitd const& orbit)
{
	glDisable(GL_POINT_SMOOTH);
	glPointSize(1);
	glColor3f(0.5, 0.5, 0.5);
	glBegin(GL_POINTS);
	{
		// double period = 2 * M_PI / orbit.MeanMotion();
		double a = orbit.LargeAxis();
		double e = orbit.Eccentricity();
		double step = 2 * std::numbers::pi / (a * m_scale);
		double c = e * a;
		double k = sqrt(1 - e * e);
		double b = k * a;
		double centerX = -c;
		double centerY = 0;

		double t = 0;
		while (t < 2 * std::numbers::pi)
		{

			glVertex2d(centerX + a * cos(t), centerY + b * sin(t));
			t += step;
		}
	}
	glEnd();
}

void CMainApp::DrawPlanets()
{
	for (int i = 0; i < PLANET_COUNT; ++i)
	{

		PlanetInfo const& planet = m_planets[i];
		CEllipticOrbitd const& orbit = planet.orbit;
		DrawOrbit(orbit);

		DWORD color = planet.color;
		glColor3ub(GetBValue(color), GetGValue(color), GetRValue(color));

		glEnable(GL_POINT_SMOOTH);
		glPointSize(static_cast<float>(planet.size * std::sqrt(m_scale) / 4));
		glBegin(GL_POINTS);
		{
			CVector2d pos = orbit.PlanetPosition2D(m_time);
			glVertex2d(pos.x, pos.y);
		}
		glEnd();
	}
}

void CMainApp::OnReshape(int width, int height)
{
	if (!width || !height)
	{
		return;
	}

	// Задаем порт просмотра размером с клиентскую область окна
	glViewport(0, 0, width, height);
	double hw = width * 0.5;
	double hh = height * 0.5;

	// Вычисляем соотношение сторон окна
	double aspect = double(width) / height;

	// Устанавливаем матрицу проецирования
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-hw / m_scale, hw / m_scale, -hh / m_scale, hh / m_scale);

	glMatrixMode(GL_MODELVIEW);
}
