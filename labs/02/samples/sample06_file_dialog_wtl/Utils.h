#pragma once

class CUtils
{
public:
	static std::wstring GetFileExtension(std::wstring const& filePath);
	static std::wstring GetFileName(std::wstring const& filePath);
	static std::wstring WStringToLower(std::wstring const & str);
	static CLSID GetEncoderCLSID(std::wstring const& extension);
	static bool SaveImage(std::wstring const & path, Gdiplus::Image & image, int quality = 75);
};
