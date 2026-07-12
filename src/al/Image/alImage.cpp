#include "al.h"

#include "Image/alImage.h"
#include "Classes/alColor.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

alImage::alImage() 
{
}

alImage::~alImage()
{
	Delete();
}

void alImage::Delete()
{
	if (m_data)
		alMemory::Free(m_data);
}

void alImage::Create(uint32_t x, uint32_t y)
{
	Delete();
	m_format = alImageFormat::R8G8B8A8;
	m_width = x;
	m_height = y;
	m_pitch = m_width * 4;
	m_dataSize = m_pitch * m_height;
	m_data = (uint8_t*)alMemory::Malloc(m_dataSize);
}

void alImage::_convertToR8G8B8A8_from_X8R8G8B8()
{
	for (uint32_t i = 0; i < m_dataSize; )
	{
		int32_t a = abs((int32_t)m_data[i] - 255);
		int32_t r = abs((int32_t)m_data[i + 1] - 255);
		int32_t g = abs((int32_t)m_data[i + 2] - 255);
		int32_t b = abs((int32_t)m_data[i + 3] - 255);
		a = 255;
		m_data[i] = (uint8_t)r;
		m_data[i + 1] = (uint8_t)g;
		m_data[i + 2] = (uint8_t)b;
		m_data[i + 3] = (uint8_t)a;
		i += 4;
	}
	m_format = alImageFormat::R8G8B8A8;
}

void alImage::_convertToR8G8B8A8_from_R8G8B8()
{
	uint32_t new_pitch = m_width * 4;
	uint32_t new_dataSize = m_height * new_pitch;
	uint8_t* new_data = (uint8_t*)alMemory::Malloc(new_dataSize);

	rgb* rgb_data = (rgb*)m_data;
	rgba* rgba_data = (rgba*)new_data;
	int32_t num_of_pixels = m_width * m_height;
	for (int32_t i = 0; i < num_of_pixels; ++i)
	{
		rgba_data->r = rgb_data->r;
		rgba_data->g = rgb_data->g;
		rgba_data->b = rgb_data->b;
		rgba_data->a = 255;

		rgba_data++;
		rgb_data++;
	}

	alDestroy(m_data);

	m_data = new_data;
	m_pitch = new_pitch;
	m_dataSize = new_dataSize;
	m_format = alImageFormat::R8G8B8A8;
}

void alImage::convertToR8G8B8A8()
{
	AL_ASSERT_ST(m_data);
	switch (m_format)
	{
	case alImageFormat::R8G8B8:
		_convertToR8G8B8A8_from_R8G8B8();
		break;
		/*case alImageFormat::X8R8G8B8:
			_convertToR8G8B8A8_from_X8R8G8B8();
			break;*/
	case alImageFormat::R8G8B8A8:
		break;
		/*case alImageFormat::A8R8G8B8:
			break;
		case alImageFormat::BC1:
			break;
		case alImageFormat::BC2:
			break;
		case alImageFormat::BC3:
			break;
		case alImageFormat::Unknown:
			break;*/
	default:
		break;
	}
}

void alImage::FlipVertical()
{
	AL_ASSERT_ST(m_data);
	uint8_t* line = nullptr;
	line = (uint8_t*)alMemory::Malloc(m_pitch);
	uint8_t* p_Up = &m_data[0];
	uint8_t* p_Down = &m_data[m_pitch * m_height - m_pitch];
	for (uint32_t i = 0; i < m_height / 2; ++i)
	{
		memcpy(line, p_Up, m_pitch);
		memcpy(p_Up, p_Down, m_pitch);
		memcpy(p_Down, line, m_pitch);
		p_Up += m_pitch;
		p_Down -= m_pitch;
	}
	alMemory::Free(line);
}

void alImage::FlipPixel()
{
	for (uint32_t i = 0; i < m_dataSize; )
	{
		uint8_t r = m_data[i];
		uint8_t g = m_data[i + 1];
		uint8_t b = m_data[i + 2];
		uint8_t a = m_data[i + 3];
		m_data[i] = b;
		m_data[i + 1] = g;
		m_data[i + 2] = r;
		m_data[i + 3] = a;
		i += 4;
	}
}

