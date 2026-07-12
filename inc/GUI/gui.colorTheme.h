#ifndef _AL_GUICLRTHMH_
#define _AL_GUICLRTHMH_

#include "Classes/alColor.h"

struct alGUIColorTheme
{
	alColor m_text_textColor;
	
	alColor m_panel_bg1;
	alColor m_panel_bg2;
	
	alColor m_popup_bg1;
	alColor m_popup_bg2;
	alColor m_popup_itemText;
	alColor m_popup_bg1_mouseHover;
	alColor m_popup_bg2_mouseHover;
	alColor m_popup_itemText_mouseHover;

	alColor m_button_textColor_enabled;
	alColor m_button_textColor_disabled;
	alColor m_button_textColor_mouseHover;
	alColor m_button_textColor_press;
	alColor m_button_bg1_enabled;
	alColor m_button_bg2_enabled;
	alColor m_button_bg1_disabled;
	alColor m_button_bg2_disabled;
	alColor m_button_bg1_mouseHover;
	alColor m_button_bg2_mouseHover;
	alColor m_button_bg1_press;
	alColor m_button_bg2_press;
	alColor m_button_border;
	
	alColor m_buttonIcon_disabled;
	alColor m_buttonIcon_enabled;
	alColor m_buttonIcon_mouseHover;
	alColor m_buttonIcon_press;
	
	alColor m_edit_bg1;
	alColor m_edit_bg2;
	alColor m_edit_bottomBarBG;
	alColor m_edit_bottomBarUIText;
	alColor m_edit_lineBarBG;
	alColor m_edit_lineBarText;
	alColor m_edit_bg1NA;
	alColor m_edit_bg2NA;
	alColor m_edit_textEnabled;
	alColor m_edit_textDisabled;
	alColor m_edit_textDefault;
	alColor m_edit_textSelected;
	alColor m_edit_textCursor;
	alColor m_edit_selectbg;
	alColor m_edit_scrollBG;
	alColor m_edit_scrollButtonBG;
	alColor m_edit_scrollRect;
	alColor m_edit_currlineBG;

	alColor m_combo_bg_enabled;
	alColor m_combo_bg_disabled;
	alColor m_combo_bg_mouseHover;
	alColor m_combo_bg_press;
	alColor m_combo_textColor_enabled;
	alColor m_combo_textColor_disabled;
	alColor m_combo_textColor_mouseHover;
	alColor m_combo_textColor_press;
};

#endif

