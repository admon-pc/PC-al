#include "demo.h"
#include "example_gui_all2.h"

#include "Image/alImage.h"

#include "System/alSystemPopup.h"
#include "System/alCursor.h"

#include <filesystem>

#define DEMOGUIALL2_ID_CUT 101
#define DEMOGUIALL2_ID_COPY 102
#define DEMOGUIALL2_ID_PASTE 103
#define DEMOGUIALL2_ID_DELETE 104
#define DEMOGUIALL2_ID_SELECTALL 105

#define DEMOGUIALL2_CHECKBOXID_USEHSCROLL 1
#define DEMOGUIALL2_CHECKBOXID_USEVSCROLL 2
#define DEMOGUIALL2_BUTTON_DEACTIVATE 1



extern alDemo* g_demo;

void DemoExample_gui_all2_checkbox::OnButtonToggleOn()
{
	DemoExample_gui_all2* ex = (DemoExample_gui_all2*)GetUserData();
	switch (this->GetID())
	{
	case DEMOGUIALL2_CHECKBOXID_USEHSCROLL:
		ex->m_textInput_editor->m_useHorizontalScrollbar = true;
		ex->m_textInput_editor->Rebuild();
		break;
	case DEMOGUIALL2_CHECKBOXID_USEVSCROLL:
		ex->m_textInput_editor->m_useVerticalScrollbar = true;
		ex->m_textInput_editor->Rebuild();
		break;
	}
}

void DemoExample_gui_all2_checkbox::OnButtonToggleOff()
{
	DemoExample_gui_all2* ex = (DemoExample_gui_all2*)GetUserData();
	switch (this->GetID())
	{
	case DEMOGUIALL2_CHECKBOXID_USEHSCROLL:
		ex->m_textInput_editor->m_useHorizontalScrollbar = false;
		ex->m_textInput_editor->Rebuild();
		break;
	case DEMOGUIALL2_CHECKBOXID_USEVSCROLL:
		ex->m_textInput_editor->m_useVerticalScrollbar = false;
		ex->m_textInput_editor->Rebuild();
		break;
	}
}

void DemoExample_gui_all2_button::OnButtonRelease()
{
	DemoExample_gui_all2* ex = (DemoExample_gui_all2*)GetUserData();
	switch (this->GetID())
	{
	case DEMOGUIALL2_BUTTON_DEACTIVATE:
		ex->m_textInput_editor->Deactivate(true);
		ex->m_textInput_editor2->Deactivate(true);
		break;
	}
}


void DemoExample_gui_all2::OnPopupCommand(uint32_t cmd)
{
	switch (cmd)
	{
	case DEMOGUIALL2_ID_CUT:
		m_textInput_editor->CutToClipboard();
		break;
	case DEMOGUIALL2_ID_COPY:
		m_textInput_editor->CopyToClipboard();
		break;
	case DEMOGUIALL2_ID_PASTE:
		if(m_textInput_editor->IsActive())
			m_textInput_editor->PasteFromClipboard();
		if (m_textInput_editor2->IsActive())
			m_textInput_editor2->PasteFromClipboard();
		break;
	case DEMOGUIALL2_ID_DELETE:
		m_textInput_editor->DeleteSelected();
		break;
	case DEMOGUIALL2_ID_SELECTALL:
		m_textInput_editor->SelectAll();
		break;
	}
}
void DemoExample_gui_all2::OnWindowSizeChanged()
{
	if (m_GUIPanel)
	{
		m_GUIPanel->m_size.Set(g_demo->m_mainWindow->m_clientSize.x,
			g_demo->m_mainWindow->m_clientSize.y);
		m_GUIPanel->Rebuild();
	}
}

void DemoExample_gui_all2_TextInput::OnRMBRelease()
{
	DemoExample_gui_all2* ex = (DemoExample_gui_all2*)GetUserData();
	auto input = alLib::GetInput();
	alLib::GetCursor(alCursorType::Arrow)->Activate();
	ex->m_textInput_editorPopup->Show(
		g_demo->m_mainWindow,
		input->m_cursorCoordsForGUI.x,
		input->m_cursorCoordsForGUI.y);
}

