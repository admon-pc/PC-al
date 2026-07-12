#ifndef _ALIMG_H_
#define _ALIMG_H_

enum class alImageFormat : uint32_t
{
	R8G8B8,		// uint8_t uint8_t uint8_t 
	R8G8B8A8,		// uint8_t uint8_t uint8_t uint8_t

	// BMP
	X1R5G5B5,
	X4R4G4B4,
	A4R4G4B4,
	R5G6B5,
	A1R5G5B5,
	One_bit,
};

class alImage
{
public:
	alImage();
	~alImage();

	// fast creating
	void Create(uint32_t x, uint32_t y);

	// теперь m_data может хранить файл полностью. В случае DDS для DirectXTK.
	// для доступа к массиву с пикселями нужен m_bitDataOffset
	// если это обычные файлы (типа .PNG) то m_data указывает на обычный буфер а m_bitDataOffset = 0
	uint8_t* m_data = nullptr;
	uint32_t m_bitDataOffset = 0;
	uint32_t m_dataSize = 0;
	uint32_t m_fileSize = 0;
	uint32_t m_width = 0;
	uint32_t m_height = 0;
	uint32_t m_bits = 32;
	uint32_t m_mipCount = 1;
	uint32_t m_pitch = 0;
	alImageFormat m_format = alImageFormat::R8G8B8A8;

	struct rgb
	{
		uint8_t r, g, b;
	};
	struct rgba
	{
		uint8_t r, g, b, a;
	};

	void _convertToR8G8B8A8_from_X8R8G8B8();
	void _convertToR8G8B8A8_from_R8G8B8();
	void convertToR8G8B8A8();
	void FlipVertical();
	void FlipPixel();
	void Fill(const alColor& color);

	// outPosition and outUV is optional
	// it will write position and texture coordinate.
	void Fill(alImage*, const alVec2u& where, alVec2i* outPosition, alVec4f* outUV);

	void Fill(const uint8_t* indices, alColor* palette, const alVec2u& size, const alVec2u& where, alVec2i* outPosition, alVec4f* outUV);
	
	// here `where` is a pointer, and method will move position for next draw, it will add value into `where` (size.x and size.y)
	// so, you do not need to care about changin where everytime.
	void Fill(const uint8_t* indices, alColor* palette, const alVec2u& size, alVec2u* where, alVec2i* outPosition, alVec4f* outUV);

	void Delete();

	void GetUV(int32_t x, int32_t y, alVec2f* uv)
	{
		alVec2f tsz((float32_t)m_width, (float32_t)m_height);
		uv->x = (float32_t)x * 1.f / tsz.x;
		uv->y = (float32_t)y * 1.f / tsz.y;
	}
};

#endif

