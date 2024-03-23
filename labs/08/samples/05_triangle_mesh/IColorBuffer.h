#pragma once

/*
Вспомогательный интерфейс "Буфер цвета". Служит для обстрагирования 
класс CRenderContext от деталей реализации буфера кадра, в котором
строится изображение
*/
class IColorBuffer
{
public:
	virtual void SetPixelColor(unsigned x, unsigned y, std::uint32_t color) = 0;
	virtual ~IColorBuffer(void) = default;
};
