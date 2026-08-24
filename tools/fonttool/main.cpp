#include <windows.h>

#include "al.h"
#include "Image/alImage.h"

#include "System/alSystemWindow.h"
#include "System/alSystemWindowWin32.h"
#include "GS/alGS.h"
#include "Input/alInput.h"
#include "Classes/alColor.h"

#include "GUI/alGUI.h"

#include "Archive/alArchive.h"
#include "Containers/alArray.h"

#ifdef LoadImage
#undef LoadImage
#endif


AL_LINK_LIBRARY(al);

//enum
//{
//	TextureID_NEW_GREEN,
//	TextureID_NEW_RED,
//	TextureID_OPEN_GREEN,
//	TextureID_OPEN_RED,
//	TextureID_PLUS_GREEN,
//	TextureID_PLUS_RED,
//	TextureID_MINUS_GREEN,
//	TextureID_MINUS_RED,
//	TextureID_FROMSCRATCH_GREEN,
//	TextureID_FROMSCRATCH_RED,
//	TextureID_GENERATE_GREEN,
//	TextureID_GENERATE_RED,
//	
//	TextureID__end,
//};

#define FontToolGUIID_btnOpen 1
#define FontToolGUIID_btnGenerate 2
#define FontToolGUIID_btnCreate 3
#define FontToolGUIID_checkHideUnused 4

class FontTool_buttonIcon : public alGUIButtonIcon
{
public:
	FontTool_buttonIcon(alGUIContext* ct,
		alGUITextureAtlas* ta, uint32_t ii,
		const alVec2f& position, const alVec2f& size)
		:
		alGUIButtonIcon(ct, ta, ii, position, size)
	{}
	virtual ~FontTool_buttonIcon()
	{
	}
	AL_DECLARE_DEFAULT_ALLOCATOR(FontTool_buttonIcon);
	virtual void OnButtonToggleOn() override;
	virtual void OnButtonToggleOff() override;
};

class FontTool_combo_unicodeRange : public alGUIComboBox
{
public:
	FontTool_combo_unicodeRange(alGUIContext* ct, const alVec2f& position, const alVec2f& size)
		:alGUIComboBox(ct, position, size) {}
	virtual ~FontTool_combo_unicodeRange() {}
	AL_DECLARE_DEFAULT_ALLOCATOR(FontTool_combo_unicodeRange);
	virtual void OnComboSelectItem(size_t) override;
};

struct UnicodeRangeInfo
{
	char32_t m_title[100];
	size_t m_index = 0;
};
UnicodeRangeInfo g_UnicodeRangeInfo[] =
{
	{U"Basic Latin 0x0-0x7F", 0},
	{U"Latin-1 Supplement 0x80-0xFF", 0x80},
	{U"Latin Extended-A 0x100-0x17F", 0x100},
	{U"Latin Extended-B 0x180-0x24F", 0x180},
	{U"IPA Extensions 0x250-0x2AF", 0x250},
	{U"Spacing Modifier Letters 0x2B0-0x2FF", 0x2B0},
	{U"Combining Diacritical Marks 0x300-0x36F", 0x300},
	{U"Greek and Coptic 0x370-0x3FF", 0x370},
	{U"Cyrillic 0x400-0x4FF", 0x400},
	{U"Cyrillic Supplement 0x500-0x52F", 0x500},
	{U"Armenian 0x530-0x58F", 0x530},
	{U"Hebrew 0x590-0x5FF", 0x590},
	{U"Arabic 0x600-0x6FF", 0x600},
	{U"Syriac 0x700-0x74F", 0x700},
	{U"Arabic Supplement 0x750-0x77F", 0x750},
	{U"Thaana 0x780-0x7BF", 0x780},
	{U"NKo 0x7C0-0x7FF", 0x7C0},
	{U"Samaritan 0x800-0x83F", 0x800},
	{U"Mandaic 0x840-0x85F", 0x840},
	{U"Syriac Supplement 0x860-0x86F", 0x860},
	{U"Arabic Extended-B 0x870-0x89F", 0x870},
	{U"Arabic Extended-A 0x8A0-0x8FF", 0x8A0},
	{U"Devanagari 0x900-0x97F", 0x900},
	{U"Bengali 0x980-0x9FF", 0x980},
	{U"Gurmukhi 0xA00-0xA7F", 0xA00},
	{U"Gujarati 0xA80-0xAFF", 0xA80},
	{U"Oriya 0xB00-0xB7F", 0xB00},
	{U"Tamil 0xB80-0xBFF", 0xB80},
	{U"Telugu 0xC00-0xC7F", 0xC00},
	{U"Kannada 0xC80-0xCFF", 0xC80},
	{U"Malayalam 0xD00-0xD7F", 0xD00},
	{U"Sinhala 0xD80-0xDFF", 0xD80},
	{U"Thai 0xE00-0xE7F", 0xE00},
	{U"Lao 0xE80-0xEFF", 0xE80},
	{U"Tibetan 0xF00-0xFFF", 0xF00},
	{U"Myanmar 0x1000-0x109F", 0x1000},
	{U"Georgian 0x10A0-0x10FF", 0x10A0},
	{U"Hangul Jamo 0x1100-0x11FF", 0x1100},
	{U"Ethiopic 0x1200-0x137F", 0x1200},
	{U"Ethiopic Supplement 0x1380-0x139F", 0x1380},
	{U"Cherokee 0x13A0-0x13FF", 0x13A0},
	{U"Unified Canadian Aboriginal Syllabics 0x1400-0x167F", 0x1400},
	{U"Ogham 0x1680-0x169F", 0x1680},
	{U"Runic 0x16A0-0x16FF", 0x16A0},
	{U"Tagalog 0x1700-0x171F", 0x1700},
	{U"Hanunoo 0x1720-0x173F", 0x1720},
	{U"Buhid 0x1740-0x175F", 0x1740},
	{U"Tagbanwa 0x1760-0x177F", 0x1760},
	{U"Khmer 0x1780-0x17FF", 0x1780},
	{U"Mongolian 0x1800-0x18AF", 0x1800},
	{U"Unified Canadian Aboriginal Syllabics Extended 0x18B0-0x18FF", 0x18B0},
	//...
	{U"General Punctuation 0x2000-0x206F", 0x2000},
	//...
	{U"Currency Symbols 0x20A0-0x20CF", 0x20A0},
	//...
	{U"Number Forms 0x2150-0x218F", 0x2150},
	{U"Arrows 0x2190-0x21FF", 0x2190},
	//...
	{U"Emoticons (Emoji) 0x1F600-0x1F64F", 0x1F600},
};

struct GlyphInfo
{
	int m_width = 0;

	// rgba
	uint8_t* m_data = 0;

	/*alImage* image = 0; //61 для равно
	
	int textureID = 0;

	int overhang = 0;
	int underhang = 0;
	

	int leftTopX = 0;
	int leftTopY = 0;
	int rightBottomX = 0;
	int rightBottomY = 0;

	char32_t character = 0;*/
};

