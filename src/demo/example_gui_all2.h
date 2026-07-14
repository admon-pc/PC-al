#pragma once

class DemoExample_gui_all2_checkbox : public alGUIButtonIcon
{
public:
	DemoExample_gui_all2_checkbox(alGUIContext* ct,
		alGUITextureAtlas* ta, uint32_t ii)
		:
		alGUIButtonIcon(ct, ta, ii)
	{}
	virtual ~DemoExample_gui_all2_checkbox() {}

	virtual void OnButtonToggleOn() override;
	virtual void OnButtonToggleOff() override;
};

class DemoExample_gui_all2_button : public alGUIButton
{
public:
	DemoExample_gui_all2_button(alGUIContext* ct)
		:alGUIButton(ct)	{}
	virtual ~DemoExample_gui_all2_button() {}
	virtual void OnButtonRelease() override;
};

class DemoExample_gui_all2_TextInput : public alGUITextInput
{
public:
	DemoExample_gui_all2_TextInput(alGUIContext* ct):
		alGUITextInput(ct)
	{}
	virtual ~DemoExample_gui_all2_TextInput() {}

	virtual void OnRMBRelease() override;

	virtual void OnAccept() override;
	virtual void OnCancel() override;
};

class DemoExample_gui_all2_combo : public alGUIComboBox
{
public:
	DemoExample_gui_all2_combo(alGUIContext* ct)
		:alGUIComboBox(ct) {}
	virtual ~DemoExample_gui_all2_combo() {}
	virtual void OnComboSelectItem(size_t) override;
};
class DemoExample_gui_all2_list : public alGUIListBox
{
public:
	DemoExample_gui_all2_list(alGUIContext* ct)
		:alGUIListBox(ct) {}
	virtual ~DemoExample_gui_all2_list() {}
	virtual void OnListSelectItem(size_t) override;
};

class DemoExample_gui_all2_slider1 : public alGUIRangeSlider1
{
public:
	DemoExample_gui_all2_slider1(alGUIContext* ct)
		:alGUIRangeSlider1(ct) {}
	virtual ~DemoExample_gui_all2_slider1() {}
};
class DemoExample_gui_all2_slider2 : public alGUIRangeSlider1
{
public:
	DemoExample_gui_all2_slider2(alGUIContext* ct)
		:alGUIRangeSlider1(ct) {}
	virtual ~DemoExample_gui_all2_slider2() {}
};

class DemoExample_gui_all2 : public alDemoExample
{
	alGS* m_gs = 0;
	alInput* m_input = 0;
	alGUIPanel* m_GUIPanel = 0;
	alGUITextureAtlas* m_textureAtlas = 0;
	alGSTexture* m_textureAtlasTexture = 0;
	alCursorType m_currentCursor = alCursorType::Arrow;
public:
	DemoExample_gui_all2(const char32_t* title, const char32_t* desc)
	:
		alDemoExample(title, desc)
	{}
	virtual ~DemoExample_gui_all2() {}

	virtual bool Init() override;
	virtual void Shutdown() override;
	virtual bool Run() override;
	
	virtual void OnPopupCommand(uint32_t cmd) override;
	virtual void OnWindowSizeChanged() override;
	

	DemoExample_gui_all2_TextInput* m_textInput_editor = 0;
	DemoExample_gui_all2_TextInput* m_textInput_editor2 = 0;
	DemoExample_gui_all2_TextInput* m_textInput_editor_oneLine = 0;
	DemoExample_gui_all2_checkbox* m_checkbox_usehscroll = 0;
	DemoExample_gui_all2_checkbox* m_checkbox_usevscroll = 0;
	DemoExample_gui_all2_button* m_button_deactivateEditor = 0;
	alSystemPopup* m_textInput_editorPopup = 0;
	DemoExample_gui_all2_combo* m_combobox1 = 0;
	DemoExample_gui_all2_list* m_listbox1 = 0;
	
	DemoExample_gui_all2_slider1* m_slider1 = 0;
	int32_t m_slider1Var = 0;

	DemoExample_gui_all2_slider2* m_slider2 = 0;
	float32_t m_slider2Var = 0;

	struct directory_files
	{
		uint32_t m_flags = 0;
		uint32_t m_importantData[4];
		char32_t m_name[100];
		uint32_t m_importantData2[4];
		
	};
	alArray<directory_files> m_dirFiles;

	alUnicodeString m_name;

};

