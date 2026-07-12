#ifndef _AL_GUIFONTH_
#define _AL_GUIFONTH_


struct alGUIFontGlyph
{
	int32_t textureID = 0;
	float32_t width = 0;
	float32_t height = 0;
	float32_t underhang = 0;
	float32_t overhang = 0;
	char32_t symbol = 0;


	// texture coords, left-top left-bottom etc.
	alVec2f lt, rb;
};

class alGUIFont
{
	alGUIFontGlyph* m_glyphs[0x10FFFF];
	alArray<alGSTexture*> m_textures;
public:
	alGUIFont();
	virtual ~alGUIFont();

	bool Load(const char* fn, alGS*);
	alGUIFontGlyph* GetGlyph(char32_t ch);
	alGUIFontGlyph* GetUnknownGlyph();
	void SetGlyph(char32_t ch, int32_t textureID, float32_t h, float32_t w, alVec4f* uv);

	float32_t GetTextLength(const char32_t* str, size_t i);
	alVec2f GetFontSize()
	{
		return alVec2f(m_maxWidth, m_maxHeight);
	}

	char32_t m_unknownGlyph = U'?';

	// return index in array
	//   -1 if error
	size_t AddTexture(alGSTexture*);

	alGSTexture* GetTexture(size_t);
	size_t GetTextureNum();

	float32_t m_maxHeight = 0;
	float32_t m_maxWidth = 0;

	float32_t m_spaceWidth = 4;
	float32_t m_tabWidth = 10;
	float32_t m_spacing = 0;
};


#endif

