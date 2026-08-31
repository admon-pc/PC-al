#include <windows.h>

#include "al.h"
#include "Image/alImage.h"

#include "System/alSystemWindow.h"
#include "System/alSystemWindowWin32.h"
#include "System/alSystemPopup.h"
#include "GS/alGS.h"
#include "Input/alInput.h"
#include "Classes/alColor.h"

#include "GUI/alGUI.h"

#include "Archive/alArchive.h"
#include "Containers/alArray.h"

#include <filesystem>

#ifdef LoadImage
#undef LoadImage
#endif


AL_LINK_LIBRARY(al);

#define FontToolClipboardData_MAGIC 0xBBAABBCC
struct FontToolClipboardData_t
{
	enum
	{
		type_rgba8,
		type_cell,
	};

	uint32_t m_magic = 0;
	uint32_t m_type = type_rgba8;
	uint32_t m_glyphDataSize = 0;
	uint32_t m_glyphWidth = 0;
	uint32_t m_fontHeight = 0;
	int m_overhang = 0;
	int m_underhang = 0;
};

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
#define FontToolGUIID_btnSave 5
#define FontToolGUIID_btnSave_Cancel 6
#define FontToolGUIID_btnSave_Save 7
#define FontToolGUIID_comboRanges 8
#define FontToolGUIID_comboSave_ImageSizes 9
#define FontToolGUIID_textSave_NumImages 10
#define FontToolGUIID_popupCell_Create 11
#define FontToolGUIID_popupCell_Delete 12
#define FontToolGUIID_popupCell_Copy 13
#define FontToolGUIID_popupCell_Paste 14
#define FontToolGUIID_popupEdit_Copy 15
#define FontToolGUIID_popupEdit_Paste 16
#define FontToolGUIID_popupEdit_Clear 17
#define FontToolGUIID_btnSetWidth 18
#define FontToolGUIID_btnSetWidth_Close 19
#define FontToolGUIID_rangeSetWidth 20

#define FontToolEventID_popupCell 1

class FontTool_Text : public alGUIText
{
public:
	FontTool_Text(alGUIContext* ct, const alVec2f& position, const alVec2f& size) :
		alGUIText(ct, position, size)
	{}
	virtual ~FontTool_Text() {}
};

class FontTool_TextInput : public alGUITextInput
{
public:
	FontTool_TextInput(alGUIContext* ct, const alVec2f& position, const alVec2f& size) :
		alGUITextInput(ct, position, size)
	{}
	virtual ~FontTool_TextInput() {}

	virtual void OnRMBRelease() override;
	virtual void OnAccept() override;
	virtual void OnCancel() override;
};

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
#include "ranges.inl"
struct GlyphName
{
	char32_t m_title[100];
};
#include "names.inl"

struct ImageSizesInfo
{
	char32_t m_title[10];
};
ImageSizesInfo imageSizes[] =
{
	{U"256"},
	{U"512"},
	{U"1024"},
	{U"2048"},
	{U"4096"},
};

struct GlyphInfo
{
	int m_width = 0;

	// rgba
	uint8_t* m_data = 0;

	uint32_t textureID = 0;
	
	int overhang = 0;
	int underhang = 0;

