#include "al.h"

#include <memory>

#include "Image/alImage.h"
#include "alImageLoader.BMP.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;


#pragma pack(push,2)
struct BitmapHeader 
{
	uint16_t		bfType;
	uint32_t		bfSize;
	uint16_t		bfReserved1;
	uint16_t		bfReserved2;
	uint32_t		bfOffBits;
};
#pragma pack(pop)

struct rgbquad 
{
	uint8_t Blue;
	uint8_t Green;
	uint8_t Red;
	uint8_t Reserved;
};

struct ciexyzTriple 
{
	alVec3i		ciexyzRed;
	alVec3i		ciexyzGreen;
	alVec3i		ciexyzBlue;
};

struct BitmapInfoHeader_v5 
{
	uint32_t			bV5Size;		//	размер header в файле
	int32_t			bV5Width;		//	ширина
	int32_t			bV5Height;		//	высота
	uint16_t			bV5Planes;		//	хз что это, всегда 1
	uint16_t			bV5BitCount;	//	биты
	uint32_t			bV5Compression;	//	1 - RLE 8bit, 2 - RLE 4bit, 3 или что-то, видимо, специальные обозначения у разработчиков 2D редакторов.
	uint32_t			bV5SizeImage;	//	размер массива пикселей/индексов
	int32_t			bV5XPelsPerMeter;// размер в чём-то, видимо для печати или вывода ещё кудато
	int32_t			bV5YPelsPerMeter;//	для обычного использования в ПК не играет никакой роли
	uint32_t			bV5ClrUsed;		//	обычно тут ноль
	uint32_t			bV5ClrImportant;//	и тут ноль
	uint32_t			bV5RedMask;		//	для определения позиции цветов
	uint32_t			bV5GreenMask;	//	в форматах типа x1r5g5b5
	uint32_t			bV5BlueMask;
	uint32_t			bV5AlphaMask;
	uint32_t			bV5CSType;		//	далее информация для более специализированного
	ciexyzTriple bV5Endpoints;	//	использования.
	uint32_t			bV5GammaRed;	//	для передачи простой картинки достаточно того
	uint32_t			bV5GammaGreen;	//	что указано выше. А эта часть нужна для, например,
	uint32_t			bV5GammaBlue;	//	тех кто делает видео плеер, видео редактор
	uint32_t			bV5Intent;		//	что-то типа этого. Как бы универсальное решение
	uint32_t			bV5ProfileData;	//	от Microsoft
	uint32_t			bV5ProfileSize;
	uint32_t			bV5Reserved;
};

void alImageLoader_BMP_decompress8BitRLE(uint8_t*& rleData, uint8_t*& inds, uint32_t size, uint32_t width, uint32_t height);
void alImageLoader_BMP_decompress4BitRLE(uint8_t*& rleData, uint8_t*& inds, uint32_t size, uint32_t width, uint32_t height);

alImageLoaderBMP::alImageLoaderBMP()
{
}

alImageLoaderBMP::~alImageLoaderBMP()
{
}

alImage* alImageLoaderBMP::Load(const char* fn)
{
	AL_ASSERT_ST(fn);

	alFileBuffer fb;
	if (fb.ReadFile(fn))
	{
		auto i = Load(&fb);
		if (i)
			return i;
	}
	else
	{
		alLog::PrintWarning("BMP: failed to open file\n");
	}
	alLog::PrintWarning("BMP: file [%s]\n", fn);
	return 0;
}