class FontTool_button : public alGUIButton
{
public:
	FontTool_button(alGUIContext* ct, const alVec2f& position, const alVec2f& size)
		:
		alGUIButton(ct, position, size) {}
	virtual ~FontTool_button() {}
	AL_DECLARE_DEFAULT_ALLOCATOR(FontTool_button);
	virtual void OnButtonRelease() override;
};



class SystemWindowCallback;
class FontTool
{
	//void _state_NewOrOpen();
	//void _state_GenerateOrFromScratch();
	//void _state_Generate();
	//void _state_Edit();

//	void (FontTool::*OnRun)();

	alGUIContext* m_guiContext = 0;
	alGUIPanel* m_guiPanel_first = 0;
	alGUIPanel* m_guiPanel_edit = 0;
	alGUITextureAtlas* m_textureAtlas = 0;
	alGSTexture* m_textureAtlasTexture = 0;

	FontTool_combo_unicodeRange* m_comboRanges = 0;
	FontTool_buttonIcon* m_checkHideUnused = 0;

	uint32_t m_visibleLineNum = 0;
	uint32_t m_visibleCellNum = 0;
	uint32_t m_startDrawCellIndex = 0;

	uint32_t m_cellsInRow = 8;
	uint32_t m_cellSizeX = 64;
	uint32_t m_cellSizeY = 64;
	float32_t m_cellPanelWidth = 0.f;
	alVec4f m_cellPanelRect;
	void _moveUpView(uint32_t);
	void _moveDownView(uint32_t);
	
	float32_t m_editorCellSize = 0.f;
	alVec4f m_editRect;

	uint32_t m_selected = 0;
	void OnSelect();

public:
	FontTool();
	~FontTool();

	bool Init();
	void Run();

	void StartEdit();

	void OnButtonGenerate();
	void OnButtonOpen();
	void OnButtonCreate();
	
	void OnRebuild();
	void OnDraw();
	void OnUpdate();

	void create_tmp_font();

	bool m_hideUnUsed = false;
	bool m_run = true;
	bool m_editMode = false;
	SystemWindowCallback* m_windowCallback = 0;
	alSystemWindow* m_mainWindow = 0;
	alGSTexture* m_textureNoData = 0;
	alGSTexture* m_textureSelect = 0;
	alGS* m_gs = 0;

	GlyphInfo m_glyphs[0x10FFFF];
	float m_fontHeightMax = 0.f;
	float m_fontWidthMax = 0.f;

	alGSTexture* m_whiteTexture = 0;
	alGUIFont* m_fontGUI = 0;
	alGUIFont* m_fontDefault = 0;

	void InitCellTexture();
	alGSTexture* m_cellTexture = 0;

	void GoTo(size_t);
	//alGSTextureCacheNode* m_guiTextureNodes[TextureID__end];
	//alGSTextureCache* m_guiTextures = 0;
};

void FontTool_combo_unicodeRange::OnComboSelectItem(size_t index)
{
	UnicodeRangeInfo* ptrInfo = (UnicodeRangeInfo*)m_items;
	uint8_t* ptr = (uint8_t*)m_items;
	m_text = (char32_t*)(&ptr[index * m_stride] + m_textOffset);
	FontTool* app = (FontTool*)GetUserData();
	app->GoTo(ptrInfo[index].m_index);
}
void FontTool_button::OnButtonRelease()
{
	FontTool* app = (FontTool*)GetUserData();
	if (GetID() == FontToolGUIID_btnOpen)
	{
		//example->m_buttonExitPressed = true;
		app->OnButtonOpen();
	}

	if (GetID() == FontToolGUIID_btnGenerate)
	{
		app->OnButtonGenerate();
	}

	if (GetID() == FontToolGUIID_btnCreate)
	{
		app->OnButtonCreate();
	}
}
void FontTool_buttonIcon::OnButtonToggleOn()
{
	FontTool* app = (FontTool*)GetUserData();
	if (GetID() == FontToolGUIID_checkHideUnused)
	{
		app->m_hideUnUsed = true;
	}
}

void FontTool_buttonIcon::OnButtonToggleOff()
{
	FontTool* app = (FontTool*)GetUserData();
	if (GetID() == FontToolGUIID_checkHideUnused)
	{
		app->m_hideUnUsed = false;
	}
}
class SystemWindowCallback : public alSystemWindowCallback
{
	FontTool* m_demo = 0;
public:
	SystemWindowCallback(FontTool* dd) : m_demo(dd) {}
	virtual ~SystemWindowCallback() {}

	virtual void OnSizeChanged(alSystemWindow*)
	{
		m_demo->OnRebuild();
	}
	
	virtual alVec2i OnGPUUpdateSize(alSystemWindow* w) 
	{
		alVec2i s;
		s.x = w->m_clientSize.x / 2;
		s.y = w->m_clientSize.y / 2;
		return s; 
	}
	
	virtual alVec2i OnMinMaxInfo(alSystemWindow* w)
	{
		return alVec2i(800, 600); 
	}

	virtual void OnClose(alSystemWindow* window)
	{
		auto windowID = window->GetID();
		if (!windowID)
		{
			m_demo->m_run = false;
		}
	}
};



FontTool::FontTool()
{
//	OnRun = &FontTool::_state_NewOrOpen;

	for (size_t i = 0; i < 0x10FFFF; i++)
	{
		m_glyphs[i].m_data = 0;
	}

	/*for (int i = 0; i < TextureID__end; ++i)
	{
		m_guiTextureNodes[i] = 0;
	}*/
	m_windowCallback = alCreate<SystemWindowCallback>(this);
}

FontTool::~FontTool()
{
	for (size_t i = 0; i < 0x10FFFF; i++)
	{
		AL_FREE(m_glyphs[i].m_data);
	}
	AL_DESTROY(m_textureNoData);
	AL_DESTROY(m_textureSelect);
	AL_DESTROY(m_textureAtlas);
	AL_DESTROY(m_textureAtlasTexture);
	AL_DESTROY(m_fontGUI);
	AL_DESTROY(m_guiContext);
	//AL_DESTROY(m_guiTextures);
	AL_DESTROY(m_cellTexture);
	AL_DESTROY(m_gs);
	AL_DESTROY(m_windowCallback);
	AL_DESTROY(m_mainWindow);
}

