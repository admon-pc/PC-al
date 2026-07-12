#include "al.h"

#include "Image/alImage.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

#include "alImageLoader.PNG.h"

#include <iostream>
#include <fstream>
#include <filesystem>

#include <png.h>
#include <zlib.h>

static void __cdecl user_error_fn(png_structp png_ptr, png_const_charp msg) 
{
	fprintf(stderr, "%s\n", msg);
	longjmp(png_jmpbuf(png_ptr), 1);
}

static void __cdecl user_warning_fn(png_structp /*png_ptr*/, png_const_charp msg) 
{
	fprintf(stderr, "%s\n", msg);
}

class PNG
{
public:

	PNG() :
		png_ptr(nullptr),
		info_ptr(nullptr)
	{}

	~PNG() {
		if (info_ptr) {
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		}
		else if (png_ptr)
			png_destroy_read_struct(&png_ptr, NULL, NULL);
	}

	png_structp png_ptr;
	png_infop info_ptr;
};

void PNGAPI user_read_fn(png_structp png_ptr, png_bytep data, png_size_t length)
{
	alFileBuffer* fb = (alFileBuffer*)png_get_io_ptr(png_ptr);
	fb->Read(data, (uint32_t)length);
}

alImageLoaderPNG::alImageLoaderPNG()
{
}

alImageLoaderPNG::~alImageLoaderPNG()
{
}

alImage* alImageLoaderPNG::Load(const char* fn)
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
		alLog::PrintWarning("PNG: failed to open file\n");
	}
	alLog::PrintWarning("PNG: file [%s]\n", fn);
	return 0;
}

alImage* alImageLoaderPNG::Load(alFileBuffer* fb)
{
	AL_ASSERT_ST(fb);

	uint8_t buf[4];
	fb->Read(buf, 4);
	if (png_sig_cmp(buf, (png_size_t)0, 4))
	{
		alLog::PrintWarning("PNG: bad file\n");
	}

	PNG png;
	png.png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, user_error_fn, user_warning_fn);
	if (!png.png_ptr)
	{
		alLog::PrintWarning("PNG: bad file\n");
		return 0;
	}

	png.info_ptr = png_create_info_struct(png.png_ptr);
	if (!png.info_ptr)
	{
		alLog::PrintWarning("PNG: bad file\n");
		return 0;
	}

	if (setjmp(png_jmpbuf(png.png_ptr)))
	{
		alLog::PrintWarning("PNG: bad file\n");
		return 0;
	}

	png_set_read_fn(png.png_ptr, (void*)fb, user_read_fn);
	png_set_sig_bytes(png.png_ptr, 4);
	png_read_info(png.png_ptr, png.info_ptr);

	uint32_t w, h;
	int32_t bits;
	int32_t color_type;
	png_get_IHDR(png.png_ptr, png.info_ptr, &w, &h, &bits, &color_type, NULL, NULL, NULL);

	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png.png_ptr);

	if (bits < 8) {
		if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
			png_set_expand_gray_1_2_4_to_8(png.png_ptr);
		else
			png_set_packing(png.png_ptr);
	}

	if (png_get_valid(png.png_ptr, png.info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png.png_ptr);

	if (bits == 16)
		png_set_strip_16(png.png_ptr);

	if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png.png_ptr);

	int32_t intent;
	const float64_t screen_gamma = 2.2;

	if (png_get_sRGB(png.png_ptr, png.info_ptr, &intent))
		png_set_gamma(png.png_ptr, screen_gamma, 0.45455);
	else {
		double image_gamma;
		if (png_get_gAMA(png.png_ptr, png.info_ptr, &image_gamma))
			png_set_gamma(png.png_ptr, screen_gamma, image_gamma);
		else
			png_set_gamma(png.png_ptr, screen_gamma, 0.45455);
	}

	png_read_update_info(png.png_ptr, png.info_ptr);
	png_get_IHDR(png.png_ptr, png.info_ptr, &w, &h, &bits, &color_type,
		NULL, NULL, NULL);

	alImage* image = alCreate<alImage>();
	image->m_width = w;
	image->m_height = h;
	if (color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
		image->m_format = alImageFormat::R8G8B8A8;
		image->m_pitch = image->m_width * 4;
	}
	else {
		image->m_format = alImageFormat::R8G8B8;
		image->m_pitch = image->m_width * 3;
	}

	image->m_dataSize = image->m_pitch * image->m_height;
	image->m_data = (uint8_t*)alMemory::Malloc(image->m_dataSize);

	std::unique_ptr<png_bytep[]> row_pointers(new png_bytep[image->m_height]);
	for (uint32_t row = 0, p = 0; row < image->m_height; ++row) {
		row_pointers[row] = &image->m_data[p];
		p += image->m_pitch;
	}

	png_read_image(png.png_ptr, row_pointers.get());
	png_read_end(png.png_ptr, NULL);

	image->convertToR8G8B8A8();

	return image;
}

