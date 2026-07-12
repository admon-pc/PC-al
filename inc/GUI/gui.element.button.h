#ifndef _AL_GUIBUTTONH_
#define _AL_GUIBUTTONH_

class alGUIButton : public alGUIElement
{
	alUnicodeString m_text;
	alGUIFont* m_font = 0;
	alColor m_currColBG1;
	alColor m_currColBG2;
	alColor m_currColText;
	alVec2f m_textPosition;
	alGSMesh* m_bgMesh = 0;
	alGSMesh* m_bgMeshBorder = 0;
public:
	alGUIButton(alGUIContext* ct);
	virtual ~alGUIButton();

	virtual void Draw(float32_t dt) override;
	virtual void Update(float32_t dt) override;
	virtual void Rebuild() override;

	void SetFont(alGUIFont*);
	void SetText(const char32_t* text, ...);
	void ClearText();

	bool m_drawBG = true;
	bool m_disabled = false;
	bool m_lerpColors = false;
	bool m_textInCenter = true;
	bool m_roundCorners = false;
	uint32_t m_roundIterations = 4;
	float32_t m_roundSize = 5.f;
	uint32_t m_borderSize = 0;

	bool m_toggleButton = false;
	bool m_isToggleOn = false;

	virtual void OnButtonRelease() {}
	virtual void OnButtonToggleOn() {}
	virtual void OnButtonToggleOff() {}
};

#endif