bool FontTool::Init()
{
	m_mainWindow = alLib::CreateSystemWindow(m_windowCallback);
	if (!m_mainWindow)
		return false;


	m_mainWindow->Show();

	m_gs = alLib::CreateGS(alVideoDriverType::Direct3D11);
	if (!m_gs->Init(m_mainWindow))
		return false;
	m_gs->SetClearColor(ColorDarkGrey);
	{
		alImage img;
		alColor dataColor[2] = { ColorTransparent, ColorWhite };
		uint8_t data[] =
		{
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
			1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1,
			1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,
			1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
			1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,
			1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1,
			1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		};
		img.Create(32,32);
		img.Fill(data, dataColor, alVec2u(32,32), alVec2u(), 0, 0);
		m_textureNoData = m_gs->CreateTexture(&img);
	}
	{
		alImage img;
		alColor dataColor[2] = { ColorTransparent, ColorWhite };
		uint8_t data[] =
		{
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		};
		img.Create(32, 32);
		img.Fill(data, dataColor, alVec2u(32, 32), alVec2u(), 0, 0);
		m_textureSelect = m_gs->CreateTexture(&img);
	}

	{
		auto img = alLib::LoadALImage("../data/textures/gui.png");
		if (img)
		{
			m_textureAtlasTexture = m_gs->CreateTexturePoint(img);
			AL_DESTROY(img);
		}
	}
	m_textureAtlas = alCreate<alGUITextureAtlas>(m_textureAtlasTexture);
	uint32_t iconIndex_iconBase = m_textureAtlas->AddUV(alVec2u(0, 0), alVec2u(14, 14));
	uint32_t iconIndex_iconCheck1 = m_textureAtlas->AddUV(alVec2u(14, 0), alVec2u(14, 14));
	uint32_t iconIndex_iconCheck2 = m_textureAtlas->AddUV(alVec2u(28, 0), alVec2u(14, 14));

	alLib::InitializeDefaultFont(m_gs);
	m_fontDefault = alLib::GetDefaultFont();

	m_fontGUI = alLib::CreateGUIFont();
	m_fontGUI->Load("../data/font.zip", m_gs);
	m_whiteTexture = m_gs->GetWhiteTexture();
	/*m_guiTextures = alCreate<alGSTextureCache>(m_gs);
	m_guiTextureNodes[TextureID_FROMSCRATCH_GREEN] = m_guiTextures->GetTexture("../data/tools/fonttool/fscrg.png");
	m_guiTextureNodes[TextureID_FROMSCRATCH_RED] = m_guiTextures->GetTexture("../data/tools/fonttool/fscrr.png");
	m_guiTextureNodes[TextureID_GENERATE_GREEN] = m_guiTextures->GetTexture("../data/tools/fonttool/geng.png");
	m_guiTextureNodes[TextureID_GENERATE_RED] = m_guiTextures->GetTexture("../data/tools/fonttool/genr.png");
	m_guiTextureNodes[TextureID_MINUS_GREEN] = m_guiTextures->GetTexture("../data/tools/fonttool/minusg.png");
	m_guiTextureNodes[TextureID_MINUS_RED] = m_guiTextures->GetTexture("../data/tools/fonttool/minusr.png");
	m_guiTextureNodes[TextureID_NEW_GREEN] = m_guiTextures->GetTexture("../data/tools/fonttool/newg.png");
	m_guiTextureNodes[TextureID_NEW_RED] = m_guiTextures->GetTexture("../data/tools/fonttool/newr.png");
	m_guiTextureNodes[TextureID_OPEN_GREEN] = m_guiTextures->GetTexture("../data/tools/fonttool/openg.png");
	m_guiTextureNodes[TextureID_OPEN_RED] = m_guiTextures->GetTexture("../data/tools/fonttool/openr.png");
	m_guiTextureNodes[TextureID_PLUS_GREEN] = m_guiTextures->GetTexture("../data/tools/fonttool/plsg.png");
	m_guiTextureNodes[TextureID_PLUS_RED] = m_guiTextures->GetTexture("../data/tools/fonttool/plsr.png");*/
	m_guiContext = alLib::CreateGUIContext(m_mainWindow, m_gs);
	m_guiPanel_first = m_guiContext->GetNewPanel(alVec2f(100,0), alVec2f(500, 500));

	float32_t position = 0.f;
	FontTool_button* btn = new FontTool_button(m_guiContext, alVec2f(0, position), alVec2f(150.f, 32.f));
	btn->SetUserData(this);
	btn->SetID(FontToolGUIID_btnOpen);
	btn->SetFont(m_fontGUI);
	btn->SetText(U"Open");
	m_guiPanel_first->AddElement(btn, true);
	position += 40;

	btn = new FontTool_button(m_guiContext, alVec2f(0, position), alVec2f(150.f, 32.f));
	btn->SetUserData(this);
	btn->SetID(FontToolGUIID_btnGenerate);
	btn->SetText(U"Generate");
	btn->SetFont(m_fontGUI);
	m_guiPanel_first->AddElement(btn, true);
	position += 40;

	btn = new FontTool_button(m_guiContext, alVec2f(0, position), alVec2f(150.f, 32.f));
	btn->SetUserData(this);
	btn->SetID(FontToolGUIID_btnCreate);
	btn->SetText(U"Create");
	btn->SetFont(m_fontGUI);
	m_guiPanel_first->AddElement(btn, true);
	position += 40;
	m_guiPanel_first->Rebuild();

	m_guiPanel_edit = m_guiContext->GetNewPanel(alVec2f(100, 0), alVec2f(500, 500));
	m_comboRanges = new FontTool_combo_unicodeRange(m_guiContext, alVec2f(0, 0), alVec2f(200, 15));
	m_comboRanges->SetUserData(this);
	m_comboRanges->SetFont(m_fontGUI);
	m_comboRanges->m_text.Assign(U"Go To...");
	m_comboRanges->SetItems(g_UnicodeRangeInfo, 56,
		sizeof(UnicodeRangeInfo), 0);
	m_guiPanel_edit->AddElement(m_comboRanges, true);
	m_checkHideUnused = new FontTool_buttonIcon(m_guiContext, m_textureAtlas, iconIndex_iconBase, alVec2f(200.f, 3), alVec2f(14.f, 14.f));
	m_checkHideUnused->SetUserData(this);
	m_checkHideUnused->m_lerpColors = true;
	m_checkHideUnused->m_toggleButton = true;
	m_checkHideUnused->m_iconIndexMouseHover = iconIndex_iconCheck1;
	m_checkHideUnused->m_iconIndexPress = iconIndex_iconCheck2;
	m_checkHideUnused->SetFont(m_fontGUI);
	m_checkHideUnused->m_useText = true;
	m_checkHideUnused->SetID(FontToolGUIID_checkHideUnused);
	m_checkHideUnused->SetText(U"Hide unused");
	m_checkHideUnused->m_textIndent.y = -1;
	m_guiPanel_edit->AddElement(m_checkHideUnused, true);

	m_guiPanel_edit->m_size.x = 400;
	m_guiPanel_edit->Rebuild();
	m_guiPanel_edit->SetVisible(false);
	
	OnRebuild();

	return true;
}

void FontTool::InitCellTexture()
{
	alImage img;
	img.Create(m_fontHeightMax, m_fontHeightMax);
	alGSTextureInfo ti(&img);
	ti.m_cpuAccess = alGSTextureCPUAccess::Write;	
	m_cellTexture = m_gs->CreateTexture(&ti);
}