void DemoExample_gui_all2_TextInput::OnAccept()
{
	DemoExample_gui_all2* ex = (DemoExample_gui_all2*)GetUserData();
	if (m_text.Size())
		ex->m_name = m_text;
	else
		ex->m_name.Clear();
}

void DemoExample_gui_all2_TextInput::OnCancel()
{
	DemoExample_gui_all2* ex = (DemoExample_gui_all2*)GetUserData();
	m_text = ex->m_name;
}

void DemoExample_gui_all2_combo::OnComboSelectItem(size_t index)
{
	DemoExample_gui_all2* ex = (DemoExample_gui_all2*)GetUserData();
	uint8_t* ptr = (uint8_t*)m_items;
	m_text = (char32_t*)(&ptr[index * m_stride] + m_textOffset);
}

bool DemoExample_gui_all2::Init()
{
	m_gs = g_demo->m_gs;
	m_input = alLib::GetInput();

	auto img = alLib::LoadALImage("../data/demo/3.png");
	if (img)
	{
		m_textureAtlasTexture = m_gs->CreateTexturePoint(img);
		AL_DESTROY(img);
	}
	m_textureAtlas = alCreate<alGUITextureAtlas>(m_textureAtlasTexture);
	uint32_t iconIndex_iconBase = m_textureAtlas->AddUV(alVec2u(0, 0), alVec2u(14, 14));
	uint32_t iconIndex_iconCheck1 = m_textureAtlas->AddUV(alVec2u(14, 0), alVec2u(14, 14));
	uint32_t iconIndex_iconCheck2 = m_textureAtlas->AddUV(alVec2u(28, 0), alVec2u(14, 14));

	m_textInput_editorPopup = alLib::CreateSystemPopup();
	m_textInput_editorPopup->AddItem(U"Cut", DEMOGUIALL2_ID_CUT, 0);
	m_textInput_editorPopup->AddItem(U"Copy", DEMOGUIALL2_ID_COPY, 0);
	m_textInput_editorPopup->AddItem(U"Paste", DEMOGUIALL2_ID_PASTE, 0);
	m_textInput_editorPopup->AddItem(U"Delete", DEMOGUIALL2_ID_DELETE, 0);
	m_textInput_editorPopup->AddSeparator();
	m_textInput_editorPopup->AddItem(U"Select All", DEMOGUIALL2_ID_SELECTALL, 0);

	m_textInput_editor = alCreate<DemoExample_gui_all2_TextInput>(g_demo->m_GUI);
	m_textInput_editor->SetUserData(this);
	m_textInput_editor->m_position.Set(3.f, 5.f);
	m_textInput_editor->m_size.Set(300,200);

	alUnicodeString ustr;
	//ustr.ReadFromFile("D:\\texteditor.txt");
	ustr.Assign(U"111");

	m_textInput_editor->SetText(ustr.c_str(), ustr.Size());
	m_textInput_editor->SetFont(g_demo->m_guiFont,0);
//	m_textInput_editor->Activate();
	g_demo->m_guiFont->m_unknownGlyph = U' ';

	m_textInput_editor2 = alCreate<DemoExample_gui_all2_TextInput>(g_demo->m_GUI);
	m_textInput_editor2->SetUserData(this);
	m_textInput_editor2->m_position.Set(305.f, 5.f);
	m_textInput_editor2->m_size.Set(300, 200);
	m_textInput_editor2->SetFont(g_demo->m_guiFont,0);
	m_textInput_editor2->m_textDefault.Assign(U"Default text");
	m_textInput_editor2->m_useBottombar = false;
	m_textInput_editor2->m_useHorizontalScrollbar = false;
	m_textInput_editor2->m_useLinebar = false;
	m_textInput_editor2->m_useVerticalScrollbar = false;

	m_textInput_editor_oneLine = alCreate<DemoExample_gui_all2_TextInput>(g_demo->m_GUI);
	m_textInput_editor_oneLine->SetUserData(this);
	m_textInput_editor_oneLine->m_position.Set(610.f, 5.f);
	m_textInput_editor_oneLine->m_size.Set(140, 18);
	m_textInput_editor_oneLine->SetFont(g_demo->m_guiFont, 0);
	m_textInput_editor_oneLine->m_textDefault.Assign(U"Enter Name");
	m_textInput_editor_oneLine->m_oneLine = true;
	m_textInput_editor_oneLine->m_useBottombar = false;
	m_textInput_editor_oneLine->m_useHorizontalScrollbar = false;
	m_textInput_editor_oneLine->m_useLinebar = false;
	m_textInput_editor_oneLine->m_useVerticalScrollbar = false;

	m_checkbox_usehscroll = alCreate<DemoExample_gui_all2_checkbox>(g_demo->m_GUI, m_textureAtlas, iconIndex_iconBase);
	m_checkbox_usehscroll->m_position.Set(0.f, 205.f);
	m_checkbox_usehscroll->m_size.Set(14.f, 14.f);
	m_checkbox_usehscroll->m_lerpColors = true;
	m_checkbox_usehscroll->m_toggleButton = true;
	m_checkbox_usehscroll->m_iconIndexMouseHover = iconIndex_iconCheck1;
	m_checkbox_usehscroll->m_iconIndexPress = iconIndex_iconCheck2;
	m_checkbox_usehscroll->m_useText = true;
	m_checkbox_usehscroll->m_isToggleOn = m_textInput_editor->m_useHorizontalScrollbar;
	m_checkbox_usehscroll->SetText(U"Use horizontal scrollbar");
	m_checkbox_usehscroll->SetFont(g_demo->m_guiFontMini);
	m_checkbox_usehscroll->SetID(DEMOGUIALL2_CHECKBOXID_USEHSCROLL);
	m_checkbox_usehscroll->SetUserData(this);

	m_checkbox_usevscroll = alCreate<DemoExample_gui_all2_checkbox>(g_demo->m_GUI, m_textureAtlas, iconIndex_iconBase);
	m_checkbox_usevscroll->m_position.Set(0.f, 215.f);
	m_checkbox_usevscroll->m_size.Set(14.f, 14.f);
	m_checkbox_usevscroll->m_lerpColors = true;
	m_checkbox_usevscroll->m_toggleButton = true;
	m_checkbox_usevscroll->m_iconIndexMouseHover = iconIndex_iconCheck1;
	m_checkbox_usevscroll->m_iconIndexPress = iconIndex_iconCheck2;
	m_checkbox_usevscroll->m_useText = true;
	m_checkbox_usevscroll->m_isToggleOn = m_textInput_editor->m_useHorizontalScrollbar;
	m_checkbox_usevscroll->SetText(U"Use vertical scrollbar");
	m_checkbox_usevscroll->SetFont(g_demo->m_guiFontMini);
	m_checkbox_usevscroll->SetID(DEMOGUIALL2_CHECKBOXID_USEVSCROLL);
	m_checkbox_usevscroll->SetUserData(this);

	m_button_deactivateEditor = alCreate<DemoExample_gui_all2_button>(g_demo->m_GUI);
	m_button_deactivateEditor->m_position.Set(10.f, 230);
	m_button_deactivateEditor->m_size.Set(100.f, 30.f);
	m_button_deactivateEditor->SetText(U"Deactivate");
	m_button_deactivateEditor->SetFont(g_demo->m_guiFont);
	m_button_deactivateEditor->m_lerpColors = true;
	m_button_deactivateEditor->m_roundCorners = true;
	m_button_deactivateEditor->m_roundIterations = 3;
	m_button_deactivateEditor->m_roundSize = 3;
	m_button_deactivateEditor->SetUserData(this);
	m_button_deactivateEditor->SetID(DEMOGUIALL2_BUTTON_DEACTIVATE);

	m_dirFiles.clear();
	{
		for (const auto& entry : std::filesystem::recursive_directory_iterator("./"))
		{
			if (entry.is_regular_file())
			{
				directory_files o;
				memset(&o, 0, sizeof(o));
				alLib::sprintf(o.m_name, U"%s", entry.path().generic_u32string().c_str());
				m_dirFiles.push_back(o);
			}
		}
		for (int i = 0; i < 113; ++i)
		{
			directory_files o;
			memset(&o, 0, sizeof(o));
			alLib::sprintf(o.m_name, U"Object%i", i);
			m_dirFiles.push_back(o);
		}
	}

	m_combobox1 = alCreate<DemoExample_gui_all2_combo>(g_demo->m_GUI);
	m_combobox1->m_position.Set(5, 300);
	m_combobox1->m_size.Set(100, 30);
	m_combobox1->SetFont(g_demo->m_guiFont);
	m_combobox1->m_text.Assign(U"...");
	m_combobox1->SetItems(m_dirFiles.m_data, m_dirFiles.size(),
		sizeof(directory_files), 16);

	m_GUIPanel = g_demo->m_GUI->GetNewPanel();
	m_GUIPanel->m_drawBG = false;
	m_GUIPanel->m_position.Set(0.f, 0.f);
	m_GUIPanel->AddElement(m_textInput_editor);
	m_GUIPanel->AddElement(m_textInput_editor2);
	m_GUIPanel->AddElement(m_checkbox_usehscroll);
	m_GUIPanel->AddElement(m_checkbox_usevscroll);
	m_GUIPanel->AddElement(m_button_deactivateEditor);
	m_GUIPanel->AddElement(m_textInput_editor_oneLine);
	m_GUIPanel->AddElement(m_combobox1);

	m_GUIPanel->m_size.Set(g_demo->m_mainWindow->m_clientSize.x,
		g_demo->m_mainWindow->m_clientSize.y);
	m_GUIPanel->Rebuild();

	return true;
}