alImage* alImageLoaderBMP::Load(alFileBuffer* fb)
{
	AL_ASSERT_ST(fb);

	BitmapHeader header;
	BitmapInfoHeader_v5 info;

	//if (fread(&header, 1, sizeof(BitmapHeader), file) != sizeof(BitmapHeader))
	if(fb->Read(&header, sizeof(BitmapHeader)) != sizeof(BitmapHeader))
	{
		alLog::PrintWarning("BMP: Not a correct BMP file\n");
		return 0;
	}

	if (header.bfType != 19778)
	{
		alLog::PrintWarning("BMP: Not a correct BMP file\n");
		return 0;
	}

	fb->Read(&info, sizeof(BitmapInfoHeader_v5));
	if (info.bV5Size < 40U)
	{
		alLog::PrintWarning("BMP: Bad header size\n");
		return 0;
	}

	if (!info.bV5Width || !info.bV5Height)
	{
		alLog::PrintWarning("BMP: Bad file\n");
		return 0;
	}

	if (info.bV5BitCount != 1u &&
		info.bV5BitCount != 4u &&
		info.bV5BitCount != 8u &&
		info.bV5BitCount != 16u &&
		info.bV5BitCount != 24u &&
		info.bV5BitCount != 32u)
	{
		alLog::PrintWarning("BMP: Bad bit count\n");
		return 0;
	}

	alPtr<alImage> image = alCreate<alImage>();

	image.m_data->m_width = static_cast<uint32_t>(info.bV5Width);
	image.m_data->m_height = static_cast<uint32_t>(info.bV5Height);
	image.m_data->m_bits = info.bV5BitCount;

	bool flipPixel = false;

	if (image.m_data->m_bits == 24)
	{
		image.m_data->m_format = alImageFormat::R8G8B8;
		image.m_data->m_pitch = image.m_data->m_width * 3;
		image.m_data->m_dataSize = image.m_data->m_pitch * image.m_data->m_height;

		image.m_data->m_data = (uint8_t*)alMemory::Malloc(image.m_data->m_dataSize);

		fb->Seek(54, SEEK_SET);
		fb->Read(image.m_data->m_data, image.m_data->m_dataSize);
		flipPixel = true;
	}
	else if (image.m_data->m_bits == 32)
	{
		image.m_data->m_pitch = image.m_data->m_width * 4u;
		uint32_t offset = header.bfOffBits;

		if (info.bV5Compression == 3) // BI_BITFIELDS
		{
			fb->Seek(offset, SEEK_SET);
			image.m_data->m_format = alImageFormat::R8G8B8A8;

			image.m_data->m_dataSize = image.m_data->m_pitch * image.m_data->m_height;
			image.m_data->m_data = (uint8_t*)alMemory::Malloc(image.m_data->m_dataSize);

			fb->Read(image.m_data->m_data, image.m_data->m_dataSize);
			image.m_data->FlipPixel();
		}
		else
		{
			alLog::PrintWarning("BMP: unsupported format\n");
			return 0;
		}
	}
	else if (image.m_data->m_bits == 16u)
	{
		if (info.bV5Size != 40U && info.bV5Size != 56u)
		{
			alLog::PrintWarning("BMP: unsupported format\n");
			return 0;
		}

		image.m_data->m_pitch = image.m_data->m_width * 2u;

		if (info.bV5Size == 40U)
		{ // x1r5g5b5
			fb->Seek(54u, SEEK_SET);
			image.m_data->m_format = alImageFormat::X1R5G5B5;
		}
		else
		{
			if (info.bV5RedMask == 3840U &&
				info.bV5GreenMask == 240U &&
				info.bV5BlueMask == 15u)
			{
				if (info.bV5AlphaMask)
				{
					image.m_data->m_format = alImageFormat::A4R4G4B4;
				}
				else
				{
					image.m_data->m_format = alImageFormat::X4R4G4B4;
				}
			}
			else if (info.bV5RedMask == 63488u &&
				info.bV5GreenMask == 2016u &&
				info.bV5BlueMask == 31u)
			{
				image.m_data->m_format = alImageFormat::R5G6B5;
			}
			else if (info.bV5RedMask == 31744u &&
				info.bV5GreenMask == 992u &&
				info.bV5BlueMask == 31u)
			{
				if (info.bV5AlphaMask)
					image.m_data->m_format = alImageFormat::A1R5G5B5;
			}
			else
			{
				alLog::PrintWarning("BMP: unsupported format\n");
				return 0;
			}
			fb->Seek(70U, SEEK_SET);
		}
		image.m_data->m_dataSize = image.m_data->m_pitch * image.m_data->m_height;
		image.m_data->m_data = (uint8_t*)alMemory::Malloc(image.m_data->m_dataSize);

		fb->Read(image.m_data->m_data, image.m_data->m_dataSize);
	}
	else if (image.m_data->m_bits == 8u)
	{
		image.m_data->m_format = alImageFormat::R8G8B8A8;
		image.m_data->m_pitch = image.m_data->m_width * 4u;
		image.m_data->m_dataSize = image.m_data->m_pitch * image.m_data->m_height;
		fb->Seek(54u, SEEK_SET);
		uint32_t tableSize = header.bfOffBits - 54u;
		std::unique_ptr<uint8_t[]> table(new uint8_t[tableSize]);
		fb->Read(table.get(), tableSize);
		rgbquad* quadTable = reinterpret_cast<rgbquad*>(table.get());
		fb->Seek(header.bfOffBits, SEEK_SET);
		image.m_data->m_data = (uint8_t*)alMemory::Malloc(image.m_data->m_dataSize);
		uint32_t indSize = image.m_data->m_width * image.m_data->m_height;
		uint8_t* _data = new uint8_t[indSize];
		std::unique_ptr<uint8_t[]> inds(_data);
		if (!info.bV5Compression)
		{
			fb->Read(inds.get(), indSize);
		}
		else
		{
			uint8_t* _data2 = new uint8_t[info.bV5SizeImage];
			std::unique_ptr<uint8_t[]> rle(_data2);
			fb->Read(rle.get(), info.bV5SizeImage);
			alImageLoader_BMP_decompress8BitRLE(_data2, _data, info.bV5SizeImage, image.m_data->m_width, image.m_data->m_height);
		}
		uint8_t* u8_ptr = inds.get();
		for (uint32_t i = 0u, count = 0u; i < indSize; )
		{
			image.m_data->m_data[count] = quadTable[u8_ptr[i]].Red;
			image.m_data->m_data[count + 1u] = quadTable[u8_ptr[i]].Green;
			image.m_data->m_data[count + 2u] = quadTable[u8_ptr[i]].Blue;
			image.m_data->m_data[count + 3u] = 255u;
			count += 4u;
			i += 1u;
		}
	}
	else if (image.m_data->m_bits == 4u)
	{
		image.m_data->m_format = alImageFormat::R8G8B8A8;
		image.m_data->m_pitch = image.m_data->m_width * 4u;
		image.m_data->m_dataSize = image.m_data->m_pitch * image.m_data->m_height;
		fb->Seek(54u, SEEK_SET);
		uint32_t tableSize = header.bfOffBits - 54u;
		std::unique_ptr<uint8_t[]> table(new uint8_t[tableSize]);
		fb->Read(table.get(), tableSize);
		rgbquad* quadTable = reinterpret_cast<rgbquad*>(table.get());
		fb->Seek(header.bfOffBits, SEEK_SET);
		uint32_t indSize = image.m_data->m_width * image.m_data->m_height / 2;
		image.m_data->m_data = (uint8_t*)alMemory::Malloc(image.m_data->m_dataSize);
		std::unique_ptr<uint8_t[]> inds(new uint8_t[indSize]);
		uint8_t* u8_ptr = inds.get();
		if (!info.bV5Compression)
		{
			fb->Read(inds.get(), indSize);
		}
		else
		{
			std::unique_ptr<uint8_t[]> rle(new uint8_t[info.bV5SizeImage]);
			fb->Read(rle.get(), info.bV5SizeImage);
			uint8_t* rlePtr = rle.get();
			alImageLoader_BMP_decompress4BitRLE(rlePtr, u8_ptr, indSize, image.m_data->m_width, image.m_data->m_height);
		}
		for (uint32_t i = 0u, count = 0u; i < indSize; )
		{
			uint8_t color = u8_ptr[i];
			uint8_t part1 = color & 15u;
			uint8_t part2 = (color & 240U) >> 4u;

			image.m_data->m_data[count] = quadTable[part2].Red;
			image.m_data->m_data[count + 1u] = quadTable[part2].Green;
			image.m_data->m_data[count + 2u] = quadTable[part2].Blue;
			image.m_data->m_data[count + 3u] = 255u;
			image.m_data->m_data[count + 4u] = quadTable[part1].Red;
			image.m_data->m_data[count + 5u] = quadTable[part1].Green;
			image.m_data->m_data[count + 6u] = quadTable[part1].Blue;
			image.m_data->m_data[count + 7u] = 255u;
			count += 8u;
			i += 1u;
		}
	}
	else if (image.m_data->m_bits == 1u)
	{
		image.m_data->m_format = alImageFormat::One_bit;
		image.m_data->m_pitch = image.m_data->m_width;
		image.m_data->m_dataSize = info.bV5SizeImage;
		fb->Seek(header.bfOffBits, SEEK_SET);
		image.m_data->m_data = (uint8_t*)alMemory::Malloc(image.m_data->m_dataSize);
		fb->Read(image.m_data->m_data, image.m_data->m_dataSize);
	}
	else
	{
		alLog::PrintWarning("BMP: Bad file\n");
		return 0;
	}

	image.m_data->convertToR8G8B8A8();
	image.m_data->FlipVertical();
	if (flipPixel)
		image.m_data->FlipPixel();

	auto ret = image.m_data;
	image.m_data = 0;
	return ret;
}

