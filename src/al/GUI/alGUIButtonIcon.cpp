#include "al.h"
#include "GUI/alGUI.h"
#include "GS/alGS.h"
#include "GS/alGSTexture.h"
#include "Classes/alColor.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

alGUIButtonIcon::alGUIButtonIcon(alGUIContext* ct, alGUITextureAtlas* ta, uint32_t iconIndex)
	:
	alGUIElement(ct),
	m_textureAtlas(ta)
{
	m_iconIndexBase = iconIndex;
	m_iconIndexMouseHover = iconIndex;
	m_iconIndexPress = iconIndex;
	m_currIcon = 0;
	m_currCol = ColorWhite;
	m_textIndent.y = 2;
}

alGUIButtonIcon::~alGUIButtonIcon()
{
}

void alGUIButtonIcon::Draw(float32_t dt)
{
	OnDraw();
	m_gs->SetScissorRect(m_clipArea);
	if (m_disabled)
	{
		m_currCol = m_colorTheme->m_buttonIcon_disabled;
	}
	else
	{
		if (m_toggleButton)
		{
			
			if (m_isToggleOn)
			{
				m_currCol = m_colorTheme->m_buttonIcon_press;
				m_currIcon = 2;
			}
			else
			{
				if (m_isCursorInSensorArea)
				{
					float32_t l = 10.f * dt;
					alMath::Lerp1(m_currCol, m_colorTheme->m_buttonIcon_mouseHover, l, m_currCol);
					m_currIcon = 1;
				}
				else
				{
					m_currCol = m_colorTheme->m_buttonIcon_enabled;
					m_currIcon = 0;
				}
			}
		}
		else
		{
			if (m_lerpColors)
			{
				float32_t l = 10.f * dt;
				alMath::Lerp1(m_currCol, m_colorTheme->m_buttonIcon_enabled, l, m_currCol);
				m_currIcon = 0;

				if (m_isLMBClicked)
				{
					alMath::Lerp1(m_currCol, m_colorTheme->m_buttonIcon_press, l, m_currCol);
					m_currIcon = 2;

				}
				else
				{
					if (m_isCursorInSensorArea)
					{
						alMath::Lerp1(m_currCol, m_colorTheme->m_buttonIcon_mouseHover, l, m_currCol);
						m_currIcon = 1;
					}
				}
			}
			else
			{
				m_currCol = m_colorTheme->m_buttonIcon_enabled;
				m_currIcon = 0;


				if (m_isLMBClicked)
				{
					m_currCol = m_colorTheme->m_buttonIcon_press;
					m_currIcon = 2;

				}
				else
				{

					if (m_isCursorInSensorArea)
					{
						m_currCol = m_colorTheme->m_buttonIcon_mouseHover;
						m_currIcon = 1;

					}
				}
			}
		}
	}
	m_gs->DrawRectangle(
		m_buildArea,
		m_currCol,
		m_currCol,
		m_textureAtlas->GetTexture(),
		&m_UV[m_currIcon]);

	if (m_useText && m_text.Size() && m_font)
	{
		m_gs->DrawText(m_text.Data(),
			m_text.Size(),
			m_font,
			m_textPosition, 
			m_currCol);
	}
}

void alGUIButtonIcon::Update(float32_t dt)
{
	bool isLMBClick = m_isLMBClicked;
	bool isRMBClick = m_isRMBClicked;
	bool isMMBClick = m_isMMBClicked;

	alGUIElement::Update(dt);
	
	if (m_isCursorInSensorArea)
	{
		/*if (m_radioButton)
		{
		}
		else */if (m_toggleButton)
		{
			if (m_isLMBClicked && m_input->m_isLMBDown)
			{
				if (m_radioButton)
				{
					OnButtonToggleOn();
					this->RadioCheck();
				}
				else
				{
					if (m_isToggleOn)
					{
						m_isToggleOn = false;
						OnButtonToggleOff();
					}
					else
					{
						m_isToggleOn = true;
						OnButtonToggleOn();
					}
				}
			}
		}
		else
		{
			if (isLMBClick && (!m_isLMBClicked))
			{
				OnButtonRelease();
			}
		}
	}
	else
	{
	}
}

void alGUIButtonIcon::Rebuild()
{
	alGUIElement::Rebuild();
	m_UV[0].Set(0.f, 0.f, 1.f, 1.f);
	m_UV[1].Set(0.f, 0.f, 1.f, 1.f);
	m_UV[2].Set(0.f, 0.f, 1.f, 1.f);
	if (m_textureAtlas)
	{
		m_UV[0] = m_textureAtlas->GetUV(m_iconIndexBase);
		m_UV[1] = m_textureAtlas->GetUV(m_iconIndexMouseHover);
		m_UV[2] = m_textureAtlas->GetUV(m_iconIndexPress);
	}

	if (m_useText && m_text.Size() && m_font)
	{
		m_textPosition.x = m_buildArea.z + m_textIndent.x;
		m_textPosition.y = m_buildArea.y + m_textIndent.y;

		m_textPosition.x += 1;

		float32_t tlen = m_font->GetTextLength(m_text.c_str(), m_text.Size());
		m_clipArea.z += tlen;
		m_sensorArea.z += tlen;
	}
}

void alGUIButtonIcon::SetFont(alGUIFont* font)
{
	m_font = font;
}

void alGUIButtonIcon::SetText(const char32_t* text, ...)
{
	char32_t buf[0xF00];

	va_list args;
	va_start(args, text);
	alLib::vsnprintf(buf, 0xF00, text, args);
	va_end(args);

	m_text.Assign(buf);
}

void alGUIButtonIcon::ClearText()
{
	m_text.Clear();
}

void alGUIButtonIcon::RadioCheck()
{
	if (m_context->m_radioGroups[m_radioGroup].m_activeButton != this)
	{
		if (m_context->m_radioGroups[m_radioGroup].m_activeButton)
		{
			m_context->m_radioGroups[m_radioGroup].m_activeButton->m_isToggleOn = false;
			m_context->m_radioGroups[m_radioGroup].m_activeButton
				->OnButtonToggleOff();
		}
	}

	m_context->m_radioGroups[m_radioGroup].m_activeButton = this;
	m_isToggleOn = true;
}






