bool alImageLoaderPNG::GetInfo(const char* fn, alImage* img)
{
	AL_ASSERT_ST(fn);
	AL_ASSERT_ST(img);

	alFileBuffer fb;
	if (fb.ReadFile(fn))
	{
		bool r = GetInfo(&fb, img);
		if (r)
			return true;
	}
	else
	{
		alLog::PrintWarning("PNG: failed to open file\n");
	}
	alLog::PrintWarning("PNG: file [%s]\n", fn);
	return false;
}

bool alImageLoaderPNG::GetInfo(alFileBuffer* fb, alImage* img)
{
	AL_ASSERT_ST(fb);
	AL_ASSERT_ST(img);

	uint8_t buf[4];
	fb->Read((char*)buf, 4);
	if (png_sig_cmp(buf, (png_size_t)0, 4))
	{
		alLog::PrintWarning("PNG: bad file\n");
		return false;
	}

	PNG png;
	png.png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, user_error_fn, user_warning_fn);
	if (!png.png_ptr)
	{
		alLog::PrintWarning("PNG: bad file\n");
		return false;
	}

	png.info_ptr = png_create_info_struct(png.png_ptr);
	if (!png.info_ptr)
	{
		alLog::PrintWarning("PNG: bad file\n");
		return false;
	}

	if (setjmp(png_jmpbuf(png.png_ptr)))
	{
		alLog::PrintWarning("PNG: bad file\n");
		return false;
	}

	png_set_read_fn(png.png_ptr, (void*)fb, user_read_fn);
	png_set_sig_bytes(png.png_ptr, 4);
	png_read_info(png.png_ptr, png.info_ptr);
	img->m_width = png_get_image_width(png.png_ptr, png.info_ptr);
	img->m_height = png_get_image_height(png.png_ptr, png.info_ptr);
	return true;
}

uint32_t alImageLoaderPNG::GetExtensionNum()
{
	return 1;
}

const char* alImageLoaderPNG::GetExtension(uint32_t i)
{
	return "png";
}

bool alImageLoaderPNG::Save(alImage* img, const char* p)
{
	AL_ASSERT_ST(img);
	AL_ASSERT_ST(p);
	
	FILE* fp = NULL;
	PNG png;

	alStringA str8(p);

	fopen_s(&fp, str8.data(), "wb");
	if (fp == NULL)
	{
		alLog::PrintWarning("PNG: failed to open file\n");
		alLog::PrintWarning("PNG: file [%s]\n", p);
		return false;
	}

	// Initialize write structure
	png.png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png.png_ptr == NULL)
	{
		alLog::PrintWarning("PNG: Could not allocate write struct\n");
		alLog::PrintWarning("PNG: file [%s]\n", p);
		return false;
	}

	// Initialize info structure
	png.info_ptr = png_create_info_struct(png.png_ptr);
	if (png.info_ptr == NULL)
	{
		alLog::PrintWarning("PNG: Could not allocate info struct\n");
		alLog::PrintWarning("PNG: file [%s]\n", p);
		return false;
	}

	// Setup Exception handling
	if (setjmp(png_jmpbuf(png.png_ptr)))
	{
		alLog::PrintWarning("PNG: Error during png creation\n");
		alLog::PrintWarning("PNG: file [%s]\n", p);
		return false;
	}

	png_init_io(png.png_ptr, fp);

	// Write header (8 bit colour depth)
	png_set_IHDR(png.png_ptr, png.info_ptr, img->m_width, img->m_height,
		8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png.png_ptr, png.info_ptr);

	auto row = (png_bytep)malloc(4 * img->m_width);

	uint8_t* imgData = &img->m_data[0];

	uint32_t y = 0;
	for (y = 0; y < img->m_height; ++y)
	{
		memcpy(&row[0], &imgData[0], img->m_pitch);
		png_write_row(png.png_ptr, row);
		imgData += img->m_pitch;
	}

	// End write
	png_write_end(png.png_ptr, NULL);

	free(row);
	fclose(fp);

	return true;
}