bool alImageLoaderBMP::GetInfo(const char* fn, alImage* img)
{
	AL_ASSERT_ST(fn);
	AL_ASSERT_ST(img);

	alFileBuffer fb;
	if (fb.ReadFile(fn))
	{
		if (GetInfo(&fb, img))
			return true;
	}
	else
	{
		alLog::PrintWarning("BMP: failed to open file\n");
	}
	alLog::PrintWarning("BMP: file [%s]\n", fn);
	return true;
}

bool alImageLoaderBMP::GetInfo(alFileBuffer* fb, alImage* img)
{
	AL_ASSERT_ST(fb);
	AL_ASSERT_ST(img);

	BitmapHeader header;
	BitmapInfoHeader_v5 info;
	if (fb->Read(&header, sizeof(BitmapHeader)) != sizeof(BitmapHeader))
	{
		alLog::PrintWarning("BMP: Not a correct BMP file\n");
		return false;
	}

	fb->Read(&info, sizeof(BitmapInfoHeader_v5));
	if (info.bV5Size < 40U)
	{
		alLog::PrintWarning("BMP: Bad header size\n");
		return false;
	}

	img->m_width = static_cast<uint32_t>(info.bV5Width);
	img->m_height = static_cast<uint32_t>(info.bV5Height);
	img->m_bits = info.bV5BitCount;
	return true;
}

uint32_t alImageLoaderBMP::GetExtensionNum()
{
	return 1;
}

const char* alImageLoaderBMP::GetExtension(uint32_t i)
{
	return "bmp";
}

bool alImageLoaderBMP::Save(alImage* img, const char* fn)
{
	AL_ASSERT_ST(img);
	AL_ASSERT_ST(fn);

	return false;
}

