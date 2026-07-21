#ifndef _AL_GUIPANELH_
#define _AL_GUIPANELH_

#include "Classes/alColor.h"

class alGUIPanel : public alGUIElement
{
	alArray<alGUIElement*> m_elements;
	alArray<alGUIElement*> m_elementsOwn; // for destroying
public:
	alGUIPanel(alGUIContext* ct);
	virtual ~alGUIPanel();

	void Update(float32_t dt) override;
	void Draw(float32_t dt) override;
	void Rebuild() override;

	void AddElement(alGUIElement* e, bool own = false);

	bool m_drawBG = true;
};


#endif

