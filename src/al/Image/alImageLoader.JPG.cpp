#include "al.h"

#include "Image/alImage.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

#ifdef AL_CONFIG_COMPILE_WITH_JPEG
#include "alImageLoader.JPG.h"

#include "jpeglib.h"
#include <setjmp.h>

struct my_error_mgr 
{
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
};
typedef struct my_error_mgr* my_error_ptr;

void my_error_exit(j_common_ptr cinfo)
{
	my_error_ptr myerr = (my_error_ptr)cinfo->err;
	(*cinfo->err->output_message) (cinfo);
	longjmp(myerr->setjmp_buffer, 1);
}

class JPG_Loader 
{
public:
	JPG_Loader() {}
	~JPG_Loader() {
		jpeg_destroy_decompress(&cinfo);
	}

	bool GetInfo(alFileBuffer* fb, alImage* img)
	{
		JSAMPARRAY buffer = 0;
		int row_stride = 0;

		cinfo.err = jpeg_std_error(&jerr.pub);
		jerr.pub.error_exit = my_error_exit;
		if (setjmp(jerr.setjmp_buffer)) 
		{
			return false;
		}
		jpeg_create_decompress(&cinfo);
		jpeg_mem_src(&cinfo,fb->Data(), fb->Size());
		jpeg_read_header(&cinfo, TRUE);

		img->m_width = static_cast<uint32_t>(cinfo.image_width);
		img->m_height = static_cast<uint32_t>(cinfo.image_height);
		img->m_bits = 24;
		return true;
	}

	alImage* Load(alFileBuffer* fb)
	{
		cinfo.err = jpeg_std_error(&jerr.pub);
		jerr.pub.error_exit = my_error_exit;
		if (setjmp(jerr.setjmp_buffer)) 
		{
			alLog::PrintWarning("JPG: bad file\n");
			return 0;
		}
		jpeg_create_decompress(&cinfo);
		jpeg_mem_src(&cinfo, fb->Data(), fb->Size());
		jpeg_read_header(&cinfo, TRUE);
		jpeg_start_decompress(&cinfo);


		alPtr<alImage> image = alCreate<alImage>();
		auto img = image.m_data;

		img->m_width = static_cast<uint32_t>(cinfo.image_width);
		img->m_height = static_cast<uint32_t>(cinfo.image_height);
		img->m_bits = 24;
		img->m_format = alImageFormat::R8G8B8;
		img->m_pitch = cinfo.output_width * cinfo.output_components;
		img->m_dataSize = img->m_pitch * img->m_height;
		img->m_data = (uint8_t*)alMemory::Malloc(img->m_dataSize);

		uint8_t* imageDataPtr = img->m_data;

		JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)
			((j_common_ptr)&cinfo, JPOOL_IMAGE, img->m_pitch, 1);

		while (cinfo.output_scanline < cinfo.output_height) 
		{
			jpeg_read_scanlines(&cinfo, buffer, 1);
			memcpy(&imageDataPtr[0], buffer[0], img->m_pitch);
			imageDataPtr += img->m_pitch;
		}
		jpeg_finish_decompress(&cinfo);

		img->convertToR8G8B8A8();

		auto ret = image.m_data;
		image.m_data = 0;
		return ret;
	}

	bool Save(alImage* img, const char* fn)
	{
		FILE* outfile = 0;
		fopen_s(&outfile, fn, "wb");
		if (outfile)
		{
			cinfo_compress.err = jpeg_std_error(&jerr.pub);
			jpeg_create_compress(&cinfo_compress);

			jpeg_stdio_dest(&cinfo_compress, outfile);
			cinfo_compress.image_width = img->m_width;
			cinfo_compress.image_height = img->m_height;
			cinfo_compress.input_components = 3;
			cinfo_compress.in_color_space = JCS_RGB;
			jpeg_set_defaults(&cinfo_compress);
			jpeg_set_quality(&cinfo_compress, 90, TRUE);
			jpeg_start_compress(&cinfo_compress, TRUE);
			int32_t row_stride = img->m_width * 3;
			JSAMPROW row_pointer[1];

			struct rgb_t
			{
				uint8_t r;
				uint8_t g;
				uint8_t b;
			};
			struct rgba_t
			{
				uint8_t r;
				uint8_t g;
				uint8_t b;
				uint8_t a;
			};

			uint8_t* imageRowInput = img->m_data;
			uint8_t* imageRowRGB = (uint8_t*)malloc(img->m_width * 3);

			while (cinfo_compress.next_scanline < cinfo.image_height)
			{
				for (uint32_t i = 0; i < img->m_width; ++i)
				{
					switch (img->m_format)
					{
					default:
					case alImageFormat::R8G8B8:
						imageRowRGB = imageRowInput;
						break;
					case alImageFormat::R8G8B8A8:
					{
						rgba_t* _rgba = (rgba_t*)imageRowInput;
						rgb_t* _rgb = (rgb_t*)imageRowRGB;
						_rgb[i].r = _rgba[i].r;
						_rgb[i].g = _rgba[i].g;
						_rgb[i].b = _rgba[i].b;
					}break;
					}
				}

				imageRowInput += img->m_pitch;

				//row_pointer[0] = &image_buffer[cinfo_compress.next_scanline * row_stride];
				row_pointer[0] = imageRowRGB;
				(void)jpeg_write_scanlines(&cinfo_compress, row_pointer, 1);
			}

			free(imageRowRGB);

			jpeg_finish_compress(&cinfo_compress);
			jpeg_destroy_compress(&cinfo_compress);

			fclose(outfile);
		}

		return false;
	}

	jpeg_decompress_struct cinfo;
	jpeg_compress_struct cinfo_compress;
	my_error_mgr jerr;
};

alImageLoaderJPG::alImageLoaderJPG()
{
}

alImageLoaderJPG::~alImageLoaderJPG()
{
}

alImage* alImageLoaderJPG::Load(const char* fn)
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
		alLog::PrintWarning("JPG: failed to open file\n");
	}
	alLog::PrintWarning("JPG: file [%s]\n", fn);
	return 0;
}

alImage* alImageLoaderJPG::Load(alFileBuffer* fb)
{
	AL_ASSERT_ST(fb);
	JPG_Loader loader;
	return loader.Load(fb);
}

bool alImageLoaderJPG::GetInfo(const char* fn, alImage* img)
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
		alLog::PrintWarning("JPG: failed to open file\n");
	}
	alLog::PrintWarning("JPG: file [%s]\n", fn);
	return false;
}

bool alImageLoaderJPG::GetInfo(alFileBuffer* fb, alImage* img)
{
	AL_ASSERT_ST(fb);
	AL_ASSERT_ST(img);
	JPG_Loader loader;
	if (loader.GetInfo(fb, img))
		return true;
	alLog::PrintWarning("JPG: bad file\n");
	return false;
}

uint32_t alImageLoaderJPG::GetExtensionNum()
{
	return 3;
}

const char* alImageLoaderJPG::GetExtension(uint32_t i)
{
	switch (i)
	{
	default:
	case 0:
		return "jpg";
	case 1:
		return "jpeg";
	case 2:
		return "jfif";
	}
	return "jpg";
}

bool alImageLoaderJPG::Save(alImage* img, const char* fn)
{
	AL_ASSERT_ST(img);
	AL_ASSERT_ST(fn);
	JPG_Loader loader;
	return loader.Save(img, fn);
}


#endif

