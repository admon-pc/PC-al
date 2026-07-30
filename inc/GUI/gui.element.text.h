#ifndef _AL_GUITEXTH_
#define _AL_GUITEXTH_

class alGUIText : public alGUIElement
{
	alUnicodeString m_text;
	alGUIFont* m_font = 0;
public:
	alGUIText(alGUIContext* ct, const alVec2f& position, const alVec2f& size);
	virtual ~alGUIText();

	virtual void Draw(float32_t dt) override;
	virtual void Update(float32_t dt) override;
	virtual void Rebuild() override;

	virtual void SetFont(alGUIFont*);
	virtual void SetText(const char32_t* text, ...);
	virtual void Clear();

};

#endif

