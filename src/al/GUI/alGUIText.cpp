#include "al.h"
#include "GUI/alGUI.h"
#include "GS/alGS.h"
#include "GS/alGSTexture.h"
#include "Classes/alColor.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

alGUIText::alGUIText(alGUIContext* ct)
	:
	alGUIElement(ct)
{
	m_font = alLib::GetDefaultFont();
}

alGUIText::~alGUIText()
{
}

void alGUIText::Draw(float32_t dt)
{
	OnDraw();
	auto textSz = m_text.Size();
	if (m_font && textSz)
	{
		m_gs->SetScissorRect(m_clipArea);
		//m_gs->DrawRectangle(m_clipArea, ColorRed, ColorRed);
		m_gs->DrawText(m_text.Data(), textSz, m_font,
			alVec2f(m_buildArea.x, m_buildArea.y),
			m_colorTheme->m_text_textColor);
	}
}

void alGUIText::Update(float32_t dt)
{
	alGUIElement::Update(dt);
}

void alGUIText::Rebuild()
{
	auto textSz = m_text.Size();
	if (m_font && textSz)
	{
		m_size.x = m_font->GetTextLength(m_text.Data(), textSz);
		m_size.y = m_font->m_maxHeight;
	}

	alGUIElement::Rebuild();
}


void alGUIText::SetFont(alGUIFont* font)
{
	m_font = font;
}

void alGUIText::SetText(const char32_t* text, ...)
{
	char32_t buf[0xF00];

	va_list args;
	va_start(args, text);
	alLib::vsnprintf(buf, 0xF00, text, args);
	va_end(args);

	m_text.Assign(buf);
}

void alGUIText::Clear()
{
	m_text.Clear();
}