	/*alImage* image = 0; //61 для равно
	
	int textureID = 0;

	
	

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

class FontTool_slider : public alGUIRangeSlider1
{
public:
	FontTool_slider(alGUIContext* ct, const alVec2f& position, const alVec2f& size)
		:
		alGUIRangeSlider1(ct, position, size) {}
	virtual ~FontTool_slider() {}
	AL_DECLARE_DEFAULT_ALLOCATOR(FontTool_slider);

	virtual void OnSliderValueChanged() override;
};



class SystemWindowCallback;
class FontTool
{
	bool m_pollEventAtTheEndOfFrame = false;
	void _pollEvents();
	//void _state_NewOrOpen();
	//void _state_GenerateOrFromScratch();
	//void _state_Generate();
	//void _state_Edit();

//	void (FontTool::*OnRun)();

	alGUIContext* m_guiContext = 0;
	alGUIPanel* m_guiPanel_first = 0;
	alGUIPanel* m_guiPanel_edit = 0;
	alGUIPanel* m_guiPanel_edit_base = 0;
	alGUIPanel* m_guiPanel_edit_setWidth = 0;
	alGUIPanel* m_guiPanel_save = 0;
	alGUITextureAtlas* m_textureAtlas = 0;
	alGSTexture* m_textureAtlasTexture = 0;


	FontTool_combo_unicodeRange* m_comboRanges = 0;
	FontTool_buttonIcon* m_checkHideUnused = 0;
	FontTool_TextInput* m_textInput_editor_oneLine = 0;

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
	void ShowPopupOnCell();

	void ShowPopupOnEditRect();

	alGUIFont* m_testFont = 0;
	void UpdateTestFont();

public:
	FontTool();
	~FontTool();

	bool Init();
	void Run();
	void OnPopupCommand(uint32_t cmd);

	void DeleteCell(uint32_t);
	void CreateCell(uint32_t);
	void CopyCellToClipboard(uint32_t);
	void PasteCellFromClipboard(uint32_t);
	void CopyImageToClipboard(uint32_t);
	void PasteImageFromClipboard(uint32_t);
	void ClearImage(uint32_t);

	void StartEdit();

	void OnButtonGenerate();
	void OnButtonOpen();
	void OnButtonCreate();
	void OnButtonSave();
	void OnButtonSave_Save();
	void OnButtonSave_Cancel();
	
	void OnComboSaveSize(uint32_t);
	void OnButtonEnableSetWidthMode();
	void OnButtonDisableSetWidthMode();
	void OnSliderSetWidth();

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
	int32_t m_fontHeightMax = 10;
	float m_fontWidthMax = 0.f;

	alGSTexture* m_whiteTexture = 0;
	alGUIFont* m_fontGUI = 0;
	alGUIFont* m_fontDefault = 0;

	void InitCellTexture();
	alGSTexture* m_cellTexture = 0;

	alImage m_glyphImageOriginal;
	void _saveGlyphImageOriginal();

	void GoTo(size_t);
	//alGSTextureCacheNode* m_guiTextureNodes[TextureID__end];
	//alGSTextureCache* m_guiTextures = 0;
	uint32_t m_saveImageSize = 512;
};

void FontTool_combo_unicodeRange::OnComboSelectItem(size_t index)
{
	switch (GetID())
	{
	case FontToolGUIID_comboRanges:
	{
		UnicodeRangeInfo* ptrInfo = (UnicodeRangeInfo*)m_items;
		uint8_t* ptr = (uint8_t*)m_items;
		m_text = (char32_t*)(&ptr[index * m_stride] + m_textOffset);
		FontTool* app = (FontTool*)GetUserData();
		app->GoTo(ptrInfo[index].m_index);
	}break;
	case FontToolGUIID_comboSave_ImageSizes:
	{
		FontTool* app = (FontTool*)GetUserData();
		app->OnComboSaveSize(index);
		uint8_t* ptr = (uint8_t*)m_items;
		m_text = (char32_t*)(&ptr[index * m_stride] + m_textOffset);
	}break;
	}

	
}
void FontTool_button::OnButtonRelease()
{
	FontTool* app = (FontTool*)GetUserData();

	switch (GetID())
	{
	case FontToolGUIID_btnOpen:
		app->OnButtonOpen();
		break;
	case FontToolGUIID_btnGenerate:
		app->OnButtonGenerate();
		break;
	case FontToolGUIID_btnCreate:
		app->OnButtonCreate();
		break;
	case FontToolGUIID_btnSave:
		app->OnButtonSave();
		break;
	case FontToolGUIID_btnSave_Cancel:
		app->OnButtonSave_Cancel();
		break;
	case FontToolGUIID_btnSave_Save:
		app->OnButtonSave_Save();
		break;
	case FontToolGUIID_btnSetWidth:
		app->OnButtonEnableSetWidthMode();
		break;
	case FontToolGUIID_btnSetWidth_Close:
		app->OnButtonDisableSetWidthMode();
		break;
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

void FontTool_TextInput::OnRMBRelease()
{
}

void FontTool_TextInput::OnAccept()
{
}

void FontTool_TextInput::OnCancel()
{
}

void FontTool_slider::OnSliderValueChanged()
{
	FontTool* app = (FontTool*)GetUserData();
	switch (GetID())
	{
	case FontToolGUIID_rangeSetWidth:
		app->OnSliderSetWidth();
		break;
	}
}

class SystemWindowCallback : public alSystemWindowCallback
{
	FontTool* m_app = 0;
public:
	SystemWindowCallback(FontTool* dd) : m_app(dd) {}
	virtual ~SystemWindowCallback() {}

	virtual void OnSizeChanged(alSystemWindow*)
	{
		m_app->OnRebuild();
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
			m_app->m_run = false;
		}
	}
	virtual void OnPopupCommand(uint32_t cmd)
	{
		m_app->OnPopupCommand(cmd);
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
	AL_DESTROY(m_testFont);
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

	auto text = new FontTool_Text(m_guiContext, alVec2f(160, position), alVec2f(50, 15));
	text->SetFont(m_fontGUI);
	text->SetText(U"Height");
	m_guiPanel_first->AddElement(text, true);

	FontTool_slider* slider = new FontTool_slider(m_guiContext, alVec2f(160, position + 15), alVec2f(150.f, 13.f));
	slider->SetUserData(this);
	slider->m_minMax_i[0] = 6;
	slider->m_minMax_i[1] = 64;
	slider->m_ptr_i = &m_fontHeightMax;
	slider->m_type = alGUIRangeSlider1::Type::type_IntLimits;
	slider->m_colorTheme->m_slider_bg = 0xFFBB22FF;
	m_guiPanel_first->AddElement(slider, true);

	position += 40;
	m_guiPanel_first->Rebuild();

	m_guiPanel_edit = m_guiContext->GetNewPanel(alVec2f(100, 0), alVec2f(500, 500));
	m_guiPanel_edit->m_drawBG = false;
	m_comboRanges = new FontTool_combo_unicodeRange(m_guiContext, alVec2f(0, 0), alVec2f(200, 15));
	m_comboRanges->SetID(FontToolGUIID_comboRanges);
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

	btn = new FontTool_button(m_guiContext, alVec2f(0, 0), alVec2f(50.f, 32.f));
	btn->SetUserData(this);
	btn->SetID(FontToolGUIID_btnSave);
	btn->SetText(U"Save");
	btn->SetFont(m_fontGUI);
	btn->m_alignment = alGUIElementAlignment::RightTop;
	m_guiPanel_edit->AddElement(btn, true);

	m_textInput_editor_oneLine = alCreate<FontTool_TextInput>(m_guiContext, alVec2f(610.f, 5.f), alVec2f(140, 18));
	m_textInput_editor_oneLine->SetUserData(this);
	m_textInput_editor_oneLine->SetFont(m_fontGUI, 0);
	m_textInput_editor_oneLine->SetText(U"Demonstration text");
	m_textInput_editor_oneLine->m_oneLine = true;
	m_textInput_editor_oneLine->m_useBottombar = false;
	m_textInput_editor_oneLine->m_useHorizontalScrollbar = false;
	m_textInput_editor_oneLine->m_useLinebar = false;
	m_textInput_editor_oneLine->m_useVerticalScrollbar = false;
	m_guiPanel_edit->AddElement(m_textInput_editor_oneLine, true);


	m_guiPanel_edit_base = alCreate<alGUIPanel>(m_guiContext, 
		alVec2f(0, 350), 
		alVec2f(300.f, 200.f));
	m_guiPanel_edit->AddElement(m_guiPanel_edit_base, true);
	btn = new FontTool_button(m_guiContext, alVec2f(0, 0), alVec2f(110.f, 20.f));
	btn->SetUserData(this);
	btn->SetID(FontToolGUIID_btnSetWidth);
	btn->SetText(U"Set Width");
	btn->SetFont(m_fontGUI);
	m_guiPanel_edit_base->AddElement(btn, true);
	m_guiPanel_edit_base->Rebuild();

	m_guiPanel_edit_setWidth = alCreate<alGUIPanel>(m_guiContext,
		alVec2f(0, 350),
		alVec2f(300.f, 200.f));
	m_guiPanel_edit->AddElement(m_guiPanel_edit_setWidth, true);
	btn = new FontTool_button(m_guiContext, alVec2f(0, 0), alVec2f(30.f, 20.f));
	btn->SetUserData(this);
	btn->SetID(FontToolGUIID_btnSetWidth_Close);
	btn->SetText(U"OK");
	btn->SetFont(m_fontGUI);
	btn->m_alignment = alGUIElementAlignment::RightTop;
	m_guiPanel_edit_setWidth->AddElement(btn, true);
	slider = new FontTool_slider(m_guiContext, alVec2f(160, position + 15), alVec2f(150.f, 13.f));
	slider->SetID(FontToolGUIID_rangeSetWidth);
	slider->SetUserData(this);
	slider->m_minMax_i[0] = 1;
	slider->m_minMax_i[1] = 100;
	slider->m_ptr_i = 0;
	slider->m_type = alGUIRangeSlider1::Type::type_IntLimits;
	slider->m_colorTheme->m_slider_bg = 0xFFBB22FF;
	m_guiPanel_edit_setWidth->AddElement(slider, true);
	m_guiPanel_edit_setWidth->Rebuild();
	m_guiPanel_edit_setWidth->SetVisible(false);

	m_guiPanel_edit->m_size.x = 400;
	m_guiPanel_edit->Rebuild();
	m_guiPanel_edit->SetVisible(false);
	
	m_guiPanel_save = m_guiContext->GetNewPanel(alVec2f(100, 0), alVec2f(500, 500));
	m_guiPanel_save->m_drawBG = false;
	btn = new FontTool_button(m_guiContext, alVec2f(0, 0), alVec2f(50.f, 32.f));
	btn->SetUserData(this);
	btn->SetID(FontToolGUIID_btnSave_Cancel);
	btn->SetText(U"Cancel");
	btn->SetFont(m_fontGUI);
	m_guiPanel_save->AddElement(btn, true);
	auto combo = new FontTool_combo_unicodeRange(m_guiContext, alVec2f(0, 50), alVec2f(50, 15));
	combo->SetID(FontToolGUIID_comboSave_ImageSizes);
	combo->SetUserData(this);
	combo->SetFont(m_fontGUI);
	combo->m_text.Assign(U"512");
	combo->SetItems(&imageSizes, 5,
		sizeof(ImageSizesInfo), 0);
	m_guiPanel_save->AddElement(combo, true);
	text = new FontTool_Text(m_guiContext, alVec2f(50, 50), alVec2f(50, 15));
	text->SetText(U"Texture Size");
	text->SetFont(m_fontGUI);
	text->SetID(FontToolGUIID_textSave_NumImages);
	m_guiPanel_save->AddElement(text, true);
	btn = new FontTool_button(m_guiContext, alVec2f(0, 150), alVec2f(50.f, 32.f));
	btn->SetUserData(this);
	btn->SetID(FontToolGUIID_btnSave_Save);
	btn->SetText(U"Save");
	btn->SetFont(m_fontGUI);
	m_guiPanel_save->AddElement(btn, true);
	m_guiPanel_save->Rebuild();
	m_guiPanel_save->SetVisible(false);

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

	m_gs->SetScissorRect(alVec4f(0.f, 0.f, m_mainWindow->m_clientSize.x, m_mainWindow->m_clientSize.y));
	
	if (m_guiPanel_edit->m_visible)
	{
		auto G_selected = m_glyphs[m_selected];
		auto str_size = alLib::snprintf(char32Buf, 100, U"U+%.4X", m_selected);
		m_gs->DrawText(char32Buf, str_size, m_fontGUI,
			alVec2f(m_guiPanel_edit->m_buildArea.x+1, 12), ColorWhite);

		if (m_selected <= 127)
		{
			m_gs->DrawText(
				g_GlyphNames[m_selected].m_title,
				alLib::strlen(g_GlyphNames[m_selected].m_title),
				m_fontGUI,
				alVec2f(m_guiPanel_edit->m_buildArea.x + 50, 12),
				ColorWhite);
		}
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

				m_gs->DrawRectangle(cellRect, ColorBlack);
				m_gs->DrawRectangle(cellRect, alColor(src_rgba->r, src_rgba->g, src_rgba->b, src_rgba->a));

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
			str_size = alLib::snprintf(char32Buf, 100, U"Width: %u, Font Height: %u", G_selected.m_width, m_fontHeightMax);
			m_gs->DrawText(char32Buf, str_size, m_fontGUI,
				alVec2f(m_editRect.x, m_editRect.w), ColorWhite);
		}


		if(m_testFont)
			m_gs->DrawText(
				m_textInput_editor_oneLine->m_text.c_str(),
				m_textInput_editor_oneLine->m_text.size(),
				m_testFont,
				alVec2f(m_editRect.x, m_textInput_editor_oneLine->m_buildArea.y - m_testFont->m_maxHeight),
				ColorWhite);
	}
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
				else if (input->m_isRMBDown)
				{
					m_selected = drawIndex;
					OnSelect();
					alEvent event;
					event.m_type = alEventType::User;
					event.m_event_user.m_id = FontToolEventID_popupCell;
					alLib::AddEvent(event, true);
					m_pollEventAtTheEndOfFrame = true;
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

	if (alMath::PointInRect(
		input->m_cursorCoordsForGUI.x,
		input->m_cursorCoordsForGUI.y,
		m_editRect))
	{
		if (input->m_isRMBDown)
			ShowPopupOnEditRect();
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
		if(!m_pollEventAtTheEndOfFrame)
			_pollEvents();

		alLib::Update();
		m_guiContext->Update(*dt);

		auto m_currentCursor = m_guiContext->m_cursorType;
		alLib::SetCursor(m_currentCursor, alLib::GetCursor(m_currentCursor));

		if (m_editMode)
			OnUpdate();

		timer += *dt;
		if (timer > timer_limit)
		{
			timer = 0.f;
			OnDraw();
		}

		if (m_pollEventAtTheEndOfFrame)
		{
			// I don't know why it can't draw using 1 call
			OnDraw();
			OnDraw();

			_pollEvents();
			m_pollEventAtTheEndOfFrame = false;
		}
	}
}

void FontTool::GoTo(size_t i)
{
	m_startDrawCellIndex = i;
}

void FontTool::OnButtonOpen()
{
	StartEdit();
}

void FontTool::OnButtonCreate()
{
	InitCellTexture();
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
							g->underhang = abc.abcA;
							g->overhang = abc.abcC;

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

									auto color = src_rgba[o].r;
									if (color)
										color = 255;

									dst_rgba->a = src_rgba[o].r;
									dst_rgba->r = color;
									dst_rgba->g = color;
									dst_rgba->b = color;
									
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

	UpdateTestFont();
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
		m_textInput_editor_oneLine->m_position.Set(
			0,
			m_mainWindow->m_clientSize.y - m_fontGUI->m_maxHeight);
		m_textInput_editor_oneLine->m_size.Set(
			m_mainWindow->m_clientSize.x - m_cellPanelWidth,
			m_fontGUI->m_maxHeight);
		//m_textInput_editor_oneLine->Rebuild();
		

		m_guiPanel_edit->m_size.x = m_mainWindow->m_clientSize.x - m_cellPanelWidth;
		m_guiPanel_edit->m_size.y = m_mainWindow->m_clientSize.y;
		m_guiPanel_edit->m_position.x = m_mainWindow->m_clientSize.x - m_guiPanel_edit->m_size.x;
		m_guiPanel_edit->m_position.y = 0;
		/*FontTool_button* btn = dynamic_cast<FontTool_button*>(m_guiPanel_edit->GetElementByID(FontToolGUIID_btnSave));
		if (btn)
		{
			btn->m_position.x = 
		}*/
		m_guiPanel_edit->Rebuild();

