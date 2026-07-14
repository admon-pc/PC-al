#ifndef _AL_GUIH_
#define _AL_GUIH_

#include "System/alCursor.h"

#include "GUI/gui.textureAtlas.h"
#include "GUI/gui.element.h"
#include "GUI/gui.font.h"

#include "GUI/gui.colorTheme.h"

#include "GUI/gui.popup.h"

#include "GUI/gui.element.panel.h"

#include "GUI/gui.element.text.h"
#include "GUI/gui.element.button.h"
#include "GUI/gui.element.pictureBox.h"
#include "GUI/gui.element.buttonIcon.h"
#include "GUI/gui.element.textInput.h"
#include "GUI/gui.element.comboBox.h"
#include "GUI/gui.element.listBox.h"

#include "GUI/gui.element.rangeSlider.h"


struct alGUIRadioGroup
{
	alGUIButtonIcon* m_activeButton = 0;
};

class alGUIContext
{

	alSystemWindow* m_window = 0;
	alGS* m_gs = 0;
	alArray<alGUIPanel*> m_panels;
public:
	alGUIContext(alSystemWindow*, alGS*);
	~alGUIContext();
	void Update(float32_t dt);
	void Draw(float32_t dt);
	
	alGUIPanel* GetNewPanel();
	void DeleteAllPanels();


	alGS* GetGS() { return m_gs; }
	alSystemWindow* GetWindow() { return m_window; }

	bool m_isElementUnderCursor = false; // this will be set false every Update
	alGUIElement* m_elementUnderCursor = 0;
	//alGUIPanel* m_panelUnderCursor = 0;
	alGUIPopup* m_activePopup = 0;

	// if some element activated that other elements should not react
	// for: combobox
	alGUIElement* m_activeElement = 0;

	// if some text input element activated
	alGUITextInput* m_inputTextElementActivated = 0;
	
	// when click and hold
	alGUIElement* m_elementInMouseFocus = 0;

	alGSMesh* _create_bg_mesh(
		const alVec4f& buildRect,
		float32_t smooth_corner_indent,
		uint32_t smooth_corner_iterations,
		alVec4f* UV);

	alGUIRadioGroup m_radioGroups[100];

	// This is like recomendation.
	// GUI elements set this variable.
	// On application side dev can use this
	// to change cursor.
	alCursorType m_cursorType = alCursorType::Arrow;
};

#endif