long mapRange(long value, long in_min, long in_max, long out_min, long out_max) {
	if (in_max == in_min) {
		return out_min; // Avoid division by zero
	}

	// Calculate using long to preserve precision before final division
	return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void FontTool::OnDraw()
{
	alInput* input = alLib::GetInput();
	char32_t char32Buf[100];
	float32_t* dt = alLib::GetDeltaTime();

	m_gs->SetViewport(0, 0, m_mainWindow->m_clientSize.x, m_mainWindow->m_clientSize.y);
	m_gs->BeginDraw();
	m_gs->ClearAll();
	m_gs->EndDraw();
	m_gs->BeginDrawGUI();

	m_guiContext->Draw(*dt);

	if (m_editMode)
	{
		m_gs->SetScissorRect(alVec4f(0.f, 0.f, m_mainWindow->m_clientSize.x, m_mainWindow->m_clientSize.y));
		//m_gs->DrawRectangle(alVec4f(0.f, 0.f, 1000.f, 1000.f), ColorRed);
		uint32_t drawIndex = m_startDrawCellIndex;
		alVec2f drawPosition;
		uint32_t colCounter = 0;
		for (uint32_t i = 0; i < m_visibleCellNum; ++i)
		{
			if (drawIndex >= 0x10FFFF)
				break;

			alColor color = ColorWhite;

			if (drawIndex == m_selected)
				color = ColorRed;

			auto G = m_glyphs[drawIndex];
			

			alVec4f cellRect;
			cellRect.x = drawPosition.x;
			cellRect.y = drawPosition.y;
			cellRect.z = drawPosition.x + m_cellSizeX;
			cellRect.w = drawPosition.y + m_cellSizeY;
			
			alVec4f glyphRect = cellRect;

			alGSTexture* t = m_whiteTexture;
			if (!G.m_data)
			{
				t = m_textureNoData;
			}
			else
			{
				uint32_t rowPitch = 0;
				uint8_t* textureBuffer = (uint8_t*)m_cellTexture->Lock(&rowPitch);

				if (textureBuffer)
				{
					uint8_t* srcBuffer = G.m_data;
					auto ti = m_cellTexture->GetGSTextureInfo();
				//	uint32_t imgPitch = ti->m_width * 4;

					for (uint32_t ty = 0; ty < ti->m_height; ++ty)
					{
						alImage::rgba* dst_rgba = (alImage::rgba*)textureBuffer;
						alImage::rgba* src_rgba = (alImage::rgba*)srcBuffer;
						for (uint32_t tx = 0; tx < ti->m_width; ++tx)
						{
							long src_index = mapRange(tx, 0, ti->m_width, 0, G.m_width);

							dst_rgba->r = src_rgba[src_index].r;
							dst_rgba->g = src_rgba[src_index].g;
							dst_rgba->b = src_rgba[src_index].b;
							dst_rgba->a = 255;


							++dst_rgba;
						}

						textureBuffer += rowPitch;
						srcBuffer += G.m_width * 4;
					}

					m_cellTexture->UnLock();
					t = m_cellTexture;

					float32_t cellW = cellRect.z - cellRect.x;
					if (G.m_width < cellW)
					{
						auto V = G.m_width / cellW;
						glyphRect.z = cellRect.x + (cellW * V);
					}

					float32_t cellH = cellRect.w - cellRect.y;
					if (m_fontHeightMax < cellH)
					{
						auto V = m_fontHeightMax / cellH;
						glyphRect.w = cellRect.y + (cellH * V);
					}
				}
			}
			
			bool draw = true;

			if (m_hideUnUsed && !G.m_data)
				draw = false;

			if (draw)
			{
				m_gs->DrawRectangle(glyphRect, color, t);

				auto str_size = alLib::snprintf(char32Buf, 100, U"U+%.4X", drawIndex);
				m_gs->DrawText(char32Buf, str_size, m_fontGUI,
					drawPosition + alVec2f(0.f, m_cellSizeY - m_fontGUI->m_maxHeight), ColorLime);
				m_gs->DrawRectangle(cellRect, color, m_textureSelect);

				drawPosition.x += m_cellSizeX;

				++colCounter;
				if (colCounter == m_cellsInRow)
				{
					colCounter = 0;
					drawPosition.x = 0;
					drawPosition.y += m_cellSizeY;
				}
			}
			else
			{
				--i;
			}

			++drawIndex;
			if (drawIndex >= 0x10FFFF)
				break;
		}

		auto G_selected = m_glyphs[m_selected];
		if (G_selected.m_data)
		{
			m_gs->DrawRectangle(m_editRect, ColorGrey);
			
			alVec2f position;
			position.x = m_editRect.x;
			position.y = m_editRect.y;

			uint8_t* srcBuffer = G_selected.m_data;
			alImage::rgba* src_rgba = (alImage::rgba*)srcBuffer;

			uint32_t pixelsNum = G_selected.m_width * (int)m_fontHeightMax;
			uint32_t widthCounter = 0;
			for (uint32_t i = 0; i < pixelsNum; ++i)
			{
				alVec4f cellRect;
				cellRect.x = position.x;
				cellRect.y = position.y;
				cellRect.z = cellRect.x + m_editorCellSize;
				cellRect.w = cellRect.y + m_editorCellSize;

				m_gs->DrawRectangle(cellRect, alColor(src_rgba->r, src_rgba->g, src_rgba->b, 255));

				position.x += m_editorCellSize;
				++src_rgba;

				++widthCounter;
				if (widthCounter >= G_selected.m_width)
				{
					widthCounter = 0;
					position.x = m_editRect.x;
					position.y += m_editorCellSize;
				}
			}
		}
	}

	m_gs->EndDrawGUI();
	m_gs->SwapBuffers();
}

void FontTool::OnUpdate()
{
	alInput* input = alLib::GetInput();

	if (alMath::PointInRect(
		input->m_cursorCoordsForGUI.x,
		input->m_cursorCoordsForGUI.y,
		m_cellPanelRect))
	{
		if (input->m_wheelDelta &&
			input->m_kbm != alKeyboardModifier::Ctrl)
		{
			if (input->m_wheelDelta > 0.f)
				_moveUpView(1);
			if (input->m_wheelDelta < 0.f)
				_moveDownView(1);
		}
	}

	// copy from OnDraw
	uint32_t drawIndex = m_startDrawCellIndex;
	alVec2f drawPosition;
	uint32_t colCounter = 0;
	for (uint32_t i = 0; i < m_visibleCellNum; ++i)
	{
		if (drawIndex >= 0x10FFFF)
			break;

		alColor color = ColorWhite;

		if (drawIndex == m_selected)
			color = ColorRed;

		auto G = m_glyphs[drawIndex];
		bool draw = true;

		if (m_hideUnUsed && !G.m_data)
			draw = false;

		if (draw)
		{
			alVec4f cellRect;
			cellRect.x = drawPosition.x;
			cellRect.y = drawPosition.y;
			cellRect.z = drawPosition.x + m_cellSizeX;
			cellRect.w = drawPosition.y + m_cellSizeY;

			if (alMath::PointInRect(
				input->m_cursorCoordsForGUI.x,
				input->m_cursorCoordsForGUI.y,
				cellRect))
			{
				if (input->m_isLMBDown)
				{
					m_selected = drawIndex;
					OnSelect();
				}
			}

			drawPosition.x += m_cellSizeX;

			++colCounter;
			if (colCounter == m_cellsInRow)
			{
				colCounter = 0;
				drawPosition.x = 0;
				drawPosition.y += m_cellSizeY;
			}
		}
		else
		{
			--i;
		}

		++drawIndex;
		if (drawIndex >= 0x10FFFF)
			break;
	}
}

void FontTool::Run()
{
	float32_t* dt = alLib::GetDeltaTime();
	alInput* input = alLib::GetInput();
	char32_t char32Buf[100];

	float timer = 0.f;
	float timer_limit = 1.f / 60.f;

	while (m_run)
	{
		alLib::Update();
		m_guiContext->Update(*dt);

		if (m_editMode)
			OnUpdate();

		timer += *dt;
		if (timer > timer_limit)
		{
			timer = 0.f;
			OnDraw();
		}
	}
}

void FontTool::GoTo(size_t i)
{
	m_startDrawCellIndex = i;
}

//void FontTool::_state_NewOrOpen()
//{
//	m_gs->SetViewport(0, 0, m_mainWindow->m_clientSize.x, m_mainWindow->m_clientSize.y);
//	m_gs->BeginDraw();
//	m_gs->ClearAll();
//	m_gs->EndDraw();
//	m_gs->BeginDrawGUI();
//
//	uint32_t tw = m_guiTextureNodes[TextureID_NEW_RED]->m_texture->GetWidth();
//	uint32_t th = m_guiTextureNodes[TextureID_NEW_RED]->m_texture->GetHeight();
//
//	alVec4f buttonRect;
//	buttonRect.x = 0.f;
//	buttonRect.y = 0.f;
//	buttonRect.z = tw;
//	buttonRect.w = th;
//
//	auto input = alLib::GetInput();
//
//	int textureID = TextureID_NEW_RED;
//	if (alMath::PointInRect(input->m_cursorCoords.x, input->m_cursorCoords.y, buttonRect))
//	{
//		textureID = TextureID_NEW_GREEN;
//
//		if (input->m_isLMBDown)
//		{
//			OnRun = &FontTool::_state_GenerateOrFromScratch;
//		}
//	}
//
//	m_gs->DrawRectangle(buttonRect, ColorWhite, ColorWhite,
//		m_guiTextureNodes[textureID]->m_texture);
//
//
//	tw = m_guiTextureNodes[TextureID_OPEN_RED]->m_texture->GetWidth();
//	th = m_guiTextureNodes[TextureID_OPEN_RED]->m_texture->GetHeight();
//	buttonRect.x = 0.f;
//	buttonRect.y += buttonRect.w + 5.f;
//	buttonRect.z = tw;
//	buttonRect.w = buttonRect.y + th;
//
//	textureID = TextureID_OPEN_RED;
//	if (alMath::PointInRect(input->m_cursorCoords.x, input->m_cursorCoords.y, buttonRect))
//	{
//		textureID = TextureID_OPEN_GREEN;
//
//		if (input->m_isLMBDown)
//		{
//			//OnRun = &FontTool::_state_GenerateOrFromScratch;
//		}
//	}
//	m_gs->DrawRectangle(buttonRect, ColorWhite, ColorWhite,
//		m_guiTextureNodes[textureID]->m_texture);
//
//	m_gs->EndDrawGUI();
//	m_gs->SwapBuffers();
//}
//
//void FontTool::_state_GenerateOrFromScratch()
//{
//	m_gs->SetViewport(0, 0, m_mainWindow->m_clientSize.x, m_mainWindow->m_clientSize.y);
//	m_gs->BeginDraw();
//	m_gs->ClearAll();
//	m_gs->EndDraw();
//	m_gs->BeginDrawGUI();
//
//	int textureID = TextureID_GENERATE_RED;
//	uint32_t tw = m_guiTextureNodes[textureID]->m_texture->GetWidth();
//	uint32_t th = m_guiTextureNodes[textureID]->m_texture->GetHeight();
//
//	alVec4f buttonRect;
//	buttonRect.x = 0.f;
//	buttonRect.y = 0.f;
//	buttonRect.z = tw;
//	buttonRect.w = th;
//
//	auto input = alLib::GetInput();
//
//	if (alMath::PointInRect(input->m_cursorCoords.x, input->m_cursorCoords.y, buttonRect))
//	{
//		textureID = TextureID_GENERATE_GREEN;
//
//		if (input->m_isLMBDown)
//		{
//			OnRun = &FontTool::_state_Generate;
//
//			create_tmp_font();
//		}
//	}
//
//	m_gs->DrawRectangle(buttonRect, ColorWhite, ColorWhite,
//		m_guiTextureNodes[textureID]->m_texture);
//
//	textureID = TextureID_FROMSCRATCH_RED;
//	tw = m_guiTextureNodes[textureID]->m_texture->GetWidth();
//	th = m_guiTextureNodes[textureID]->m_texture->GetHeight();
//	buttonRect.x = 0.f;
//	buttonRect.y += buttonRect.w + 5.f;
//	buttonRect.z = (float32_t)tw;
//	buttonRect.w = buttonRect.y + th;
//
//	if (alMath::PointInRect(input->m_cursorCoords.x, input->m_cursorCoords.y, buttonRect))
//	{
//		textureID = TextureID_FROMSCRATCH_GREEN;
//
//		if (input->m_isLMBDown)
//		{
//			//OnRun = &FontTool::_state_Generate;
//		}
//	}
//	m_gs->DrawRectangle(buttonRect, ColorWhite, ColorWhite,
//		m_guiTextureNodes[textureID]->m_texture);
//
//	m_gs->EndDrawGUI();
//	m_gs->SwapBuffers();
//}
//
//void FontTool::_state_Generate()
//{
//}
//
//void FontTool::_state_Edit()
//{
//}

void FontTool::create_tmp_font()
{
	int fontSize = 10;
	HDC dc = CreateDC(L"DISPLAY", L"DISPLAY", 0, 0);

	bool bold = false;
	bool italic = false;
	bool aa = false;

	HFONT font = CreateFontW(
		-MulDiv(fontSize, GetDeviceCaps(dc, LOGPIXELSY), 72), 0,
		0, 0,
		bold ? FW_BOLD : 0,
		italic, 0, 0,
		ANSI_CHARSET, 0, 0,
		aa ? ANTIALIASED_QUALITY : 0,
		0, L"Consolas");

	SelectObject(dc, font);
	SetTextAlign(dc, TA_LEFT | TA_TOP | TA_NOUPDATECP);

	int size = GetFontUnicodeRanges(dc, 0);
	
	uint8_t* buf = new uint8_t[size];
	LPGLYPHSET glyphs = (LPGLYPHSET)buf;
	GetFontUnicodeRanges(dc, glyphs);

	int maxSizeX = 0;
	int maxSizeY = 0;
	//alImage img;

	//printf("glyphs->cRanges %i\n", glyphs->cRanges);
	for (DWORD range = 0; range < glyphs->cRanges; ++range)
	{
		WCRANGE* current = &glyphs->ranges[range];
		//printf("Range %i, num of glyphs %i\n", range, current->cGlyphs);
		for (int ch = current->wcLow; ch < current->wcLow + current->cGlyphs; ++ch)
		{
			wchar_t currentchar = ch;
			if (IsDBCSLeadByte((BYTE)ch))
				continue;

		//	wprintf(L"Char%i: %c\n", ch, currentchar);

			// get the dimensions
			SIZE size;
			ABC abc;
			GetTextExtentPoint32W(dc, &currentchar, 1, &size);
			
			int underhang = 0;
			int overhang = 0;

			if (GetCharABCWidthsW(dc, currentchar, currentchar, &abc)) // for unicode fonts, get overhang, underhang, width
			{
			//	size.cx = abc.abcB;		// full font width (ignoring padding/underhang )
				underhang = abc.abcA;	// underhang/padding left - can also be negative (in which case it's overhang left)
				overhang = abc.abcC;	// overhang/padding right - can also be negative (in which case it's underhand right)

				if (abc.abcB - abc.abcA + abc.abcC < 1)
					continue; // nothing of width 0
			}
			if (size.cy < 1)
				continue;
			if (size.cx < 1)
				continue;
			
			HBITMAP bmp = CreateCompatibleBitmap(dc, size.cx, size.cy);
			HDC bmpdc = CreateCompatibleDC(dc);
			LOGBRUSH lbrush;
			lbrush.lbColor = RGB(0, 0, 0);
			lbrush.lbHatch = 0;
			lbrush.lbStyle = BS_SOLID;
			HBRUSH brush = CreateBrushIndirect(&lbrush);
			HPEN pen = CreatePen(PS_NULL, 0, 0);
			HGDIOBJ oldbmp = SelectObject(bmpdc, bmp);
			HGDIOBJ oldbmppen = SelectObject(bmpdc, pen);
			HGDIOBJ oldbmpbrush = SelectObject(bmpdc, brush);
			HGDIOBJ oldbmpfont = SelectObject(bmpdc, font);
			SetTextColor(bmpdc, RGB(255, 255, 255));
			Rectangle(bmpdc, 0, 0, size.cx, size.cy);
			SetBkMode(bmpdc, TRANSPARENT);

			

			TextOutW(bmpdc, 
				0,
				0,
				&currentchar,
				1);


			BITMAP b;
			GetObject(bmp, sizeof(BITMAP), (LPSTR)&b);
			WORD cClrBits = (WORD)(b.bmPlanes * b.bmBitsPixel);
			

			PBITMAPINFO pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
				sizeof(BITMAPINFOHEADER));
			pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			pbmi->bmiHeader.biWidth = b.bmWidth;
			pbmi->bmiHeader.biHeight = b.bmHeight;
			pbmi->bmiHeader.biPlanes = b.bmPlanes;
			pbmi->bmiHeader.biBitCount = b.bmBitsPixel;
			pbmi->bmiHeader.biCompression = BI_RGB;
			pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8
				* pbmi->bmiHeader.biHeight;
			pbmi->bmiHeader.biClrImportant = 0;
			LPBYTE lpBits; // memory pointer
			PBITMAPINFOHEADER pbih = (PBITMAPINFOHEADER)pbmi;
			lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);
			GetDIBits(dc, bmp, 0, (WORD)pbih->biHeight, lpBits, pbmi, DIB_RGB_COLORS);
			if (cClrBits <= 8) // we're not supporting these
			{
			}
			else if (cClrBits <= 16)
			{
			}
			else if (cClrBits <= 24)
			{
			}
			else
			{
				alImage* img = alCreate<alImage>();
				img->Create(size.cx, size.cy);

				int rowsize = img->m_pitch;
				uint8_t* dst = img->m_data;
				uint8_t* src = lpBits;
				for (int i = 0; i < pbih->biHeight; ++i)
				{
					memcpy(dst, src, rowsize);

					dst += rowsize;
					src += rowsize;
				}

				uint8_t* m;
				for (m = img->m_data; m < img->m_data+ img->m_dataSize; m += 4)
				{
					if (m[0] > 0) // pixel has colour
					{
					//	printf("%i ", m[0]);

						m[3] = m[0];  // set alpha
						m[0] = m[1] = m[2] = 255; // everything else is full
					}
				}

				//char cbuf[100];
				//sprintf_s(cbuf, 100, "file0x%X.png", (uint32_t)ch);
				//alLib::SaveImage(cbuf, &img, alSaveImageType::png);
				img->FlipVertical();
				/*myglyphs[currentchar] = alCreate<GlyphNode>();
				myglyphs[currentchar]->image = img;
				myglyphs[currentchar]->character = currentchar;
				myglyphs[currentchar]->overhang = overhang;
				myglyphs[currentchar]->underhang = underhang;
				myglyphs[currentchar]->sizeX = size.cx;
				myglyphs[currentchar]->sizeY = size.cy;
				myglyphs[currentchar]->leftTopX = 0;
				myglyphs[currentchar]->leftTopY = 0;
				myglyphs[currentchar]->rightBottomX = 0;
				myglyphs[currentchar]->rightBottomY = 0;*/

				if (size.cx > maxSizeX)
					maxSizeX = size.cx;
				if (size.cy > maxSizeY)
					maxSizeY = size.cy;
			}

			LocalFree(pbmi);
			GlobalFree(lpBits);
			DeleteDC(bmpdc);
			DeleteObject(brush);
			DeleteObject(pen);
			DeleteObject(bmp);
		}
	}
	
	int imageIndex = 0;
	alImage img;
	img.Create(512, 512);
	int drawPositionX = 0;
	int drawPositionY = 0;

	alUnicodeString ustr;

	alArray<alStringA> zipfiles;

	for (int i = 0; i < 0x10FFFF; ++i)
	{
		//if (myglyphs[i])
		//{
		//	myglyphs[i]->textureID = imageIndex;

		//	int rbX = drawPositionX + myglyphs[i]->sizeX;
		//	int rbY = drawPositionY + myglyphs[i]->sizeY;

		//	if (rbX > 512)
		//	{
		//		drawPositionX = 0;
		//		rbX = drawPositionX + myglyphs[i]->sizeX;

		//		drawPositionY = rbY;
		//		rbY = drawPositionY + myglyphs[i]->sizeY;

		//		if (rbY > 512)
		//		{
		//			char buf[100];
		//			sprintf_s(buf, 100, "font%i.png", imageIndex);
		//			alLib::SaveImage(buf, &img, alSaveImageType::png);
		//			
		//			zipfiles.push_back(buf);

		//			imageIndex++;

		//			drawPositionY = 0;
		//			rbY = drawPositionY + myglyphs[i]->sizeY;

		//			img.Fill(alColor(0.f,0.f,0.f,0.f));
		//		}
		//	}

		//	img.Fill(myglyphs[i]->image, alVec2u(drawPositionX, drawPositionY), 0, 0);

		//	// write code, not char
		//	ustr.Append((uint32_t)myglyphs[i]->character);

		//	ustr.Append(" ");
		//	ustr.Append(drawPositionX);
		//	ustr.Append(" ");
		//	ustr.Append(drawPositionY);
		//	ustr.Append(" ");
		//	ustr.Append(drawPositionX + myglyphs[i]->sizeX);
		//	ustr.Append(" ");
		//	ustr.Append(drawPositionY + myglyphs[i]->sizeY);
		//	ustr.Append(" ");
		//	ustr.Append(myglyphs[i]->underhang);
		//	ustr.Append(" ");
		//	ustr.Append(myglyphs[i]->overhang);
		//	ustr.Append(" ");
		//	ustr.Append(myglyphs[i]->textureID);
		//	ustr.Append("\n");

		//	drawPositionX = rbX;
		//}
	}

	if (imageIndex == 0)
	{
		char buf[100];
		sprintf_s(buf, 100, "font%i.png", imageIndex);
		alLib::SaveImage(buf, &img, alSaveImageType::png);
		zipfiles.push_back(buf);
	}

	{
		char buf[100];
		sprintf_s(buf, 100, "font.txt");
		ustr.SaveToFileUTF32(buf);
		zipfiles.push_back(buf);
	}

	if(zipfiles.m_size)
	{
		alArray<alFileBuffer*> fbs;
		alArchiveFileZipFileData* files = new alArchiveFileZipFileData[zipfiles.m_size];
		
		for (size_t i = 0; i < zipfiles.m_size; ++i)
		{
			sprintf_s(files[i].m_fileName, 0xff, zipfiles.m_data[i].c_str());
			
			alFileBuffer* fb = new alFileBuffer;
			fbs.push_back(fb);
			fb->ReadFile(files[i].m_fileName);
			files[i].m_data = fb->Data();
			files[i].m_dataSize = fb->Size();
		}

		alArchive::SaveZip(files, zipfiles.m_size, "font.zip");

		for (size_t i = 0; i < zipfiles.m_size; ++i)
		{
			alLib::RemoveFile(files[i].m_fileName);
		}

		for (size_t i = 0; i < fbs.m_size; ++i)
		{
			delete fbs.m_data[i];
		}
		delete[] files;
	}
}

