#include "al.h"
#include "GUI/alGUI.h"
#include "GS/alGSTexture.h"
#include "GS/alGS.h"
#include "Archive/alArchive.h"

#include "Image/alImage.h"
#include "Image/alImageLoader.h"

#include "Classes/alTextReader.h"

#include "System/alSystemWindow.h"
#include "System/alSystemWindowWin32.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

alGUIFont::alGUIFont()
{
	for (uint32_t i = 0; i < 0x10FFFF; ++i)
	{
		m_glyphs[i] = 0;
	}
}

alGUIFont::~alGUIFont()
{
	for (size_t i = 0; i < m_textures.m_size; ++i)
	{
		AL_DESTROY(m_textures.m_data[i]);
	}

	for (uint32_t i = 0; i < 0x10FFFF; ++i)
	{
		AL_DESTROY(m_glyphs[i]);
	}
}

alGUIFontGlyph* alGUIFont::GetGlyph(char32_t ch)
{
	if (ch && ch < 0x10FFFF)
		return m_glyphs[(uint32_t)ch];
	return nullptr;
}

alGUIFontGlyph* alGUIFont::GetUnknownGlyph()
{
	return m_glyphs[(uint32_t)m_unknownGlyph];
}

void alGUIFont::SetGlyph(char32_t ch, int32_t textureID, float32_t h, float32_t w, alVec4f* uv)
{
	if (ch && ch < 0x10FFFF)
	{
		if (!m_glyphs[(uint32_t)ch])
			m_glyphs[(uint32_t)ch] = alCreate<alGUIFontGlyph>();
		
		auto g = m_glyphs[(uint32_t)ch];

		g->height = h;
		g->width = w;
		g->symbol = ch;
		g->textureID = textureID;
		g->lt.x = uv->x;
		g->lt.y = uv->y;
		g->rb.x = uv->z;
		g->rb.y = uv->w;

		if (h > m_maxHeight)
			m_maxHeight = h;

		if (w > m_maxWidth)
			m_maxWidth = w;
	}
}

float32_t alGUIFont::GetTextLength(const char32_t* str, size_t str_len)
{
	AL_ASSERT_ST(str);
	float32_t len = 0;
	
	for (size_t i = 0; i < str_len; ++i)
	{
		auto g = GetGlyph(str[i]);
		if (!g)
			continue;
		len += g->width - g->overhang + m_spacing;

		if (g->symbol == u' ')
			len += m_spaceWidth;

		if (g->symbol == u'\t')
			len += m_tabWidth;
	}
	return len;
}

size_t alGUIFont::AddTexture(alGSTexture* t)
{
	if (t)
	{
		m_textures.push_back(t);
		return m_textures.size()-1;
	}
	return -1;
}

bool alGUIFont::Load(const char* fn, alGS* gs)
{
	AL_ASSERT_ST(fn);
	AL_ASSERT_ST(gs);
	bool ret = false;
	if (fn && gs)
	{
		alStringA name;
		alLib::GetFileName(fn, &name);

		auto zip = alArchive::LoadZip(fn);
		if (zip)
		{
			for (int i = 0; i < 100; ++i)
			{
				char buf[0xff];
				sprintf_s(buf, 0xff, "%s%i.png", name.c_str(), i);

				alFileBuffer* fb = zip->GetFile(buf);
				if (fb)
				{
					alLog::Print("Load Image: ../%s\n", buf);
					alImage * image = alLib::GetImageLoaderPNG()->Load(fb);
					if (image)
					{
						alGSTextureInfo ti;
						ti.m_imagePtr = image;
						ti.m_filter = alGSTextureFilter::PPP;
						ti.UseMipMaps(false);

						alGSTexture* texture = gs->CreateTexture(&ti);
						AL_DESTROY(image);

						if (texture)
						{
							m_textures.push_back(texture);
						}
					}
				}
			}

			char buf[0xff];
			sprintf_s(buf, 0xff, "%s.txt", name.c_str());
			alFileBuffer* fileBuffer = zip->GetFile(buf);
			if (fileBuffer)
			{
				auto data = fileBuffer->Data();
				const char32_t* str = (const char32_t*)(&data[4]);

				alUnicodeString ustr;
				ustr.Append(str, (fileBuffer->Size() / 4) - 4);

				char32_t ch = 0;
				alTextReader<char32_t> tr(ustr.Data(), ustr.Size());
				while (!tr.IsEnd())
				{
					auto line = tr.GetLine(true);
					uint32_t charCode = line.GetUint();
					uint32_t leftTopX = line.GetUint();
					uint32_t leftTopY = line.GetUint();
					uint32_t rightBotX = line.GetUint();
					uint32_t rightBotY = line.GetUint();
					int32_t underhang = line.GetInt();
					int32_t overhang = line.GetInt();
					uint32_t textureID = line.GetUint();
					if (charCode < 0x10FFFF)
					{
						if (!m_glyphs[charCode])
							m_glyphs[charCode] = alCreate<alGUIFontGlyph>();

						auto glyph = m_glyphs[charCode];

						glyph->height = float32_t(rightBotY - leftTopY);
						glyph->width = float32_t(rightBotX - leftTopX);
						glyph->overhang = (float32_t)overhang;
						glyph->underhang = (float32_t)underhang;
						glyph->symbol = (char32_t)charCode;
						glyph->textureID = textureID;

						if (glyph->height > m_maxHeight)
							m_maxHeight = glyph->height;

						if (glyph->width > m_maxWidth)
							m_maxWidth = glyph->width;

						auto texture = this->GetTexture(textureID);
						if (texture)
						{
							alVec2f tsz((float32_t)texture->GetWidth(), (float32_t)texture->GetHeight());
							float32_t mulX = 1.f / tsz.x;
							float32_t mulY = 1.f / tsz.y;

						//	glyph->lt.x = leftTopX * mulX;
						//	glyph->lt.y = leftTopY * mulY;
						//	glyph->rb.x = rightBotX * mulX;
						//	glyph->rb.y = rightBotY * mulY;
							glyph->lt = texture->GetUV((float32_t)leftTopX, (float32_t)leftTopY);
							glyph->rb = texture->GetUV((float32_t)rightBotX, (float32_t)rightBotY);
							ret = true;
						}
					}
				}
			}

			AL_DESTROY(zip);
		}
	}

	if (ret)
	{
		auto getRidOfUnprintableChars = [](char32_t ch, alGUIFont* thisFont)
		{
			if (!thisFont->m_glyphs[ch])
				thisFont->m_glyphs[ch] = alCreate<alGUIFontGlyph>();
			thisFont->m_glyphs[ch]->width = 0;
			thisFont->m_glyphs[ch]->symbol = U' ';
		};
		getRidOfUnprintableChars(U'\n', this);
		getRidOfUnprintableChars(U'\r', this);
		getRidOfUnprintableChars(U'\t', this);
		getRidOfUnprintableChars(U'\f', this);
		getRidOfUnprintableChars(U'\v', this);
	}

	return ret;
}

alGSTexture* alGUIFont::GetTexture(size_t i)
{
	if (i < m_textures.m_size)
		return m_textures.m_data[i];
	return 0;
}

size_t alGUIFont::GetTextureNum()
{
	return m_textures.m_size;
}