void alImage::Fill(const alColor& color)
{
	uint8_t* data = m_data;
	for (uint32_t h = 0; h < m_height; ++h)
	{
		for (uint32_t w = 0; w < m_width; ++w)
		{
			*data = color.GetAsByteRed();   ++data;
			*data = color.GetAsByteGreen(); ++data;
			*data = color.GetAsByteBlue();  ++data;
			*data = color.GetAsByteAlpha(); ++data;
		}
	}
}

void alImage::Fill(
	alImage* img, 
	const alVec2u& where,
	alVec2i* outPosition,
	alVec4f* outUV)
{
	AL_ASSERT_ST(img);
	AL_ASSERT_ST(this->m_data);
	AL_ASSERT_ST(this->m_format == img->m_format);

	uint32_t srcW = img->m_width;
	uint32_t srcH = img->m_height;

	if (outPosition)
	{
		outPosition->x = where.x;
		outPosition->y = where.y;
	}
	if (outUV)
	{
		alVec2f uvlt;
		alVec2f uvrb;
		GetUV(where.x, where.y, &uvlt);
		GetUV(where.x + srcW, where.y + srcH, &uvrb);
		outUV->x = uvlt.x;
		outUV->y = uvlt.y;
		outUV->z = uvrb.x;
		outUV->w = uvrb.y;
	}

	if (m_format == alImageFormat::R8G8B8A8
		&& img->m_format == alImageFormat::R8G8B8A8)
	{
		rgba* rgbadataSrc = (rgba*)img->m_data;
		rgba* rgbadataDst = (rgba*)m_data;

	
		
		for (uint32_t yi = 0; yi < srcH; ++yi)
		{
			uint32_t whereY = where.y + yi;
			if (whereY >= this->m_height)
				continue;

			for (uint32_t xi = 0; xi < srcW; ++xi)
			{
				uint32_t whereX = where.x + xi;


				if (whereX >= this->m_width)
				{
					//break;
					continue;
				}


				uint32_t srcIndex = xi + (yi * srcW);
				uint32_t dstIndex = whereX + (whereY * this->m_width);

				if (rgbadataSrc[srcIndex].a == 0)
					continue;

				rgbadataDst[dstIndex] = rgbadataSrc[srcIndex];
			}
			if (whereY >= this->m_height)
			{
				continue;
			//	break;
			}
		}
	}
}

void alImage::Fill(
	const uint8_t* indices, 
	alColor* palette, 
	const alVec2u& size, 
	const alVec2u& where,
	alVec2i* outPosition, 
	alVec4f* outUV)
{
	uint32_t srcW = size.x;
	uint32_t srcH = size.y;

	if (outPosition)
	{
		outPosition->x = where.x;
		outPosition->y = where.y;
	}
	if (outUV)
	{
		alVec2f uvlt;
		alVec2f uvrb;
		GetUV(where.x, where.y, &uvlt);
		GetUV(where.x + srcW, where.y + srcH, &uvrb);
		outUV->x = uvlt.x;
		outUV->y = uvlt.y;
		outUV->z = uvrb.x;
		outUV->w = uvrb.y;
	}

	if (m_format == alImageFormat::R8G8B8A8)
	{
		rgba* rgbadataDst = (rgba*)m_data;		

		for (uint32_t yi = 0; yi < srcH; ++yi)
		{
			uint32_t whereY = where.y + yi;
			if (whereY >= this->m_height)
				continue;

			for (uint32_t xi = 0; xi < srcW; ++xi)
			{
				uint32_t whereX = where.x + xi;


				if (whereX >= this->m_width)
				{
					//break;
					continue;
				}

				uint32_t srcIndex = xi + (yi * srcW);
				uint32_t dstIndex = whereX + (whereY * this->m_width);

				alColor clr = palette[indices[srcIndex]];

				rgbadataDst[dstIndex].r = clr.GetAsByteRed();
				rgbadataDst[dstIndex].g = clr.GetAsByteGreen();
				rgbadataDst[dstIndex].b = clr.GetAsByteBlue();
				rgbadataDst[dstIndex].a = clr.GetAsByteAlpha();
			}
			if (whereY >= this->m_height)
			{
				continue;
				//	break;
			}
		}
	}
}

void alImage::Fill(const uint8_t* indices, alColor* palette, const alVec2u& size, alVec2u* wherePtr, alVec2i* outPosition, alVec4f* outUV)
{
	alVec2u where = *wherePtr;
	Fill(indices, palette, size, where, outPosition, outUV);
	wherePtr->x += size.x;
}
