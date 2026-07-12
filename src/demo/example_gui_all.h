#pragma once



class DemoExample_gui_all_testTextObject : public alGUIText
{
public:
	DemoExample_gui_all_testTextObject(alGUIContext* ct) :alGUIText(ct) {}
	virtual ~DemoExample_gui_all_testTextObject() {}

	virtual void OnMouseEnter()override;
	virtual void OnMouseLeave()override;
	virtual void OnMouseMove()override;
	virtual void OnLMBHit()override;
	virtual void OnRMBHit()override;
	virtual void OnMMBHit()override;
};

class DemoExample_gui_all_pictureBox : public alGUIPictureBox
{
public:
	DemoExample_gui_all_pictureBox(alGUIContext* ct) :alGUIPictureBox(ct) {}
	virtual ~DemoExample_gui_all_pictureBox() {}

	bool m_zoom = false;
	alUnicodeString m_clickedPosition;

	virtual void OnMouseEnter()override {}
	virtual void OnMouseLeave()override {}
	virtual void OnMouseMove() override {}
	virtual void OnLMBHit()override;
	virtual void OnRMBHit() override {}
	virtual void OnMMBHit() override {}
};

class DemoExample_gui_all_TestButton : public alGUIButton
{
public:
	DemoExample_gui_all_TestButton(alGUIContext* ct) :alGUIButton(ct) {}
	virtual ~DemoExample_gui_all_TestButton() {}
	virtual void OnButtonRelease() override;
};

class DemoExample_gui_all_TestButtonIcon : public alGUIButtonIcon
{
public:
	DemoExample_gui_all_TestButtonIcon(alGUIContext* ct,
		alGUITextureAtlas* ta, uint32_t ii)
		:
		alGUIButtonIcon(ct,ta,ii) 
	{}
	virtual ~DemoExample_gui_all_TestButtonIcon() {}
};

class DemoExample_gui_all : public alDemoExample
{
	friend class DemoExample_gui_all_testTextObject;
	friend class DemoExample_gui_all_TestButton;

	alGS* m_gs = 0;
	alInput* m_input = 0;

	alGSTexture* m_texture = 0;
	alGSTexture* m_textureAtlasTexture = 0;

	bool m_buttonExitPressed = false;
	
	alGUIPanel* m_GUIPanel = 0;

	alUnicodeString m_textObject2_text;
	bool m_textObject2_mouseMoved = false;
	float32_t m_textObject2_mouseMoved_timer = 0.f;

	DemoExample_gui_all_testTextObject* m_textObject1 = 0;
	DemoExample_gui_all_testTextObject* m_textObject2 = 0;
	DemoExample_gui_all_testTextObject* m_textObject2minifont = 0;
	DemoExample_gui_all_TestButton* m_button1 = 0;
	DemoExample_gui_all_TestButton* m_button2 = 0;
	DemoExample_gui_all_TestButton* m_button3 = 0;
	DemoExample_gui_all_TestButton* m_button4 = 0;
	DemoExample_gui_all_TestButton* m_button5 = 0;
	DemoExample_gui_all_TestButton* m_button6 = 0;
	DemoExample_gui_all_TestButton* m_buttonToggle = 0;
	DemoExample_gui_all_TestButtonIcon* m_buttonIcon1 = 0;
	DemoExample_gui_all_TestButtonIcon* m_buttonIcon2 = 0;
	DemoExample_gui_all_TestButtonIcon* m_buttonIcon3 = 0;
	DemoExample_gui_all_TestButtonIcon* m_buttonIcon3textR = 0;
	DemoExample_gui_all_TestButtonIcon* m_buttonIconRadio1_1 = 0;
	DemoExample_gui_all_TestButtonIcon* m_buttonIconRadio2_1 = 0;
	DemoExample_gui_all_TestButtonIcon* m_buttonIconRadio3_1 = 0;
	DemoExample_gui_all_TestButtonIcon* m_buttonIconRadio1_2 = 0;
	DemoExample_gui_all_TestButtonIcon* m_buttonIconRadio2_2 = 0;
	DemoExample_gui_all_TestButtonIcon* m_buttonIconRadio3_2 = 0;

	alGUITextureAtlas* m_textureAtlas = 0;

	DemoExample_gui_all_pictureBox* m_pictureBox = 0;
public:
	DemoExample_gui_all(const char32_t* title, const char32_t* desc)
	:
		alDemoExample(title, desc)
	{}
	virtual ~DemoExample_gui_all() {}

	virtual bool Init() override;
	virtual void Shutdown() override;

	virtual bool Run() override;
};

