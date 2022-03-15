#include "StdAfx.h"
#include "MainView.h"
#include "Flower.h"

BOOL CMainView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

void CMainView::AfterCreate()
{
	SetTimer(ANIMATION_TIMER, 10);
	m_lastTick = GetTickCount();
}

void CMainView::BeforeDestroy()
{
	KillTimer(ANIMATION_TIMER);
}

void CMainView::OnTimer([[maybe_unused]] UINT_PTR nIDEvent)
{
	RedrawWindow();
}

void CMainView::DrawScene()
{
	// стурктура хранит информацию о цветке, его положении,
	// угле и скорости вращения
	struct FlowerInfo
	{
		float x;
		float y;
		float angle;
		float speed;
		CFlower flower;
	};

	// статическая переменная функции будет инициализирована только раз
	static FlowerInfo flowers[] = {
		{ 0, 0, 0, 34, CFlower(0.3f, 0.2f, 9, RGB(255, 0, 0), RGB(255, 255, 0)) },
		{ 0.5f, 0.8f, 0, -57, CFlower(0.1f, 0.08f, 7, RGB(0, 0, 255), RGB(255, 0, 255)) },
		{ -0.7f, 0.3f, 0, 49, CFlower(0.2f, 0.11f, 13, RGB(125, 22, 255), RGB(255, 200, 133)) },
		{ -0.4f, -0.5f, 0, 68, CFlower(0.25f, 0.2f, 11, RGB(13, 239, 0), RGB(255, 200, 133)) },
		{ 0.7f, -0.4f, 0, -42, CFlower(0.3f, 0.11f, 9, RGB(223, 0, 64), RGB(112, 220, 173)) },
	};
	static const size_t NUM_FLOWERS = sizeof(flowers) / sizeof(*flowers);

	DWORD currentTick = GetTickCount();
	float delta = (currentTick - m_lastTick) * 0.001f;
	m_lastTick = currentTick;

	// Очищаем буфер кадра
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();

	// Рисуем и вращаем все цветы в массиве
	for (size_t i = 0; i < NUM_FLOWERS; ++i)
	{
		FlowerInfo& flowerInfo = flowers[i];

		// выполняем изменение угла вращения
		flowerInfo.angle = fmodf(flowerInfo.angle + flowerInfo.speed * delta, 360);
		// сохраняем текущую матрицу трансформации
		glPushMatrix();

		// вращаем и переносим цветок
		glTranslatef(flowerInfo.x, flowerInfo.y, 0);
		glRotatef(flowerInfo.angle, 0, 0, 1);

		// рисуем цветок
		flowerInfo.flower.Draw();

		// восстанавливаем текущую матрицу
		glPopMatrix();
	}
}

void CMainView::OnSize(UINT uMsg, CSize /*size*/)
{
	&uMsg;
	CRect rc;
	GetClientRect(rc);
	int width = rc.Width();
	int height = rc.Height();

	// Настраиваем порт просмотра
	glViewport(0, 0, width, height);

	// Делаем текущей матрицу проецирования и настраиваем ее параметры
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// область проецирования на порт просмотра по вертикали
	// располагается от -1 до +1, а по горизонтали - пропорционально
	// соотношению сторон клиентской области окна
	double aspect = (double)width / height;
	double viewHeight = 2;
	double viewWidth = aspect * viewHeight;

	glOrtho(
		-viewWidth / 2, +viewWidth / 2,
		-viewHeight / 2, +viewHeight / 2,
		-1, 1);

	// Делаем текущей матрицей матрицу моделирования-вида
	glMatrixMode(GL_MODELVIEW);
}
