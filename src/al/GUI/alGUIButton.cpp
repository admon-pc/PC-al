#include "al.h"
#include "GUI/alGUI.h"
#include "GS/alGS.h"
#include "GS/alGSTexture.h"
#include "Classes/alColor.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

alGUIButton::alGUIButton(alGUIContext* ct)
	:
	alGUIElement(ct)
{
	m_font = alLib::GetDefaultFont();
	m_currColBG1 = m_colorTheme->m_button_bg1_enabled;
	m_currColBG2 = m_colorTheme->m_button_bg1_enabled;
	m_currColText = m_colorTheme->m_button_textColor_enabled;
}

alGUIButton::~alGUIButton()
{
	AL_DESTROY(m_bgMesh);
	AL_DESTROY(m_bgMeshBorder);
}

void alGUIButton::Draw(float32_t dt)
{
	m_gs->SetScissorRect(m_clipArea);
	OnDraw();

	if (m_drawBG)
	{
		if (m_disabled)
		{
			m_currColBG1 = m_colorTheme->m_button_bg1_disabled;
			m_currColBG1 = m_colorTheme->m_button_bg2_disabled;
			m_currColText = m_colorTheme->m_button_textColor_disabled;
		}
		else
		{
			if (m_toggleButton)
			{
				if (m_isToggleOn)
				{
					m_currColBG1 = m_colorTheme->m_button_bg1_press;
					m_currColBG2 = m_colorTheme->m_button_bg2_press;
					m_currColText = m_colorTheme->m_button_textColor_press;
				}
				else
				{
					m_currColBG1 = m_colorTheme->m_button_bg1_enabled;
					m_currColBG2 = m_colorTheme->m_button_bg2_enabled;
					m_currColText = m_colorTheme->m_button_textColor_enabled;
				}
			}
			else
			{
				if (m_lerpColors)
				{
					float32_t l = 10.f * dt;

					alMath::Lerp1(m_currColBG1, m_colorTheme->m_button_bg1_enabled, l, m_currColBG1);
					alMath::Lerp1(m_currColBG2, m_colorTheme->m_button_bg2_enabled, l, m_currColBG2);
					alMath::Lerp1(m_currColText, m_colorTheme->m_button_textColor_enabled, l, m_currColText);

					if (m_isLMBClicked)
					{
						alMath::Lerp1(m_currColBG1, m_colorTheme->m_button_bg1_press, l, m_currColBG1);
						alMath::Lerp1(m_currColBG1, m_colorTheme->m_button_bg2_press, l, m_currColBG1);
						alMath::Lerp1(m_currColText, m_colorTheme->m_button_textColor_press, l, m_currColText);
					}
					else
					{
						if (m_isCursorInSensorArea)
						{
							alMath::Lerp1(m_currColBG1, m_colorTheme->m_button_bg1_mouseHover, l, m_currColBG1);
							alMath::Lerp1(m_currColBG2, m_colorTheme->m_button_bg2_mouseHover, l, m_currColBG2);
							alMath::Lerp1(m_currColText, m_colorTheme->m_button_textColor_mouseHover, l, m_currColText);
						}
					}
				}
				else
				{
					m_currColBG1 = m_colorTheme->m_button_bg1_enabled;
					m_currColBG2 = m_colorTheme->m_button_bg2_enabled;
					m_currColText = m_colorTheme->m_button_textColor_enabled;

					if (m_isLMBClicked)
					{
						m_currColBG1 = m_colorTheme->m_button_bg1_press;
						m_currColBG2 = m_colorTheme->m_button_bg2_press;
						m_currColText = m_colorTheme->m_button_textColor_press;
					}
					else
					{

						if (m_isCursorInSensorArea)
						{
							m_currColBG1 = m_colorTheme->m_button_bg1_mouseHover;
							m_currColBG2 = m_colorTheme->m_button_bg2_mouseHover;
							m_currColText = m_colorTheme->m_button_textColor_mouseHover;
						}
					}
				}
			}
		}

		if (m_roundCorners && m_bgMesh)
		{
			if(m_borderSize && m_bgMeshBorder)
				m_gs->DrawMeshGUI(m_bgMeshBorder, alVec2f(), m_colorTheme->m_button_border);
			m_gs->DrawMeshGUI(m_bgMesh, alVec2f(), m_currColBG1);
		}
		else
		{
			m_gs->DrawRectangle(
				m_buildArea,
				m_currColBG1,
				m_currColBG2);
		}
	}

	auto textSz = m_text.Size();
	if (m_font && textSz)
	{
		
		m_gs->DrawText(m_text.Data(), textSz, m_font, 
			m_textPosition, m_currColText);
	}
}

void alGUIButton::Update(float32_t dt)
{
	bool isLMBClick = m_isLMBClicked;
	bool isRMBClick = m_isRMBClicked;
	bool isMMBClick = m_isMMBClicked;

	alGUIElement::Update(dt);
	
	if (m_isCursorInSensorArea)
	{
		if (m_toggleButton)
		{
			if (m_isLMBClicked && m_input->m_isLMBDown)
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
		else
		{
			if (isLMBClick && (!m_isLMBClicked))
			{
				OnButtonRelease();
			}
		}
		/*else if (isRMBClick && (!m_isRMBClicked))
		{
			OnButtonRelease(this);
		}
		else if (isMMBClick && (!m_isMMBClicked))
		{
			OnButtonRelease(this);
		}*/

	
	}
	else
	{
	}
}

void alGUIButton::Rebuild()
{
	alGUIElement::Rebuild();

	m_textPosition.x = m_buildArea.x;
	m_textPosition.y = m_buildArea.y;

	if (m_textInCenter)
	{
		auto textSz = m_text.Size();
		if (m_font && textSz)
		{
			auto textW = m_font->GetTextLength(m_text.Data(), textSz);
			auto textH = m_font->m_maxHeight;

			alVec2f buttonHalfSize;
			buttonHalfSize.x = ((m_buildArea.z - m_buildArea.x) * 0.5f);
			buttonHalfSize.y = ((m_buildArea.w - m_buildArea.y) * 0.5f);

			m_textPosition.x += buttonHalfSize.x - textW * 0.5f;
			m_textPosition.y += buttonHalfSize.y - textH * 0.5f;
		}
	}
	else
	{
		
	}

	if (m_roundCorners)
	{
		AL_DESTROY(m_bgMesh);
		AL_DESTROY(m_bgMeshBorder);

		auto ba = m_buildArea;
		if (m_borderSize)
		{
			ba.x += m_borderSize;
			ba.y += m_borderSize;
			ba.z -= m_borderSize;
			ba.w -= m_borderSize;
		}

		m_bgMesh = m_context->_create_bg_mesh(ba, m_roundSize,
		m_roundIterations, 0);

		if (m_borderSize)
			m_bgMeshBorder = m_context->_create_bg_mesh(m_buildArea, m_roundSize,
			m_roundIterations, 0);
	}
}

void alGUIButton::SetFont(alGUIFont* font)
{
	m_font = font;
}

void alGUIButton::SetText(const char32_t* text, ...)
{
	char32_t buf[0xF00];

	va_list args;
	va_start(args, text);
	alLib::vsnprintf(buf, 0xF00, text, args);
	va_end(args);

	m_text.Assign(buf);
}

void alGUIButton::ClearText()
{
	m_text.Clear();
}





























