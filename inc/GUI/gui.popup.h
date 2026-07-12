#ifndef _AL_GUIpopupH_
#define _AL_GUIpopupH_


class alGUIPopup : public alGUIElement
{
	alGUIFont* m_font = 0;
public:
	alGUIPopup(alGUIContext*);
	virtual ~alGUIPopup();

	virtual void Draw(float32_t dt) override;

	void SetPositionAndSize(float32_t posX, float32_t posY, float32_t szX, float32_t szY);
	void Rebuild();
	void Update(float32_t dt);

	void Activate();

	alGUIColorTheme* m_colorTheme = 0;
};

#endif

