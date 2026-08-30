#pragma once

#include <chrono>

namespace vkc
{

/// Ход времени в приложении: длительность кадра и частота кадров.
class FpsCounter
{
public:
	/// Отмечает начало нового кадра.
	void Tick();

	/// Секунд, прошедших с предыдущего кадра.
	float DeltaTime() const noexcept { return m_deltaTime; }

	/// Кадров в секунду, усреднённое за последнюю четверть секунды.
	float Fps() const noexcept { return m_fps; }

private:
	using Clock = std::chrono::steady_clock;

	Clock::time_point m_lastTime = Clock::now();
	float m_deltaTime = 0.0f;
	float m_fps = 0.0f;
	float m_accumulator = 0.0f;
	int m_frames = 0;
};

} // namespace vkc
