#include "StdAfx.h"
#include "Utils.h"

using namespace std;
using namespace Gdiplus;

std::wstring CUtils::GetFileExtension(std::wstring const& filePath)
{
	std::wstring fileName = GetFileName(filePath);

	size_t stopPos = fileName.find_first_of(L".");

	return 
		(stopPos != std::wstring::npos) ? fileName.substr(stopPos + 1)
		: std::wstring();
}

std::wstring CUtils::GetFileName(std::wstring const& filePath)
{
	size_t slashPos = filePath.find_last_of(L"\\/");
	return
		(slashPos == std::wstring::npos) ? filePath 
		: filePath.substr(slashPos + 1);
}

std::wstring CUtils::WStringToLower(std::wstring const & str)
{
	std::wstring result(str);
	std::transform(result.begin(), result.end(), result.begin(), [](wchar_t ch) { return static_cast<wchar_t>(std::tolower(ch)); });
	return result;
}

CLSID CUtils::GetEncoderCLSID(std::wstring const& extension)
{
	// Приводим разрешение к виду "*.разрешение"
	std::wstring extensionMask = L"*." + WStringToLower(extension) + L";";

	// Запрашиваем у GDI+ количество кодировщиков изображений
	// и размер блока данных для хранения их описания
	UINT numEncoders;
	UINT encodersSize;
	GetImageEncodersSize(&numEncoders, &encodersSize);

	// Выделяем буфер для хранения информации о кодировщиках
	std::vector<BYTE> encodersBuffer(encodersSize);

	// Запрашиваем у GDI+ информацию обо всех кодировщиков
	ImageCodecInfo* pInstalledCodecs = reinterpret_cast<ImageCodecInfo *>(&encodersBuffer[0]);
	GetImageEncoders(numEncoders, encodersSize, pInstalledCodecs);

	// ищем подходящий кодировщик изображений
	for (unsigned i = 0; i < numEncoders; ++i)
	{
		ImageCodecInfo & codec = pInstalledCodecs[i];

		// получаем расширения файлов, поддерживаемых данным кодировщиком
		// в формате: *.jpg;*.jpe;*.jpeg
		std::wstring extensions = WStringToLower(codec.FilenameExtension) + L";";

		// Если в списке расширений содержится маска расширения файла
		// то кодек считается найденным
		if (extensions.find(extensionMask) != wstring::npos)
		{
			return codec.Clsid;
		}
	}

	// не нашли подходящий кодировщик, возвращаем нулевой идентификатор
	return CLSID_NULL;
}

bool CUtils::SaveImage(std::wstring const & path, Image & image, int quality)
{
	CLSID encoderId = GetEncoderCLSID(GetFileExtension(path));

	if (encoderId == CLSID_NULL)
	{
		return false;
	}

	EncoderParameters parameters;
	parameters.Count = 1;

	EncoderParameter & param0 = parameters.Parameter[0];
	param0.Guid = EncoderCompression;
	param0.NumberOfValues = 1;
	param0.Type = EncoderParameterValueTypeLong;
	param0.Value = &quality;

	return image.Save(path.c_str(), &encoderId, &parameters) == Ok;
}