void FontTool::OnButtonOpen()
{
	StartEdit();
}

void FontTool::OnButtonCreate()
{
	StartEdit();
}


void FontTool::OnButtonGenerate()
{
	LOGFONT lf = { 0 };
	lf.lfHeight = 12; // Default height
	lf.lfWeight = FW_NORMAL;
	// Initialize LOGFONT string buffer
	lf.lfFaceName[0] = '\0';

	alSystemWindowOSDataWin32* w32 = (alSystemWindowOSDataWin32*)m_mainWindow->GetOSData();

	CHOOSEFONT cf = { 0 };
	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.hwndOwner = w32->m_hwnd;
	cf.lpLogFont = &lf;
	cf.Flags = CF_EFFECTS | CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;
	cf.rgbColors = RGB(0, 0, 0); // Default text color

	if (ChooseFontW(&cf) == TRUE && (lf.lfFaceName[0]))
	{
		HFONT font = CreateFontIndirect(&lf);
		if (font)
		{
			HDC dc = CreateDC(L"DISPLAY", L"DISPLAY", 0, 0);
			SelectObject(dc, font);
			SetTextAlign(dc, TA_LEFT | TA_TOP | TA_NOUPDATECP);
			int size = GetFontUnicodeRanges(dc, 0);
			uint8_t* buf = new uint8_t[size];
			LPGLYPHSET glyphs = (LPGLYPHSET)buf;
			GetFontUnicodeRanges(dc, glyphs);
			
			int maxSizeX = 0;
			int maxSizeY = 0;

			for (DWORD range = 0; range < glyphs->cRanges; ++range)
			{
				WCRANGE* current = &glyphs->ranges[range];
				for (int ch = current->wcLow; ch < current->wcLow + current->cGlyphs; ++ch)
				{
					if (ch >= 0x10FFFF)
						continue;

					wchar_t currentchar = ch;
					if (IsDBCSLeadByte((BYTE)ch))
						continue;
					SIZE size;
					ABC abc;
					GetTextExtentPoint32W(dc, &currentchar, 1, &size);
					int underhang = 0;
					int overhang = 0;

					if (GetCharABCWidthsW(dc, currentchar, currentchar, &abc)) // for unicode fonts, get overhang, underhang, width
					{
						size.cx = abc.abcB;		// full font width (ignoring padding/underhang )
						underhang = abc.abcA;	// underhang/padding left - can also be negative (in which case it's overhang left)
						overhang = abc.abcC;	// overhang/padding right - can also be negative (in which case it's underhand right)
		//				printf("[%i][%i][%i] : [%i]\n", abc.abcA, abc.abcB, abc.abcC, abc.abcB - abc.abcA + abc.abcC);
						if (abc.abcB - abc.abcA + abc.abcC < 1)
							continue; // nothing of width 0
					}
					if (size.cy < 1)
						continue;
					if (size.cx < 1)
						continue;

					HBITMAP bmp = CreateCompatibleBitmap(dc, size.cx, size.cy);
					HDC bmpdc = CreateCompatibleDC(dc);
					LOGBRUSH lbrush;
					lbrush.lbColor = RGB(0, 0, 0);
					lbrush.lbHatch = 0;
					lbrush.lbStyle = BS_SOLID;
					HBRUSH brush = CreateBrushIndirect(&lbrush);
					HPEN pen = CreatePen(PS_NULL, 0, 0);
					HGDIOBJ oldbmp = SelectObject(bmpdc, bmp);
					HGDIOBJ oldbmppen = SelectObject(bmpdc, pen);
					HGDIOBJ oldbmpbrush = SelectObject(bmpdc, brush);
					HGDIOBJ oldbmpfont = SelectObject(bmpdc, font);
					SetTextColor(bmpdc, RGB(255, 255, 255));
					Rectangle(bmpdc, 0, 0, size.cx, size.cy);
					SetBkMode(bmpdc, TRANSPARENT);

					TextOutW(bmpdc,
						-underhang,
						0,
						&currentchar,
						1);


					BITMAP b;
					GetObject(bmp, sizeof(BITMAP), (LPSTR)&b);
					WORD cClrBits = (WORD)(b.bmPlanes * b.bmBitsPixel);


					PBITMAPINFO pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
						sizeof(BITMAPINFOHEADER));
					pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
					pbmi->bmiHeader.biWidth = b.bmWidth;
					pbmi->bmiHeader.biHeight = b.bmHeight;
					pbmi->bmiHeader.biPlanes = b.bmPlanes;
					pbmi->bmiHeader.biBitCount = b.bmBitsPixel;
					pbmi->bmiHeader.biCompression = BI_RGB;
					pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8
						* pbmi->bmiHeader.biHeight;
					pbmi->bmiHeader.biClrImportant = 0;
					LPBYTE lpBits; // memory pointer
					PBITMAPINFOHEADER pbih = (PBITMAPINFOHEADER)pbmi;
					lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);
					GetDIBits(dc, bmp, 0, (WORD)pbih->biHeight, lpBits, pbmi, DIB_RGB_COLORS);
					if (cClrBits <= 8) // we're not supporting these
					{
					}
					else if (cClrBits <= 16)
					{
					}
					else if (cClrBits <= 24)
					{
					}
					else
					{
						//alImage* img = alCreate<alImage>();
						//img->Create(size.cx, size.cy);
						GlyphInfo* g = &m_glyphs[ch];
						if (!g->m_data)
						{
							g->m_data = (uint8_t*)alMemory::Malloc(size.cx * size.cy * 4);
							g->m_width = size.cx;

							int rowsize = size.cx * 4; // width * rgba

							uint8_t* dst = g->m_data;
							uint8_t* src = lpBits + (rowsize * size.cy) - rowsize; // I will copy rows from bottom to top
							alImage::rgba* src_rgba = (alImage::rgba*)(src);
							alImage::rgba* dst_rgba = (alImage::rgba*)(dst);


							/*alImage iimg;
							iimg.Create(size.cx, size.cy);
							iimg.Fill(ColorBlack);
							alImage::rgba* iimg_rgba = (alImage::rgba*)iimg.m_data;*/

							for (int i = 0; i < pbih->biHeight; ++i)
							{
								//memcpy(dst, src, rowsize);
								for (int o = 0; o < size.cx; ++o)
								{
									/*iimg_rgba->a = src_rgba[o].r;
									iimg_rgba->r = src_rgba[o].r;
									iimg_rgba->g = src_rgba[o].r;
									iimg_rgba->b = src_rgba[o].r;
									++iimg_rgba;*/

									dst_rgba->a = src_rgba[o].r;
									dst_rgba->r = src_rgba[o].r;
									dst_rgba->g = src_rgba[o].r;
									dst_rgba->b = src_rgba[o].r;
									
									dst += 4;
									dst_rgba = (alImage::rgba*)(dst);
								}

								src -= rowsize;
								src_rgba = (alImage::rgba*)src;
							}

							/*static int fni = 0;
							char fn[100];
							sprintf_s(fn,100, "%i.png", fni);
							alLib::SaveImage(fn, &iimg, alSaveImageType::png);
							++fni;*/

							if (size.cx > maxSizeX)
								maxSizeX = size.cx;
							if (size.cy > maxSizeY)
								maxSizeY = size.cy;
						}
					}

					LocalFree(pbmi);
					GlobalFree(lpBits);
					DeleteDC(bmpdc);
					DeleteObject(brush);
					DeleteObject(pen);
					DeleteObject(bmp);
				}
			}
			
			m_fontWidthMax = maxSizeX;
			m_fontHeightMax = maxSizeY;
		//	printf("m_fontWidthMax %f\n", m_fontWidthMax);
			InitCellTexture();

			if (buf)
				delete[]buf;
			DeleteObject(font);
		}

		StartEdit();
	}
}

