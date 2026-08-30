#include <VulkanCore/FpsCounter.h>

namespace vkc
{

namespace
{
constexpr float kAveragingPeriod = 0.25f;
}

void FpsCounter::Tick()
{
	const Clock::time_point now = Clock::now();

	m_deltaTime = std::chrono::duration<float>(now - m_lastTime).count();
	m_lastTime = now;

	m_accumulator += m_deltaTime;
	m_frames += 1;

	if (m_accumulator >= kAveragingPeriod)
	{
		m_fps = static_cast<float>(m_frames) / m_accumulator;
		m_accumulator = 0.0f;
		m_frames = 0;
	}
}

} // namespace vkc
