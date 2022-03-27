#pragma once

/*
Класс "Координатный фрейм" для визуализации осей координат
*/
class Frame
{
public:
	// Инициализация координатного фрейма
	// size - длина стрелки,
	Frame(float size = 1, float endSize = 0.1);

	// Рисуем координатные оси
	void Draw(void) const;

private:
	float m_size;
	float m_endSize;
};