void FontTool::OnRebuild()
{
	if (m_gs)
	{
		m_gs->UpdateWindowData();
	}


	auto windowSzY = m_mainWindow->m_clientSize.y;
	if (!windowSzY)
		windowSzY = 1;
	m_visibleLineNum = (uint32_t)ceilf(float(windowSzY) / float(m_cellSizeY));
	m_visibleCellNum = m_visibleLineNum * m_cellsInRow;

	m_cellPanelWidth = float(m_cellsInRow) * float(m_cellSizeY);

	m_cellPanelRect.z = m_cellPanelWidth;
	m_cellPanelRect.w = m_mainWindow->m_clientSize.y;

	if (m_guiPanel_edit)
	{
		m_guiPanel_edit->m_size.x = m_mainWindow->m_clientSize.x - m_cellPanelWidth;
		m_guiPanel_edit->m_size.y = m_mainWindow->m_clientSize.y;
		m_guiPanel_edit->m_position.x = m_mainWindow->m_clientSize.x - m_guiPanel_edit->m_size.x;
		m_guiPanel_edit->m_position.y = 0;
		m_guiPanel_edit->Rebuild();
	}

	m_editRect.x = m_cellPanelWidth;
	m_editRect.y = 30;
	m_editRect.z = m_mainWindow->m_clientSize.x;
	m_editRect.w = m_editRect.y + 300;
}

