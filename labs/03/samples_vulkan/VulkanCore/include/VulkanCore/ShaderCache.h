#pragma once

#include <vulkan/vulkan.hpp>

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>

namespace vkc
{

class RenderDevice;

/// Ошибка компиляции GLSL. Log() содержит вывод компилятора как есть,
/// вместе с номерами строк, и читать его стоит целиком.
class ShaderCompileError : public std::runtime_error
{
public:
	ShaderCompileError(std::filesystem::path file, std::string log);

	const std::filesystem::path& File() const noexcept { return m_file; }
	const std::string& Log() const noexcept { return m_log; }

private:
	std::filesystem::path m_file;
	std::string m_log;
};

/// Компилирует GLSL в SPIR-V и запоминает результат.
/// Стадия определяется по расширению: .vert, .frag, .comp.
class ShaderCache
{
public:
	explicit ShaderCache(const RenderDevice& device);
	~ShaderCache();

	ShaderCache(const ShaderCache&) = delete;
	ShaderCache& operator=(const ShaderCache&) = delete;

	/// Повторный запрос того же пути отдаёт уже скомпилированный модуль.
	/// Бросает ShaderCompileError, если файла нет или он не компилируется.
	vk::ShaderModule Get(const std::filesystem::path& path);

private:
	struct Impl; // прячет заголовки shaderc от того, кто подключает этот файл
	std::unique_ptr<Impl> m_impl;
};

} // namespace vkc
