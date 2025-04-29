#include <windows.h>
// windows.h must be included befor gdiplus.h
#include <gdiplus.h>

#include "Finally.h"
#include "TextureLoader.h"

namespace
{
class GDIPlusInitializer
{
public:
	GDIPlusInitializer()
	{
		Gdiplus::GdiplusStartupInput input;
		Gdiplus::GdiplusStartupOutput output;
		if (Gdiplus::GdiplusStartup(&m_token, &input, &output) != Gdiplus::Ok)
		{
			throw std::runtime_error("Failed to initialize GDI+");
		}
	}
	GDIPlusInitializer(const GDIPlusInitializer&) = delete;
	GDIPlusInitializer& operator=(const GDIPlusInitializer&) = delete;

	~GDIPlusInitializer()
	{
		Gdiplus::GdiplusShutdown(m_token);
	}

private:
	ULONG_PTR m_token;
};

void OnceInitGdiPlus()
{
	static GDIPlusInitializer gdiPlusInitializer;
}

struct TextureInfo
{
	Gdiplus::PixelFormat pixelFormat;
	GLint textureFormat;
	GLint colorComponents;
	GLint internalFormat;
};

TextureInfo TextureInfoFromBitmap(Gdiplus::Bitmap& bmp)
{
	if (bmp.GetPixelFormat() & PixelFormatAlpha)
	{
		return { PixelFormat32bppARGB, GL_BGRA_EXT, 4, GL_RGBA };
	}
	else
	{
		return { PixelFormat24bppRGB, GL_BGR_EXT, 3, GL_RGB };
	}
}

} // namespace

Texture TextureLoader::LoadTexture2D(const std::filesystem::path& path, GLenum target) const
{
	OnceInitGdiPlus();

	Gdiplus::Bitmap bmp(path.wstring().c_str());
	if (bmp.GetLastStatus() != Gdiplus::Ok)
	{
		throw std::runtime_error("Error loading texture file " + path.string());
	}

	const Gdiplus::Rect lockRect(0, 0, bmp.GetWidth(), bmp.GetHeight());

	auto textureInfo = TextureInfoFromBitmap(bmp);
	Texture texture;
	texture.Create();
	glBindTexture(target, texture);

	Gdiplus::BitmapData bitmapData{};
	if (Gdiplus::Ok != bmp.LockBits(&lockRect, Gdiplus::ImageLockModeRead, textureInfo.pixelFormat, &bitmapData))
	{
		throw std::runtime_error("Error locking bitmap data");
	}
	auto releaser = util::Finally([&bitmapData, &bmp] {
		bmp.UnlockBits(&bitmapData);
	});

	glTexImage2D(target, 0, textureInfo.internalFormat,
		bmp.GetWidth(), bmp.GetHeight(),
		0, textureInfo.textureFormat, GL_UNSIGNED_BYTE, bitmapData.Scan0);
}