void FontTool::StartEdit()
{
	m_guiPanel_first->SetVisible(false);
	m_guiPanel_edit->SetVisible(true);
	m_editMode = true;
}

void FontTool::_moveUpView(uint32_t num)
{
	if (m_hideUnUsed)
	{
		while (true)
		{
			if (m_startDrawCellIndex <= m_cellsInRow)
				break;

			m_startDrawCellIndex -= m_cellsInRow;

			if (!m_startDrawCellIndex)
				break;

			auto index = m_startDrawCellIndex;
			for (int i = 0; i < m_cellsInRow; ++i)
			{
				if (m_glyphs[index].m_data)
					return;
				++index;
			}
		}
	}
	else
	{
		if (m_startDrawCellIndex)
		{
			for (uint32_t i = 0; i < num; ++i)
			{
				m_startDrawCellIndex -= m_cellsInRow;

				if (!m_startDrawCellIndex)
					break;
			}
		}
	}
}

void FontTool::_moveDownView(uint32_t num)
{
	if (m_hideUnUsed)
	{
		while (true)
		{
			m_startDrawCellIndex += m_cellsInRow;
			if (m_startDrawCellIndex >= 0x10FFFF)
				break;

			auto index = m_startDrawCellIndex;
			for (int i = 0; i < m_cellsInRow; ++i)
			{
				if (m_glyphs[index].m_data)
					return;
				++index;
			}
		}
	}
	else
	{
		for (uint32_t i = 0; i < num; ++i)
		{
			m_startDrawCellIndex += m_cellsInRow;
			if (m_startDrawCellIndex >= 0x10FFFF)
				break;
		}
	}
}

void FontTool::OnSelect()
{
	m_editorCellSize = 0.f;
	auto G_selected = m_glyphs[m_selected];
	if (G_selected.m_data)
	{
		m_editorCellSize = 20.f;
		float32_t width = G_selected.m_width * m_editorCellSize;

		auto editRectWidth = m_editRect.z - m_editRect.x;
		if (editRectWidth <= 0.f) editRectWidth = 1.f;

		if (width > editRectWidth)
		{
			m_editorCellSize *= editRectWidth / width;
			m_editorCellSize = G_selected.m_width * m_editorCellSize;
		}

		float32_t height = m_fontHeightMax * m_editorCellSize;
		auto editRectHeight = m_editRect.w - m_editRect.y;
		if (editRectHeight <= 0.f) editRectHeight = 1.f;
		if (height > editRectHeight)
		{
			m_editorCellSize *= editRectHeight / height;
			//m_editorCellSize = m_fontHeightMax * m_editorCellSize;
		}
	}
}

int main()
{
	alLib::InitializeLib();

	FontTool* dd = new FontTool;
	if (dd->Init())
	{
		dd->Run();
	}

	return 1;
}


