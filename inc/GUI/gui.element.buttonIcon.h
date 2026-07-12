#ifndef _AL_GUIBIH_
#define _AL_GUIBIH_

class alGUIButtonIcon : public alGUIElement
{
	alGUITextureAtlas* m_textureAtlas = 0;
	alColor m_currCol;
	uint32_t m_currIcon = 0;

	bool m_isClicked = false;
	alVec4f m_UV[3];
	
	alUnicodeString m_text;
	alGUIFont* m_font = 0;
	alVec2f m_textPosition;

public:
	alGUIButtonIcon(alGUIContext* ct, alGUITextureAtlas*, uint32_t iconIndex);
	virtual ~alGUIButtonIcon();

	uint32_t m_iconIndexBase = 0;
	uint32_t m_iconIndexMouseHover = 0;
	uint32_t m_iconIndexPress = 0;

	virtual void Draw(float32_t dt) override;
	virtual void Update(float32_t dt) override;
	virtual void Rebuild() override;

	void SetFont(alGUIFont*);
	void SetText(const char32_t* text, ...);
	void ClearText();

	bool m_disabled = false;
	bool m_lerpColors = false;

	bool m_toggleButton = false;
	bool m_isToggleOn = false;

	bool m_useText = false;
	alVec2i m_textIndent;

	bool m_radioButton = false;
	uint32_t m_radioGroup = 0;
	void RadioCheck();

	virtual void OnButtonRelease() {}
	virtual void OnButtonToggleOn() {}
	virtual void OnButtonToggleOff() {}
};


#endif

