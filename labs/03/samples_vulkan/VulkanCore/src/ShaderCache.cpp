#include <VulkanCore/RenderDevice.h>
#include <VulkanCore/ShaderCache.h>

#include <shaderc/shaderc.hpp>

#include <fstream>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace vkc
{

namespace
{

shaderc_shader_kind KindFromExtension(const std::filesystem::path& path)
{
	const std::string extension = path.extension().string();

	if (extension == ".vert")
		return shaderc_vertex_shader;
	if (extension == ".frag")
		return shaderc_fragment_shader;
	if (extension == ".comp")
		return shaderc_compute_shader;

	throw ShaderCompileError{ path,
		"неизвестное расширение '" + extension
			+ "'; стадия определяется по расширению: .vert, .frag, .comp" };
}

std::string ReadFile(const std::filesystem::path& path)
{
	std::ifstream file{ path };
	if (!file)
		throw ShaderCompileError{ path, "файл не найден" };

	std::ostringstream text;
	text << file.rdbuf();
	return text.str();
}

/// Разрешает #include в шейдерах. Путь считается относительно файла,
/// который включает, — как в C++.
///
/// Нужен, чтобы общий код лежал в одном .glsl файле,
/// а не копировался в вершинный и фрагментный шейдеры по отдельности.
class FileIncluder : public shaderc::CompileOptions::IncluderInterface
{
public:
	shaderc_include_result* GetInclude(const char* requested, shaderc_include_type,
		const char* requesting, size_t) override
	{
		auto entry = std::make_unique<Entry>();

		const std::filesystem::path path = std::filesystem::path{ requesting }.parent_path() / requested;

		entry->name = path.string();

		std::ifstream file{ path };
		if (file)
		{
			std::ostringstream text;
			text << file.rdbuf();
			entry->content = text.str();
		}
		else
		{
			// Пустое имя — принятый в shaderc признак неудачи; текст ошибки
			// компилятор подставит в общий вывод с номером строки.
			entry->name.clear();
			entry->content = "файл не найден: " + path.string();
		}

		entry->result = shaderc_include_result{ .source_name = entry->name.c_str(),
			.source_name_length = entry->name.size(),
			.content = entry->content.c_str(),
			.content_length = entry->content.size() };

		return &m_entries.emplace_back(std::move(entry))->result;
	}

	void ReleaseInclude(shaderc_include_result*) override {}

private:
	struct Entry
	{
		std::string name;
		std::string content;
		shaderc_include_result result{};
	};

	// Записи живут до конца компиляции: shaderc хранит на них сырые указатели.
	std::vector<std::unique_ptr<Entry>> m_entries;
};

} // namespace

ShaderCompileError::ShaderCompileError(std::filesystem::path file, std::string log)
	: std::runtime_error("ошибка компиляции шейдера: " + file.string() + "\n" + log)
	, m_file(std::move(file))
	, m_log(std::move(log))
{
}

struct ShaderCache::Impl
{
	const RenderDevice* device = nullptr;
	shaderc::Compiler compiler;

	std::unordered_map<std::string, vk::raii::ShaderModule> modules;
};

ShaderCache::ShaderCache(const RenderDevice& device)
	: m_impl(std::make_unique<Impl>())
{
	m_impl->device = &device;
}

ShaderCache::~ShaderCache() = default;

vk::ShaderModule ShaderCache::Get(const std::filesystem::path& path)
{
	const std::string key = path.string();

	if (const auto found = m_impl->modules.find(key); found != m_impl->modules.end())
		return *found->second;

	const std::string source = ReadFile(path);

	shaderc::CompileOptions options;
	options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
	options.SetSourceLanguage(shaderc_source_language_glsl);
	options.SetIncluder(std::make_unique<FileIncluder>());
#ifdef NDEBUG
	options.SetOptimizationLevel(shaderc_optimization_level_performance);
#else
	options.SetGenerateDebugInfo();
#endif

	const shaderc::SpvCompilationResult result = m_impl->compiler.CompileGlslToSpv(source, KindFromExtension(path), key.c_str(), options);

	if (result.GetCompilationStatus() != shaderc_compilation_status_success)
		throw ShaderCompileError{ path, result.GetErrorMessage() };

	const std::vector<uint32_t> spirv{ result.cbegin(), result.cend() };

	const vk::ShaderModuleCreateInfo createInfo{ .codeSize = spirv.size() * sizeof(uint32_t),
		.pCode = spirv.data() };

	auto inserted = m_impl->modules.emplace(key, vk::raii::ShaderModule{ m_impl->device->Raii(), createInfo });

	return *inserted.first->second;
}

} // namespace vkc
