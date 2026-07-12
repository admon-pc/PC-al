#include "al.gdi.h"
#include "Image/alImage.h"

alGSGDITexture::alGSGDITexture()
{
}

alGSGDITexture::~alGSGDITexture()
{
	if (m_bitmap)
		delete m_bitmap;
}

bool alGSGDITexture::Init()
{
	auto img = m_textureInfo.m_imagePtr;
	m_bitmap = new Gdiplus::Bitmap((INT)img->m_width, (INT)img->m_height, PixelFormat32bppARGB);
	Gdiplus::Rect r(0, 0, img->m_width, img->m_height);
	Gdiplus::BitmapData bmpData;
	if (m_bitmap->LockBits(&r, Gdiplus::ImageLockModeWrite,
		PixelFormat32bppARGB, &bmpData) == Gdiplus::Ok)
	{
		uint8_t* src = img->m_data;
		uint8_t* dst = (uint8_t*)bmpData.Scan0;
		for (uint32_t i = 0; i < img->m_height; ++i)
		{
			memcpy(dst, src, img->m_pitch);
			dst += img->m_pitch;
			src += img->m_pitch;
		}
		m_bitmap->UnlockBits(&bmpData);
	}

	return true;
}