void DemoExample_gui_all2::Shutdown()
{
	AL_DESTROY(m_textInput_editorPopup);
	AL_DESTROY(m_textureAtlas);
	m_name.Clear();
	g_demo->m_GUI->DeleteAllPanels();
	alLib::GetCursor(alCursorType::Arrow)->Activate();
}


bool DemoExample_gui_all2::Run()
{
	g_demo->m_GUI->Update(*g_demo->m_dt);
	m_currentCursor = g_demo->m_GUI->m_cursorType;
	
	alLib::SetCursor(m_currentCursor, alLib::GetCursor(m_currentCursor));

//	Sleep(100);

	if (m_input->IsKeyHit(alInputKey::K_ESCAPE))
	{
		if (!m_textInput_editor->IsActive()
			&& !m_textInput_editor2->IsActive()
			&& !m_textInput_editor_oneLine->IsActive())
		{
			Shutdown();
			return false;
		}
	}

	m_gs->BeginDraw();
	m_gs->ClearAll();
	m_gs->EndDraw();

	m_gs->BeginDrawGUI();
	g_demo->m_GUI->Draw(*g_demo->m_dt);

	m_gs->SetScissorRect(alVec4f(0, 0, 
		(float32_t)g_demo->m_mainWindow->m_clientSize.x,
		(float32_t)g_demo->m_mainWindow->m_clientSize.y));
	
	m_gs->DrawText(
		U"Name:",
		5,
		g_demo->m_guiFont,
		alVec2f(610, 55),
		ColorWhite);
	if (m_name.Size())
	{
		m_gs->DrawText(
			m_name.Data(),
			m_name.Size(),
			g_demo->m_guiFont,
			alVec2f(645, 55),
			ColorWhite);
	}

	m_gs->EndDrawGUI();
	m_gs->SwapBuffers();
	return true;
}