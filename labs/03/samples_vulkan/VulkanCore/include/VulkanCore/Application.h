#pragma once

#include <VulkanCore/Frame.h>
#include <VulkanCore/RenderDevice.h>
#include <VulkanCore/ShaderCache.h>

#include <memory>
#include <string>

namespace vkc
{

/// Оконный цикл. Пример наследуется от Application, переопределяет нужные ему
/// хуки, а до устройства и шейдеров добирается через методы базы.
class Application
{
public:
	struct Config
	{
		std::string title;
		glm::ivec2 size{ 800, 600 };
		bool gui = true;
		uint32_t framesInFlight = 2;
		std::string fontPath = "fonts/NotoSans-Regular.ttf";
	};

	explicit Application(Config config);
	virtual ~Application();

	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	/// Крутит цикл до закрытия окна, затем дожидается простоя устройства.
	void Run();

protected:
	// Все реализации по умолчанию пустые; вызывать версию базового класса
	// не нужно ни в одном из хуков.

	/// Симуляция, матрицы, реакция на ввод. Устройство здесь не трогаем.
	virtual void OnUpdate(float dt) { (void)dt; }

	/// Загрузка данных текущего кадра на устройство.
	virtual void OnPrepare(const Frame& frame) { (void)frame; }

	/// Виджеты интерфейса. Кадр ImGui открывает и закрывает база.
	virtual void OnGui() {}

	/// Проходы рендеринга и команды рисования.
	virtual void OnRender(const Frame& frame) = 0;

	/// Вызывается после того, как база пересоздала цепочку показа.
	virtual void OnResize(vk::Extent2D extent) { (void)extent; }

	virtual void OnShutdown() {}

	// Обработчики ввода не вызываются, когда события забрал интерфейс.
	virtual void OnKey(int key, int scancode, int action, int mods);
	virtual void OnMouseButton(glm::vec2 pos, int button, int action, int mods);
	virtual void OnMouseMove(glm::vec2 pos, glm::vec2 delta);
	virtual void OnScroll(glm::vec2 offset);

	const RenderDevice& Device() const;
	ShaderCache& Shaders();
	TargetFormats Formats() const;
	vk::Extent2D Extent() const;

	/// Сколько кадров процессор записывает вперёд.
	uint32_t FramesInFlight() const;

private:
	struct Impl;
	std::unique_ptr<Impl> m_impl;
};

} // namespace vkc
