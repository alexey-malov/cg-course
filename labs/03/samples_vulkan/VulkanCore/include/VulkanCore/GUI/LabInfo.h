#pragma once

#include <string>

namespace vkc
{

/// Окно с названием работы и кратким описанием управления.
class LabInfo
{
public:
	LabInfo(std::string title, std::string description);

	void Render() const;

private:
	std::string m_title;
	std::string m_description;
};

} // namespace vkc
