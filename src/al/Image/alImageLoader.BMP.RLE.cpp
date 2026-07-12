#include "al.h"

/*
The Irrlicht Engine License
  ===========================

  Copyright (C) 2002-2015 Nikolaus Gebhardt

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
	 claim that you wrote the original software. If you use this software
	 in a product, an acknowledgement in the product documentation would be
	 appreciated but is not required.
  2. Altered source versions must be clearly marked as such, and must not be
	 misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

*/

//	irrlicht
void alImageLoader_BMP_decompress4BitRLE(uint8_t*& rleData, uint8_t*& inds, uint32_t size, uint32_t width, uint32_t height) 
{
	uint32_t lineWidth = (width + 1u) / 2u/*+pitch*/;
	uint8_t* p = rleData;
	uint8_t* newBmp = inds;
	uint8_t* d = newBmp;
	uint8_t* destEnd = newBmp + lineWidth * height;
	int32_t line = 0;
	int32_t shift = 4;

	while (rleData - p < (int32_t)size && d < destEnd) {
		if (*p == 0) {
			++p;

			switch (*p) {
			case 0: // end of line
				++p;
				++line;
				d = newBmp + (line * lineWidth);
				shift = 4;
				break;
			case 1: // end of bmp
				return;
			case 2:
			{
				++p;
				int32_t x = (uint8_t)*p; ++p;
				int32_t y = (uint8_t)*p; ++p;
				d += x / 2 + y * lineWidth;
				shift = x % 2 == 0 ? 4 : 0;
			}
			break;
			default:
			{
				// absolute mode
				int32_t count = (uint8_t)*p; ++p;
				int32_t readAdditional = ((2 - ((count) % 2)) % 2);
				int32_t readShift = 4;
				int32_t i;

				for (i = 0; i < count; ++i)
				{
					int32_t color = (((uint8_t)*p) >> readShift) & 0x0f;
					readShift -= 4;
					if (readShift < 0)
					{
						++* p;
						readShift = 4;
					}

					uint8_t mask = static_cast<uint8_t>(0x0f << shift);
					*d = static_cast<uint8_t>((*d & (~mask)) | ((color << shift) & mask));

					shift -= 4;
					if (shift < 0)
					{
						shift = 4;
						++d;
					}

				}

				for (i = 0; i < readAdditional; ++i)
					++p;
			}
			}
		}
		else
		{
			int32_t count = (uint8_t)*p; ++p;
			int32_t color1 = (uint8_t)*p; color1 = color1 & 0x0f;
			int32_t color2 = (uint8_t)*p; color2 = (color2 >> 4) & 0x0f;
			++p;

			for (int32_t i = 0; i < count; ++i)
			{
				uint8_t mask = static_cast<uint8_t>(0x0f << shift);
				uint8_t toSet = static_cast<uint8_t>((shift == 0 ? color1 : color2) << shift);
				*d = static_cast<uint8_t>((*d & (~mask)) | (toSet & mask));

				shift -= 4;
				if (shift < 0)
				{
					shift = 4;
					++d;
				}
			}
		}
	}
}

void alImageLoader_BMP_decompress8BitRLE(uint8_t*& rleData, uint8_t*& inds, uint32_t size, uint32_t width, uint32_t height) 
{
	uint8_t* p = rleData;
	uint8_t* newBmp = inds;
	uint8_t* d = newBmp;
	uint8_t* destEnd = newBmp + (width * height);
	int32_t line = 0;

	while (rleData - p < (int32_t)size && d < destEnd)
	{
		if (*p == 0)
		{
			++p;

			switch (*p)
			{
			case 0: // end of line
				++p;
				++line;
				d = newBmp + (line * (width));
				break;
			case 1: // end of bmp
				return;
			case 2:
				++p; d += (uint8_t)*p;  // delta
				++p; d += ((uint8_t)*p) * (width);
				++p;
				break;
			default:
			{
				// absolute mode
				int32_t count = (uint8_t)*p; ++p;
				int32_t readAdditional = ((2 - (count % 2)) % 2);
				int32_t i;

				for (i = 0; i < count; ++i)
				{
					*d = *p;
					++p;
					++d;
				}

				for (i = 0; i < readAdditional; ++i)
					++p;
			}
			}
		}
		else
		{
			int32_t count = (uint8_t)*p; ++p;
			uint8_t color = *p; ++p;
			for (int32_t i = 0; i < count; ++i)
			{
				*d = color;
				++d;
			}
		}
	}
}