		m_guiPanel_save->m_size = m_guiPanel_edit->m_size;
		m_guiPanel_save->m_position = m_guiPanel_edit->m_position;
		m_guiPanel_save->Rebuild();
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

void FontTool::UpdateTestFont()
{
	AL_DESTROY(m_testFont);
	m_testFont = alLib::CreateGUIFont();
	static int imageIndex = 0;

	alImage img;
	img.Create(512, 512);
	int drawPositionX = 0;
	int drawPositionY = 0;
	uint32_t textureID = 0;
	
	bool hasData = false;
	alVec4f uv;
	for (int i = 0; i < 0x10FFFF; ++i)
	{
		auto G = m_glyphs[i];
		if (G.m_data)
		{
			hasData = true;

			G.textureID = textureID;

			
			int rbX = drawPositionX + G.m_width;
			int rbY = drawPositionY + m_fontHeightMax;

			if (rbX > 512)
			{
				drawPositionX = 0;
				rbX = drawPositionX + G.m_width;

				drawPositionY = rbY;
				rbY = drawPositionY + m_fontHeightMax;

				if (rbY > 512)
				{
					/*char buf[100];
					sprintf_s(buf, 100, "font%i.png", imageIndex++);
					alLib::SaveImage(buf, &img, alSaveImageType::png);*/
					
					m_testFont->AddTexture(m_gs->CreateTexturePoint(&img));
					++textureID;

					drawPositionY = 0;
					rbY = drawPositionY + m_fontHeightMax;
					
					hasData = false;

					img.Fill(alColor(0.f,0.f,0.f,0.f));
				}
			}

			img.Fill(G.m_data, alVec2u(G.m_width, m_fontHeightMax), alVec2u(drawPositionX, drawPositionY), 0, &uv);
			m_testFont->SetGlyph((char32_t)i, G.textureID, m_fontHeightMax, G.m_width, &uv);
			m_testFont->GetGlyph((char32_t)i)->underhang = G.underhang;
			m_testFont->GetGlyph((char32_t)i)->overhang = G.overhang;

			drawPositionX = rbX;
		}
	}
	if (hasData)
	{
	//	char buf[100];
	//	sprintf_s(buf, 100, "font%i.png", imageIndex++);
	//	alLib::SaveImage(buf, &img, alSaveImageType::png);
		m_testFont->AddTexture(m_gs->CreateTexturePoint(&img));
	}
}

void FontTool::OnButtonSave()
{
	m_guiPanel_edit->SetVisible(false);
	m_guiPanel_save->SetVisible(true);
	OnComboSaveSize(-1);
}

void FontTool::OnButtonSave_Save()
{
	alStringW str;
	alLib::OpenSaveFileDialog(m_mainWindow,
		L"Save",
		L"Save",
		L"zip",
		&str);
	if (str.size())
	{
		std::filesystem::path p(str.c_str());
		auto name = p.stem();
		auto name_string = name.generic_string();

		int imageIndex = 0;
		alImage img;
		img.Create(m_saveImageSize, m_saveImageSize);
		int drawPositionX = 0;
		int drawPositionY = 0;

		alUnicodeString ustr;

		alArray<alStringA> zipfiles;
		bool hasData = false;
		for (uint32_t i = 0; i < 0x10FFFF; ++i)
		{
			hasData = true;
			auto G = m_glyphs[i];
			if (G.m_data)
			{
				m_glyphs[i].textureID = imageIndex;

				int rbX = drawPositionX + G.m_width;
				int rbY = drawPositionY + m_fontHeightMax;

				if (rbX > m_saveImageSize)
				{
					drawPositionX = 0;
					rbX = drawPositionX + G.m_width;

					drawPositionY = rbY;
					rbY = drawPositionY + m_fontHeightMax;

					if (rbY > m_saveImageSize)
					{
						char buf[100];
						sprintf_s(buf, 100, "%s%i.png", name_string.c_str(), imageIndex);
						alLib::SaveImage(buf, &img, alSaveImageType::png);
						hasData = false;

						zipfiles.push_back(buf);

						imageIndex++;

						drawPositionY = 0;
						rbY = drawPositionY + m_fontHeightMax;

						img.Fill(alColor(0.f,0.f,0.f,0.f));
					}
				}

				img.Fill(G.m_data, alVec2u(G.m_width, m_fontHeightMax), alVec2u(drawPositionX, drawPositionY), 0, 0);

				// write code, not char
				ustr.Append((uint32_t)i);

				ustr.Append(" ");
				ustr.Append(drawPositionX);
				ustr.Append(" ");
				ustr.Append(drawPositionY);
				ustr.Append(" ");
				ustr.Append(drawPositionX + G.m_width);
				ustr.Append(" ");
				ustr.Append(drawPositionY + (int)m_fontHeightMax);
				ustr.Append(" ");
				ustr.Append(G.underhang);
				ustr.Append(" ");
				ustr.Append(G.overhang);
				ustr.Append(" ");
				ustr.Append(G.textureID);
				ustr.Append("\n");

				drawPositionX = rbX;
			}
		}

		if (hasData)
		{
			char buf[100];
			sprintf_s(buf, 100, "%s%i.png", name_string.c_str(), imageIndex);
			alLib::SaveImage(buf, &img, alSaveImageType::png);
			zipfiles.push_back(buf);
		}

		{
			char buf[100];
			sprintf_s(buf, 100, "%s.txt", name_string.c_str());
			ustr.SaveToFileUTF32(buf);
			zipfiles.push_back(buf);
		}

		if (zipfiles.m_size)
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
			{
				char buf[100];
				sprintf_s(buf, 100, "%s.zip", name_string.c_str());
				alArchive::SaveZip(files, zipfiles.m_size, buf);
			}

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

		m_guiPanel_edit->SetVisible(true);
		m_guiPanel_save->SetVisible(false);
	}
}

void FontTool::OnButtonSave_Cancel()
{
	m_guiPanel_edit->SetVisible(true);
	m_guiPanel_save->SetVisible(false);
}

void FontTool::OnComboSaveSize(uint32_t index)
{
	if (index != -1)
	{
		switch (index)
		{
		default:
		case 0:
			m_saveImageSize = 256;
			break;
		case 1:
			m_saveImageSize = 512;
			break;
		case 2:
			m_saveImageSize = 1024;
			break;
		case 3:
			m_saveImageSize = 2048;
			break;
		case 4:
			m_saveImageSize = 4096;
			break;
		}
	}

	int imageNumber = 1;
	int drawPositionX = 0;
	int drawPositionY = 0;
	for (int i = 0; i < 0x10FFFF; ++i)
	{
		auto G = m_glyphs[i];
		if (G.m_data)
		{
			int rbX = drawPositionX + G.m_width;
			int rbY = drawPositionY + m_fontHeightMax;

			if (rbX > m_saveImageSize)
			{
				drawPositionX = 0;
				rbX = drawPositionX + G.m_width;

				drawPositionY = rbY;
				rbY = drawPositionY + m_fontHeightMax;

				if (rbY > m_saveImageSize)
				{
					++imageNumber;
					drawPositionY = 0;
					rbY = drawPositionY + m_fontHeightMax;
				}
			}
			drawPositionX = rbX;
		}
	}

	auto e = m_guiPanel_save->GetElementByID(FontToolGUIID_textSave_NumImages);
	if (e)
	{
		alGUIText* text = dynamic_cast<alGUIText*>(e);
		if (text)
		{
			alUnicodeString str;
			str = U"Texture Size. It will use ";
			str.Append(imageNumber);
			str.Append(U" images");
			text->SetText(str.c_str());
			text->Rebuild();
		}
	}
}

void FontTool::ShowPopupOnEditRect()
{
	alSystemPopup* popup = alLib::CreateSystemPopup();
	if (popup)
	{
		bool canPaste = false;
		uint32_t clipboardDataSize = 0;
		alLib::GetDataFromClipboard(0, &clipboardDataSize);
		if (clipboardDataSize > sizeof(BITMAPINFOHEADER))
		{
			uint8_t* data = (uint8_t*)malloc(clipboardDataSize);
			if (data)
			{
				alLib::GetDataFromClipboard(data, &clipboardDataSize);
				BITMAPINFOHEADER* hdr = (BITMAPINFOHEADER*)data;
				if (hdr->biSize == 40
					&& hdr->biWidth
					&& hdr->biHeight
					&& hdr->biBitCount == 32)
				{
					canPaste = true;
					/*uint32_t srcRowSz = hdr->biWidth * 4;
					uint32_t srcSz = srcRowSz * hdr->biHeight;
					uint8_t* src = (uint8_t*)alMemory::Malloc(srcSz);
					if (src)
					{
						memcpy(src, &data[sizeof(hdr->biSize)], srcSz);

						alImage img;
						img.m_bits = 32;
						img.m_data = G.
					}*/
				}
				free(data);
			}
		}
		
		alInput* input = alLib::GetInput();
		auto G = &m_glyphs[m_selected];
		if (G->m_data)
		{
			popup->AddItem(U"Copy", FontToolGUIID_popupEdit_Copy, 0);
			popup->AddItem(U"Clear", FontToolGUIID_popupEdit_Clear, 0);

			if (canPaste)
				popup->AddItem(U"Paste", FontToolGUIID_popupEdit_Paste, 0);
		}
		
		popup->Show(m_mainWindow, input->m_cursorCoords.x, input->m_cursorCoords.y);

		AL_DESTROY(popup);
	}
}

void FontTool::ShowPopupOnCell()
{
	alSystemPopup* popup = alLib::CreateSystemPopup();
	if (popup)
	{
		bool canPaste = false;
		uint32_t clipboardDataSize = 0;
		alLib::GetDataFromClipboard(0, &clipboardDataSize);
		if (clipboardDataSize > sizeof(FontToolClipboardData_t))
		{
			uint8_t* data = (uint8_t*)malloc(clipboardDataSize);
			if (data)
			{
				alLib::GetDataFromClipboard(data, &clipboardDataSize);
				FontToolClipboardData_t* d = (FontToolClipboardData_t*)data;
				if (d->m_magic == FontToolClipboardData_MAGIC
					&& d->m_type == d->type_cell
					&& d->m_fontHeight == m_fontHeightMax
					&& d->m_glyphWidth > 0)
				{
					if(d->m_glyphDataSize == (clipboardDataSize - sizeof(FontToolClipboardData_t)))
						canPaste = true;
				}
				free(data);
			}
		}

		alInput* input = alLib::GetInput();
		auto G = &m_glyphs[m_selected];
		if (G->m_data)
		{
			popup->AddItem(U"Delete", FontToolGUIID_popupCell_Delete, 0);
			popup->AddItem(U"Copy", FontToolGUIID_popupCell_Copy, 0);

			if(canPaste)
				popup->AddItem(U"Paste", FontToolGUIID_popupCell_Paste, 0);
		}
		else
		{
			popup->AddItem(U"Create", FontToolGUIID_popupCell_Create, 0);
			if (canPaste)
				popup->AddItem(U"Paste", FontToolGUIID_popupCell_Paste, 0);
		}
		popup->Show(m_mainWindow, input->m_cursorCoords.x, input->m_cursorCoords.y);

		AL_DESTROY(popup);
	}
}

void FontTool::_pollEvents()
{
	alEvent event;
	while (alLib::PollEvent(event))
	{
		switch (event.m_type)
		{
		case alEventType::User:
		{
			switch (event.m_event_user.m_id)
			{
				case FontToolEventID_popupCell:
					ShowPopupOnCell();
					break;
			}
		}break;
		}
	}
}

void FontTool::OnPopupCommand(uint32_t cmd)
{
	switch (cmd)
	{
	case FontToolGUIID_popupCell_Delete:
		DeleteCell(m_selected);
		break;
	case FontToolGUIID_popupCell_Create:
		CreateCell(m_selected);
		break;
	case FontToolGUIID_popupCell_Copy:
		CopyCellToClipboard(m_selected);
		break;
	case FontToolGUIID_popupCell_Paste:
		PasteCellFromClipboard(m_selected);
		break;
	case FontToolGUIID_popupEdit_Copy:
		CopyImageToClipboard(m_selected);
		break;
	case FontToolGUIID_popupEdit_Paste:
		PasteImageFromClipboard(m_selected);
		break;
	case FontToolGUIID_popupEdit_Clear:
		this->ClearImage(m_selected);
		break;
	}
}

void FontTool::DeleteCell(uint32_t index)
{
	if (index < 0x10FFFF)
	{
		auto G = &m_glyphs[index];
		if (G->m_data)
		{
			alMemory::Free(G->m_data);
			G->m_data = 0;
			UpdateTestFont();
		}
	}
}

void FontTool::CreateCell(uint32_t index)
{
	if (index < 0x10FFFF)
	{
		auto G = &m_glyphs[index];
		if (!G->m_data)
		{
			G->m_width = 10;
			G->textureID = 0;
			G->m_data = (uint8_t*)alMemory::Calloc(G->m_width * m_fontHeightMax * 4);

			UpdateTestFont();
			OnSelect();
		}
	}
}

void FontTool::CopyImageToClipboard(uint32_t index)
{
	if (index < 0x10FFFF)
	{
		auto G = &m_glyphs[index];
		if (G->m_data)
		{
			size_t pixelBytes = G->m_width * m_fontHeightMax * 4;
			size_t totalSize = sizeof(BITMAPV5HEADER) + pixelBytes;
			uint8_t* pMem = (uint8_t*)malloc(totalSize);
			if (pMem)
			{
				BITMAPV5HEADER* hdr = (BITMAPV5HEADER*)pMem;
				memset(hdr, 0, sizeof(BITMAPV5HEADER));
				hdr->bV5Size = sizeof(BITMAPV5HEADER);
				hdr->bV5Width = G->m_width;
				hdr->bV5Height = m_fontHeightMax; // positive = bottom-up
				hdr->bV5Planes = 1;
				hdr->bV5BitCount = 32;
				hdr->bV5Compression = BI_BITFIELDS;
				hdr->bV5SizeImage = pixelBytes;
				hdr->bV5RedMask = 0x00FF0000;
				hdr->bV5GreenMask = 0x0000FF00;
				hdr->bV5BlueMask = 0x000000FF;
				hdr->bV5AlphaMask = 0xFF000000;
				hdr->bV5CSType = LCS_sRGB;

				uint8_t* dst = pMem + sizeof(BITMAPV5HEADER);
				int rowsize = hdr->bV5Width * 4; // width * rgba
				uint8_t* src = G->m_data + (rowsize * hdr->bV5Height) - rowsize;;
				//uint8_t* src2= G->m_data ;

				uint8_t* dstPtr = dst;
				uint8_t* srcPtr = src;
				alImage::rgba* src_rgba = (alImage::rgba*)(srcPtr);
				alImage::rgba* dst_rgba = (alImage::rgba*)(dstPtr);
				for (uint32_t y = 0; y < hdr->bV5Height; ++y)
				{
					for (uint32_t x = 0; x < hdr->bV5Width; ++x)
					{
						dst_rgba->r = src_rgba[x].b;
						dst_rgba->g = src_rgba[x].g;
						dst_rgba->b = src_rgba[x].r;
						dst_rgba->a = src_rgba[x].a;

						dstPtr += 4;
						dst_rgba = (alImage::rgba*)(dstPtr);
					}

					srcPtr -= rowsize;
					src_rgba = (alImage::rgba*)srcPtr;
				}

				/*alImage img;
				img.m_data = dst;
				img.m_bits = 32;
				img.m_width = hdr->bV5Width;
				img.m_height = hdr->bV5Height;
				img.m_pitch = img.m_width * 4;
				img.m_dataSize = img.m_pitch * img.m_height;
				alLib::SaveImage("test.png", &img, alSaveImageType::png);
				img.m_data = 0;*/

				alLib::CopyDataToClipboard(pMem, totalSize, alClipboardDataType::RGBA8);
				free(pMem);
			}
		}
	}
}

void FontTool::PasteImageFromClipboard(uint32_t index)
{
	if (index < 0x10FFFF)
	{
		auto G = &m_glyphs[index];
		if (G->m_data)
		{
			uint32_t clipboardDataSize = 0;
			alLib::GetDataFromClipboard(0, &clipboardDataSize);
			if (clipboardDataSize > sizeof(BITMAPINFOHEADER))
			{
				uint8_t* data = (uint8_t*)malloc(clipboardDataSize);
				if (data)
				{
					alLib::GetDataFromClipboard(data, &clipboardDataSize);
					BITMAPINFOHEADER* hdr = (BITMAPINFOHEADER*)data;
					if (hdr->biSize == 40
						&& hdr->biWidth
						&& hdr->biHeight
						&& hdr->biBitCount == 32)
					{
						uint32_t srcRowSz = hdr->biWidth * 4;
						uint32_t srcSz = srcRowSz * hdr->biHeight;
						uint8_t* src = (uint8_t*)alMemory::Malloc(srcSz);
						if (src)
						{
							memcpy(src, &data[clipboardDataSize - hdr->biSizeImage], srcSz);
							alImage imgSrc;
							imgSrc.m_data = src;
							imgSrc.m_bits = 32;
							imgSrc.m_width = hdr->biWidth;
							imgSrc.m_height = hdr->biHeight;
							imgSrc.m_pitch = imgSrc.m_width * 4;
							imgSrc.m_dataSize = imgSrc.m_pitch * imgSrc.m_height;
							imgSrc.FlipPixel();
							imgSrc.FlipVertical();
							//alLib::SaveImage("imgSrc.png", &imgSrc, alSaveImageType::png);

							alImage imgDst;
							imgDst.m_data = G->m_data;
							imgDst.m_bits = 32;
							imgDst.m_width = G->m_width;
							imgDst.m_height = m_fontHeightMax;
							imgDst.m_pitch = imgDst.m_width * 4;
							imgDst.m_dataSize = imgDst.m_pitch * imgDst.m_height;

							memset(imgDst.m_data, 0, imgDst.m_dataSize);

							imgDst.Fill(src, alVec2u(hdr->biWidth, hdr->biHeight), alVec2u(), 0, 0);
							//alLib::SaveImage("imgDst.png", &imgDst, alSaveImageType::png);

							imgDst.m_data = 0;

							UpdateTestFont();
							OnSelect();
						}
					}
					free(data);
				}
			}
		}
	}
}


void FontTool::CopyCellToClipboard(uint32_t index)
{
	if (index < 0x10FFFF)
	{
		auto G = &m_glyphs[index];
		if (G->m_data)
		{
			FontToolClipboardData_t d;
			d.m_magic = FontToolClipboardData_MAGIC;
			d.m_type = FontToolClipboardData_t::type_cell;
			d.m_glyphWidth = G->m_width;
			d.m_glyphDataSize = G->m_width * (uint32_t)m_fontHeightMax * 4;
			d.m_fontHeight = m_fontHeightMax;
			d.m_overhang = G->overhang;
			d.m_underhang = G->underhang;
			if (!d.m_glyphDataSize)
				return;

			uint8_t* data = (uint8_t*)malloc(d.m_glyphDataSize + sizeof(FontToolClipboardData_t));
			if (!data)
				return;
			
			memcpy(&data[0], &d, sizeof(FontToolClipboardData_t));
			memcpy(&data[sizeof(FontToolClipboardData_t)], G->m_data, d.m_glyphDataSize);

			alLib::CopyDataToClipboard(&data[0], d.m_glyphDataSize + sizeof(FontToolClipboardData_t), alClipboardDataType::Data);
			free(data);
		}
	}
}

void FontTool::PasteCellFromClipboard(uint32_t index)
{
	if (index < 0x10FFFF)
	{
		auto G = &m_glyphs[index];

		uint32_t clipboardDataSize = 0;
		alLib::GetDataFromClipboard(0, &clipboardDataSize);
		if (clipboardDataSize > sizeof(FontToolClipboardData_t))
		{
			uint8_t* data = (uint8_t*)malloc(clipboardDataSize);
			if (data)
			{
				alLib::GetDataFromClipboard(data, &clipboardDataSize);
				FontToolClipboardData_t* d = (FontToolClipboardData_t*)data;
				if (d->m_magic == FontToolClipboardData_MAGIC
					&& d->m_type == d->type_cell
					&& d->m_fontHeight == m_fontHeightMax
					&& d->m_glyphWidth > 0)
				{
					if (d->m_glyphDataSize == (clipboardDataSize - sizeof(FontToolClipboardData_t)))
					{

						uint8_t* new_data = (uint8_t*)alMemory::Malloc(d->m_glyphDataSize);
						if (new_data)
						{
							if (G->m_data)
								DeleteCell(index);

							memcpy(new_data, &data[sizeof(FontToolClipboardData_t)], d->m_glyphDataSize);

							G->m_data = new_data;
							G->m_width = d->m_glyphWidth;
							G->overhang = d->m_overhang;
							G->underhang = d->m_underhang;

							UpdateTestFont();
							OnSelect();
						}
					}
				}
				free(data);
			}
		}
	}
}

void FontTool::ClearImage(uint32_t index)
{
	if (index < 0x10FFFF)
	{
		auto G = &m_glyphs[index];
		if (G->m_data)
		{
			memset(G->m_data, 0, G->m_width * m_fontHeightMax * 4);
			UpdateTestFont();
			OnSelect();
		}
	}
}

void FontTool::OnButtonEnableSetWidthMode()
{
	auto G = &m_glyphs[m_selected];
	if (G->m_data)
	{
		m_guiPanel_edit_base->SetVisible(false);
		m_guiPanel_edit_setWidth->SetVisible(true);

		auto slider = dynamic_cast<alGUIRangeSlider1*>(
			m_guiPanel_edit_setWidth->
				GetElementByID(FontToolGUIID_rangeSetWidth));
		if (slider)
		{
			slider->m_ptr_i = &G->m_width;
		}

		_saveGlyphImageOriginal();
	}
}

void FontTool::OnButtonDisableSetWidthMode()
{
	m_guiPanel_edit_base->SetVisible(true);
	m_guiPanel_edit_setWidth->SetVisible(false);

	UpdateTestFont();
	OnSelect();
}

void FontTool::OnSliderSetWidth()
{
	auto G = &m_glyphs[m_selected];
	if (G->m_data)
	{
		alMemory::Free(G->m_data);

		uint32_t dstRowSize = G->m_width * 4;
		uint32_t dataSize = dstRowSize * m_fontHeightMax;		
		G->m_data = (uint8_t*)alMemory::Calloc(dataSize);

		uint8_t* src = (uint8_t*)m_glyphImageOriginal.m_data;
		uint8_t* dst = (uint8_t*)G->m_data;
		
		uint32_t srcRowSize = m_glyphImageOriginal.m_width * 4;

		alImage::rgba* srcRGBA = (alImage::rgba*)src;
		alImage::rgba* dstRGBA = (alImage::rgba*)dst;

		for (uint32_t y = 0; y < m_fontHeightMax; ++y)
		{
			uint32_t src_width_counter = 0;

			for (uint32_t x = 0; x < G->m_width; ++x)
			{
				*dstRGBA = *srcRGBA;

				++dstRGBA;
				++srcRGBA;

				++src_width_counter;
				if (src_width_counter == m_glyphImageOriginal.m_width)
					break;
			}

			src += srcRowSize;
			dst += dstRowSize;

			srcRGBA = (alImage::rgba*)src;
			dstRGBA = (alImage::rgba*)dst;
		}
	}

//	OnSelect();
}

void FontTool::_saveGlyphImageOriginal()
{
	m_glyphImageOriginal.Delete();

	auto G = &m_glyphs[m_selected];
	if (G->m_data)
	{
		m_glyphImageOriginal.Create(G->m_width, m_fontHeightMax);
		memcpy(m_glyphImageOriginal.m_data, G->m_data, m_glyphImageOriginal.m_dataSize);
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


