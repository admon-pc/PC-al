#include "al.h"
#include "GUI/alGUI.h"
#include "GS/alGS.h"
#include "GS/alGSTexture.h"
#include "Classes/alColor.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

alGUITextInput::alGUITextInput(alGUIContext* ct)
	:
	alGUIElement(ct)
{
	m_font = alLib::GetDefaultFont();
	m_colorDefaultText = 0xFFAAAAAA;
	
	m_isEditActive = false;
	Update(0.f);
	_updateData();
}

alGUITextInput::~alGUITextInput()
{
	if (m_context->m_inputTextElementActivated == this)
		m_context->m_inputTextElementActivated = 0;

	Clear();
}

void alGUITextInput::Draw(float32_t dt)
{
	auto input = alLib::GetInput();
	m_gs->SetScissorRect(m_clipArea);
	if (m_drawBG)
	{
		alColor bgColor1 = m_colorTheme->m_edit_bg1NA;
		alColor bgColor2 = m_colorTheme->m_edit_bg2NA;

		if (m_isEditActive)
		{
			bgColor1 = m_colorTheme->m_edit_bg1;
			bgColor2 = m_colorTheme->m_edit_bg2;
		}

		m_gs->DrawRectangle(
			m_buildArea,
			bgColor1,
			bgColor2);
	}

	OnDraw();

	if (m_useBottombar)
	{
		m_gs->DrawRectangle(
			m_bottomBarBG,
			m_colorTheme->m_edit_bottomBarBG,
			m_colorTheme->m_edit_bottomBarBG);
		{
			char32_t buf[100];
			int sz = alLib::sprintf(buf, U"Ln:%u", m_textCursorLine);
			float32_t offset = 0.f;
			m_gs->DrawText(
				buf, sz,
				m_fontUI,
				alVec2f(m_editorArea.z - m_hscrollRightSideIndent,
					m_editorArea.w - 14),
				m_colorTheme->m_edit_bottomBarUIText);
			offset = 50;
			sz = alLib::sprintf(buf, U"Col:%u", m_textCursorCol);
			m_gs->DrawText(
				buf, sz,
				m_fontUI,
				alVec2f(m_editorArea.z - m_hscrollRightSideIndent
					+ offset,
					m_editorArea.w - 14),
				m_colorTheme->m_edit_bottomBarUIText);
		}
	}

	if (m_useLinebar)
	{
		m_gs->DrawRectangle(m_lineBar,
			m_colorTheme->m_edit_lineBarBG,
			m_colorTheme->m_edit_lineBarBG);
	}

	bool isTextCursorWasVisible = false;

	alVec2f fontSize(m_font->m_maxWidth, m_font->m_maxHeight);
	auto* text = &m_text;
	//if (text->Size())
	{
		if (m_scrollV.m_needToDraw)
		{
			m_gs->DrawRectangle(m_scrollV.m_rectBG, 
				m_colorTheme->m_edit_scrollBG);
			m_gs->DrawRectangle(m_scrollV.m_btn1Rect, 
				m_colorTheme->m_edit_scrollButtonBG);
			/*m_gs->DrawImage(
				npVec2f(2 + m_scrollV.m_btn1Rect.x, 
					2 + m_scrollV.m_btn1Rect.y),
				npVec2i(0, 0),
				npVec2i(9, 5),
				g_app->m_GUITexture);*/
			m_gs->DrawRectangle(m_scrollV.m_btn2Rect, 
				m_colorTheme->m_edit_scrollButtonBG);
			/*m_gs->DrawImage(
				npVec2f(2 + m_scrollV.m_btn2Rect.x, 
					2 + m_scrollV.m_btn2Rect.y),
				npVec2i(9, 0),
				npVec2i(9, 5),
				g_app->m_GUITexture);*/
			m_gs->DrawRectangle(m_scrollV.m_rectangle, 
				m_colorTheme->m_edit_scrollRect);
		}
		if (m_scrollH.m_needToDraw)
		{
			m_gs->DrawRectangle(m_scrollH.m_rectBG,
				m_colorTheme->m_edit_scrollBG,
				m_colorTheme->m_edit_scrollBG);
			m_gs->DrawRectangle(m_scrollH.m_btn1Rect, 
				m_colorTheme->m_edit_scrollButtonBG);
			/*m_gs->DrawImage(
				npVec2f(2 + m_scrollH.m_btn1Rect.x, 2 + m_scrollH.m_btn1Rect.y),
				npVec2i(18, 0),
				npVec2i(5, 9),
				g_app->m_GUITexture);*/
			m_gs->DrawRectangle(m_scrollH.m_btn2Rect,
				m_colorTheme->m_edit_scrollButtonBG);
			/*m_gs->DrawImage(
				npVec2f(2 + m_scrollH.m_btn2Rect.x, 2 + m_scrollH.m_btn2Rect.y),
				npVec2i(23, 0),
				npVec2i(5, 9),
				g_app->m_GUITexture);*/
			m_gs->DrawRectangle(m_scrollH.m_rectangle,
				m_colorTheme->m_edit_scrollRect);
		}

		uint64_t selminimum = m_selectionStart;
		uint64_t selmaximum = m_selectionEnd;
		if (m_selectionStart > m_selectionEnd)
		{
			selminimum = m_selectionEnd;
			selmaximum = m_selectionStart;
		}
		float carriagePosition = 0.f;
		float rowPosition = 0.f;

		size_t charCol = 1;
		size_t charColAdd = 1;
		m_tabCounter = 0;
		m_drawLastIndex = -1;
		size_t char_underMouse = -1;
		size_t char_closestMouse = -1;
		size_t char_closestOnLineMouse = -1;
		size_t char_closestOnFirstLineMouse = -1;
		size_t char_closestOnLastLineMouse = -1;
		float32_t distanceClosest = FLT_MAX;
		float32_t distanceClosestOnLine = FLT_MAX;
		float32_t distanceClosestOnFirstLine = FLT_MAX;
		float32_t distanceClosestOnLastLine = FLT_MAX;
		alVec4f lastDrawnCharRect;
		auto lineCounter = m_drawStartIndexLine;
		auto textPtr = text->c_str();
		auto textDataPtr = m_textData.c_str();
		auto textArraySize = text->Size() + 1;
		size_t charIndex = m_drawStartIndex;
		size_t lastCharIndex = text->Size() - 1;
		m_isLastCharVisible = false;
		for (size_t i = 0; i < m_numOfVisibleLines; )
		{
			if (charIndex == lastCharIndex)
			{
				m_isLastCharVisible = true;
			}

			char32_t currChar = textPtr[charIndex];
			char32_t currCharData = textDataPtr[charIndex];

			alVec4f charRect;
			charRect.x = m_editorAreaText.x + carriagePosition;
			charRect.y = m_editorAreaText.y + rowPosition;
			charRect.x -= m_horizontal_offset;

			charRect.z = charRect.x + fontSize.x;
			charRect.w = charRect.y + fontSize.y;

			float32_t charRectHalfLen = (charRect.z - charRect.x) * 0.5f;
			alVec4f charRectForPick = charRect;
			charRectForPick.x -= charRectHalfLen;
			charRectForPick.z -= charRectHalfLen;
			auto centerX = charRect.x + ((charRect.z - charRect.x) * 0.5f);
			auto centerY = charRect.y + ((charRect.w - charRect.y) * 0.5f);

			if (m_useLinebar)
			{
				if (charCol == 1)
				{
					auto charNum = alLib::sprintf(m_lineBarCharBuffer, U"%u", lineCounter);

					m_gs->SetScissorRect(m_lineBar);
					float32_t offset = 0.f;
					for (size_t k = charNum - 1; k >= 0; )
					{
						m_gs->DrawText(
							&m_lineBarCharBuffer[k],
							1,
							m_font,
							alVec2f(m_lineBar.z - fontSize.x - fontSize.x - offset,
								charRect.y),
							m_colorTheme->m_edit_lineBarText);

						offset += m_nextCharPositionOffset;
						if (!k)
							break;
						--k;
					}
				}
			}

			m_gs->SetScissorRect(m_editorAreaText);
			if (lineCounter == m_textCursorLine && charCol == 1)
			{
				alVec4f rect;
				rect.x = m_editorArea.x;
				rect.y = charRect.y;
				rect.z = m_editorArea.z;
				rect.w = charRect.w;
				m_gs->DrawRectangle(rect, 
					m_colorTheme->m_edit_currlineBG,
					m_colorTheme->m_edit_currlineBG);
			}
			alColor textColor = m_colorTheme->m_edit_textEnabled;
			if (m_isSelected)
			{
				if (charIndex >= selminimum && charIndex < selmaximum)
				{
					alVec4f rect;
					rect.x = charRect.x;
					rect.y = charRect.y;
					rect.z = rect.x;
					rect.w = rect.y;
					rect.z += m_nextCharPositionOffset;
					rect.w += m_nextLinePositionOffset;

					if (currChar == '\t')
					{
						for (size_t o = m_tabCounter + 1; o < m_tabSize; ++o)
						{
							rect.z += m_nextCharPositionOffset;
						}
					}

					m_gs->DrawRectangle(rect, 
						m_colorTheme->m_edit_selectbg);

					textColor = m_colorTheme->m_edit_textSelected;
				}
			}

			switch (currCharData & 0x700)
			{
			case 0x100:
				break;
			case 0x200:
				textColor = npDocumentTextColors_light[3];
				break;
			default:
			case 0x300:
				textColor = npDocumentTextColors_light[0];
				break;
			case 0x400:
				textColor = npDocumentTextColors_light[1];
				break;
			}


			m_gs->DrawText(&currChar,
				1,
				m_font,
				alVec2f(charRect.x, charRect.y),
				textColor);

			if (alMath::PointInRect(input->m_cursorCoordsForGUI.x,
				input->m_cursorCoordsForGUI.y, charRectForPick))
			{
				char_underMouse = charIndex;
				//	char_underMouse_rect = charRect;
			}
			auto d = alMath::Distance2d(centerX, centerY,
				input->m_cursorCoordsForGUI.x,
				input->m_cursorCoordsForGUI.y);
			if (d < distanceClosest)
			{
				distanceClosest = d;
				char_closestMouse = charIndex;
			}
			if (input->m_cursorCoordsForGUI.y >= charRect.y
				&& input->m_cursorCoordsForGUI.y < charRect.w)
			{
				if (d < distanceClosestOnLine)
				{
					distanceClosestOnLine = d;
					char_closestOnLineMouse = charIndex;
				}
			}
			if (d < distanceClosestOnLastLine)
			{
				distanceClosestOnLastLine = d;
				char_closestOnLastLineMouse = charIndex;
			}
			if (i == 0)
			{
				if (d < distanceClosestOnFirstLine)
				{
					distanceClosestOnFirstLine = d;
					char_closestOnFirstLineMouse = charIndex;
				}
			}
			if (currChar == L'\n')
			{
				distanceClosestOnLastLine = FLT_MAX;
			}

			if (charIndex == m_textCursorPosition)
			{
				isTextCursorWasVisible = true;
				m_textCursorRect.x = charRect.x;
				m_textCursorRect.y = charRect.y;
				m_textCursorRect.z = m_textCursorRect.x + 1;
				m_textCursorRect.w = m_textCursorRect.y + fontSize.y;
			}



			if (currChar == '\t')
			{
				charColAdd = 0;
				for (size_t o = m_tabCounter; o < m_tabSize; ++o)
				{
					carriagePosition += m_nextCharPositionOffset;
					++charColAdd;
				}
				m_tabCounter = 0;
			}
			else
			{
				carriagePosition += m_nextCharPositionOffset;

				++m_tabCounter;
				if (m_tabCounter == m_tabSize)
					m_tabCounter = 0;
			}

			if (m_textCursorPosition == charIndex)
			{
				m_textCursorCol = charCol;
				m_textCursorLine = lineCounter;
				if (m_getCursorColPrefer)
				{
					m_getCursorColPrefer = false;
					m_textCursorCol_prefer = m_textCursorCol;
				}
			}
			charCol += charColAdd;
			charColAdd = 1;

			if (currChar == L'\n')
			{
				m_tabCounter = 0;
				++lineCounter;
				charCol = 1;
				carriagePosition = 0.f;
				if(!m_oneLine)
					rowPosition += m_nextLinePositionOffset;
				++i;
			}

			m_drawLastIndex = charIndex;
			lastDrawnCharRect = charRect;

			++charIndex;
			if (charIndex == textArraySize)
				break;
		}

		if (m_find_horizontal_offset)
		{
			m_find_horizontal_offset = false;
			if (m_textCursorRect.z > m_editorAreaText.z)
			{
				m_horizontal_offset += m_textCursorRect.z - m_editorAreaText.z;
			}
			else if (m_textCursorRect.x < m_editorAreaText.x)
			{
				m_horizontal_offset -= m_editorAreaText.x - m_textCursorRect.x;
			}

			// Если выделенный текст при поиске находится за левой границей области
			// то нужно сдвинуть текст вправо. на сколько? Не знаю. 
			// Можно примерно поделить пополам ширину области и сместить на поолученное значение.
			// Предположительно в 99% должно всё работать корректно. Оставшийся 1 процента это возможно,
			// если результат поиска был на жутко длинной строке.
			if (m_textFindMode)
			{
				m_textFindMode = false;
				if (m_textCursorRect.x < m_editorAreaText.x + fontSize.x)
					m_horizontal_offset -= (m_editorAreaText.z - m_editorAreaText.x) * 0.5f;
			}

			if (m_horizontal_offset < 0)
				m_horizontal_offset = 0;
			//	printf("m_horizontal_offset %f\n", m_horizontal_offset);
		}

		//	printf("m_textCursorCol_prefer %u\n", m_textCursorCol_prefer);
		if (char_underMouse != -1)
		{
			m_charForMousePick = char_underMouse;
		}
		else
		{
			if (char_closestOnLineMouse != -1)
			{
				m_charForMousePick = char_closestOnLineMouse;
			}
			else
			{
				if (input->m_cursorCoordsForGUI.y >= lastDrawnCharRect.y)
				{
					m_charForMousePick = char_closestOnLastLineMouse;
				}
				else
				{
					m_charForMousePick = char_closestOnFirstLineMouse;
				}
			}
		}
	}
	/*else
	{
		isTextCursorWasVisible = true;
		m_textCursorRect.x = m_editorAreaText.x;
		m_textCursorRect.y = m_editorAreaText.y;
		m_textCursorRect.z = m_textCursorRect.x + 1;
		m_textCursorRect.w = m_textCursorRect.y + fontSize.y;
	}*/

	if (!text->Size() && m_textDefault.size())
	{
		m_gs->DrawText(
			m_textDefault.c_str(),
			m_textDefault.size(),
			m_font,
			alVec2f(m_editorAreaText.x, m_editorAreaText.y),
			m_colorTheme->m_edit_textDefault);
	}


	if (m_isEditActive && m_drawTextCursor && isTextCursorWasVisible)
	{
		m_gs->DrawRectangle(m_textCursorRect, 
			m_colorTheme->m_edit_textCursor);
	}
	//****************
}

void alGUITextInput::_updateRects()
{
	auto fontSize = m_font->GetFontSize();
	m_nextCharPositionOffset = fontSize.x;
	m_nextLinePositionOffset = fontSize.y;
	m_editorArea = m_buildArea;

	float32_t lineBarWidth = 0.f;

	if (m_useLinebar)
	{
		lineBarWidth = fontSize.x + fontSize.x;
		auto lineBar_numLines = m_drawStartIndexLine + m_numOfVisibleLines;
		//auto lineBar_numLines = m_lineNum;
		if (lineBar_numLines < 100)
			lineBarWidth += fontSize.x;
		else if (lineBar_numLines < 1000)
			lineBarWidth += fontSize.x * 2;
		else if (lineBar_numLines < 10000)
			lineBarWidth += fontSize.x * 3;
		else if (lineBar_numLines < 100000)
			lineBarWidth += fontSize.x * 4;
		else if (lineBar_numLines < 1000000)
			lineBarWidth += fontSize.x * 5;
		else if (lineBar_numLines < 10000000)
			lineBarWidth += fontSize.x * 6;
		else if (lineBar_numLines < 100000000)
			lineBarWidth += fontSize.x * 7;
		lineBarWidth += fontSize.x;

	}
		m_lineBar.x = m_editorArea.x;
		m_lineBar.y = m_editorArea.y;
		m_lineBar.z = m_lineBar.x + lineBarWidth;
		m_lineBar.w = m_editorArea.w;

	if (m_useBottombar)
	{
		m_bottomBarBG.x = m_editorArea.x;
		m_bottomBarBG.y = m_editorArea.w - m_bottomBarHeight;
		m_bottomBarBG.z = m_editorArea.z;
		m_bottomBarBG.w = m_editorArea.w;
	}
	else
	{
		m_bottomBarBG.Set(0.f);
	}

	m_editorAreaText = m_editorArea;
	m_editorAreaText.x = m_lineBar.z;
	m_editorAreaText.w -= m_bottomBarBG.w - m_bottomBarBG.y;
	m_textAreaHeight = m_editorAreaText.w - m_editorAreaText.y;
}
void alGUITextInput::Update(float32_t dt)
{
	//if (m_context->m_inputTextElementActivated && m_context->m_inputTextElementActivated != this)
	//	return;

	alGUIElement::Update(dt);
	auto input = alLib::GetInput();

	m_2clickTimer += dt;
	if (m_2clickTimer > 0.4f)
	{
		m_2clickTimer = 0.f;
		m_2clickCount = 0;
	}
	m_3clickTimer += dt;
	if (m_3clickTimer > 0.4f)
	{
		m_3clickTimer = 0.f;
		m_3clickCount = 0;
	}
	alVec2f fontSize(m_font->m_maxWidth, m_font->m_maxHeight);
	_updateRects();


	if (m_useVerticalScrollbar)
	{
		m_scrollV.m_needToDraw = true;
		if (m_drawStartIndexLine > 1)
			m_scrollV.m_needToDraw = true;
	}
	else
	{
		m_scrollV.m_needToDraw = false;
	}

	if (alMath::PointInRect(input->m_cursorCoordsForGUI.x,
		input->m_cursorCoordsForGUI.y, m_scrollV.m_rectBG))
	{
		if (input->m_isLMBDown && input->m_isLMBDown_window)
		{
			m_scrollV.m_cursorHold = true;
			m_scrollV.m_saveOnClick = m_scrollV.m_rectangle.y - m_scrollV.m_btn1Rect.w;
			m_scrollV.m_saveOnClickMouseCoord = input->m_cursorCoordsForGUI.y - m_scrollV.m_btn1Rect.w;
		}
	}
	else if (alMath::PointInRect(input->m_cursorCoordsForGUI.x,
		input->m_cursorCoordsForGUI.y, m_scrollH.m_rectBG))
	{
		if (input->m_isLMBDown && input->m_isLMBDown_window)
		{
			m_scrollH.m_cursorHold = true;
			m_scrollH.m_saveOnClick = m_scrollH.m_rectangle.x - m_scrollH.m_btn1Rect.z;
			m_scrollH.m_saveOnClickMouseCoord = input->m_cursorCoordsForGUI.x - m_scrollH.m_btn1Rect.z;
		}
	}

	if (m_scrollV.m_needToDraw)
	{
		m_scrollV.m_rectBG.x = m_editorArea.z - m_scrollV.m_size;
		m_scrollV.m_rectBG.y = m_editorArea.y;
		m_scrollV.m_rectBG.z = m_editorArea.z;
		m_scrollV.m_rectBG.w = m_editorArea.w;
		m_scrollV.m_btn1Rect.x = m_scrollV.m_rectBG.x;
		m_scrollV.m_btn1Rect.y = m_scrollV.m_rectBG.y;
		m_scrollV.m_btn1Rect.z = m_scrollV.m_rectBG.z;
		m_scrollV.m_btn1Rect.w = m_scrollV.m_btn1Rect.y + 10.f;
		m_scrollV.m_btn2Rect.x = m_scrollV.m_rectBG.x;
		m_scrollV.m_btn2Rect.y = m_scrollV.m_rectBG.w - 10.f;
		m_scrollV.m_btn2Rect.z = m_scrollV.m_rectBG.z;
		m_scrollV.m_btn2Rect.w = m_scrollV.m_rectBG.w;
		m_scrollV.m_rectangle = m_scrollV.m_rectBG;
		m_scrollV.m_rectangle.y = m_scrollV.m_btn1Rect.w;
		m_scrollV.m_rectangle.w = m_scrollV.m_btn2Rect.y;
		m_scrollV.m_rectangle.x += 2.f;
		m_scrollV.m_rectangle.z -= 2.f;
		float32_t d = alMath::Normalize((float32_t)m_lineNum + m_numOfVisibleLines,
			(float32_t)m_numOfVisibleLines);
		float32_t rectHeight = m_scrollV.m_rectangle.w - m_scrollV.m_rectangle.y;
		m_scrollV.m_rectangle.w = m_scrollV.m_rectangle.y + (rectHeight * d);
		// нельзя слишком маленький
		rectHeight = m_scrollV.m_rectangle.w - m_scrollV.m_rectangle.y;
		if (rectHeight < 30.f)
			m_scrollV.m_rectangle.w = m_scrollV.m_rectangle.y + 30.f;
		rectHeight = m_scrollV.m_rectangle.w - m_scrollV.m_rectangle.y;
		// мне нужно вычислить чему равно перемещение скролл прямоугольника
		// на 1 пиксель. какому количеству m_vertical_offset.
		// и наоборот, зная m_vertical_offset нужно правильно вычислить
		// количество пикселей.

		m_editorAreaText.z -= m_scrollV.m_size;

		// количество пикселей после прямоугольника.
		// до нижней кнопки
		float32_t emptySpaceInPixels = m_scrollV.m_btn2Rect.y - m_scrollV.m_rectangle.w;
		auto vscrollRectangleSave = m_scrollV.m_rectangle;
		float32_t ntext = alMath::Normalize((float32_t)m_lineNum, (float32_t)m_drawStartIndexLine);
		// m_textHeight даёт высоту всего текста в пикселях
		float32_t pixels = emptySpaceInPixels * ntext;
		m_scrollV.m_rectangle.y += pixels;
		m_scrollV.m_rectangle.w += pixels;

		if (m_scrollV.m_cursorHold && (input->m_mouseDelta.y != 0.f))
		{
			float32_t ycoord = input->m_cursorCoordsForGUI.y - m_scrollV.m_btn1Rect.w;
			float32_t coord_offset = ycoord - m_scrollV.m_saveOnClickMouseCoord;
			pixels = m_scrollV.m_saveOnClick + coord_offset;
			float32_t nscroll = alMath::Normalize(emptySpaceInPixels, pixels);
			//printf("%f\n", (float32_t)m_lineNum*nscroll);
			float32_t line = floorf((float32_t)m_lineNum * nscroll);
			if (line < 0.f)
				line = 0.f;
			_moveViewToLine((uint32_t)line);
		}
	}

	if (m_scrollH.m_needToDraw)
	{
		m_scrollH.m_rectBG.x = m_editorArea.x;
		m_scrollH.m_rectBG.y = m_editorArea.w - m_scrollH.m_size;
		m_scrollH.m_rectBG.z = m_editorArea.z - m_hscrollRightSideIndent;
		m_scrollH.m_rectBG.w = m_editorArea.w;
		m_scrollH.m_btn1Rect.x = m_scrollH.m_rectBG.x;
		m_scrollH.m_btn1Rect.y = m_scrollH.m_rectBG.y;
		m_scrollH.m_btn1Rect.z = m_scrollH.m_btn1Rect.x + 10.f;
		m_scrollH.m_btn1Rect.w = m_scrollH.m_rectBG.w;
		m_scrollH.m_btn2Rect.x = m_scrollH.m_rectBG.z - 10.f;
		m_scrollH.m_btn2Rect.y = m_scrollH.m_rectBG.y;
		m_scrollH.m_btn2Rect.z = m_scrollH.m_rectBG.z;
		m_scrollH.m_btn2Rect.w = m_scrollH.m_rectBG.w;
		m_scrollH.m_rectangle.x = m_scrollH.m_btn1Rect.z;
		m_scrollH.m_rectangle.y = m_scrollH.m_btn1Rect.y;
		m_scrollH.m_rectangle.z = m_scrollH.m_btn2Rect.x;
		m_scrollH.m_rectangle.w = m_scrollH.m_btn2Rect.w;
		m_scrollH.m_rectangle.y += 2.f;
		m_scrollH.m_rectangle.w -= 2.f;

		// вначале ползунок имеет размер во всю ширину.
		// Когда в ширину символы выходят за определённую границу
		// этот ползунок нужно укоротить, чтобы можно было сдвинуть текст.
		// .z координата равняется .x + (укороченная ширина)

		// ширина текстовой области
		auto textAreaWidth = m_editorAreaText.z - m_editorAreaText.x;
		// максимальная ширина текста + некое значение лимита
		// чтобы прокрутить хотяб немного
		float32_t maxPixelsWidth = 100.f + m_maxLineWidth;
		// преобразование в подобие процента, от 0 до 1
		float32_t d = alMath::Normalize(maxPixelsWidth, textAreaWidth);
		float32_t rectWidth = m_scrollH.m_rectangle.z - m_scrollH.m_rectangle.x;
		m_scrollH.m_rectangle.z = m_scrollH.m_rectangle.x + (rectWidth * d);

		// сдвиг ползунка используя текущее значение горизонтального смещения
		float32_t emptySpaceInPixels = m_scrollH.m_btn2Rect.x - m_scrollH.m_rectangle.z;
		float32_t ntext = alMath::Normalize(m_hscroll_widthLen, m_horizontal_offset);
		float32_t pixels = emptySpaceInPixels * ntext;
		m_scrollH.m_rectangle.x += pixels;
		m_scrollH.m_rectangle.z += pixels;

		// двигаем мышкой
		if (m_scrollH.m_cursorHold && (input->m_mouseDelta.x != 0.f))
		{
			float32_t xcoord = input->m_cursorCoordsForGUI.x - m_scrollH.m_btn1Rect.z;
			float32_t coord_offset = xcoord - m_scrollH.m_saveOnClickMouseCoord;
			pixels = m_scrollH.m_saveOnClick + coord_offset;
			float32_t nscroll = alMath::Normalize(emptySpaceInPixels, pixels);

			auto ww = maxPixelsWidth;
			m_horizontal_offset = m_hscroll_widthLen * nscroll;
			if (m_horizontal_offset > m_hscroll_widthLen)
				m_horizontal_offset = m_hscroll_widthLen;
			if (m_horizontal_offset < 0.f)
				m_horizontal_offset = 0.f;
			//printf("%f\n", m_horizontal_offset);
		}
	}

	// для прокрутки колесом мышки используем отдельный if
	if (m_isEditActive)
	{
		if (alMath::PointInRect(input->m_cursorCoordsForGUI.x,
			input->m_cursorCoordsForGUI.y, m_editorArea))
		{


			if (input->m_wheelDelta &&
				input->m_kbm != alKeyboardModifier::Ctrl)
			{
				if (input->m_wheelDelta > 0.f)
					_moveUpView(3);
				if (input->m_wheelDelta < 0.f)
					_moveDownView(3);
			}
		}
	}

	if (alMath::PointInRect(input->m_cursorCoordsForGUI.x,
		input->m_cursorCoordsForGUI.y, m_editorAreaText))
	{
		if (input->m_isLMBDown)
		{
			if (!m_isEditActive)
			{
				Activate();
			}
		}

		if (!m_cursorInTextEditor)
		{
			m_cursorInTextEditor = true;
		}
		m_context->m_cursorType = alCursorType::IBeam;

		if (input->m_wheelDelta && !m_oneLine)
		{
			if (input->m_kbm == alKeyboardModifier::Ctrl)
			{
				if (input->m_wheelDelta > 0.f)
					m_fontEditorSize += 1;
				if (input->m_wheelDelta < 0.f)
					m_fontEditorSize -= 1;
				if (m_fontEditorSize < 6)
					m_fontEditorSize = 6;
				if (m_fontEditorSize > 26)
					m_fontEditorSize = 26;

				/*if (m_fontEditor)
					delete m_fontEditor;
				m_fontEditor = m_renderer->CreateNPFont("Consolas", false, false, m_fontEditorSize);*/
				fontSize = m_font->GetFontSize();
				m_nextCharPositionOffset = fontSize.x;
				m_nextLinePositionOffset = fontSize.y;
				_updateData();
			}
		}

		if (input->m_isLMBDown && input->m_isLMBDown_window)
		{
			if (!m_clickedLMB)
				m_clickedLMB = true;
		}
		if (input->m_isRMBUp && m_isEditActive)
		{
			_showTextEditPopup();
			/*g_app->m_popupTextEditorPopup->Show(
				(uint32_t)input->m_cursorCoordsForGUI.x,
				(uint32_t)input->m_cursorCoordsForGUI.y);*/
		}
	}
	else
	{
		if (m_cursorInTextEditor)
		{
			m_cursorInTextEditor = false;
		}


	}

	if (input->m_isLMBUp)
	{
		m_textMouseScroll = false;
		m_clickedLMB = false;
		m_scrollV.m_cursorHold = false;
		m_scrollH.m_cursorHold = false;
		input->m_isLMBHold = false;
	}

	if (input->m_isRMBDown && !m_clickedLMB)
	{
		_showTextCursor();
		m_textCursorPosition = m_charForMousePick;
		_findHOffsetFromTextCursorPosition();
		m_history.AddUndo();
	}

	if (m_isEditActive && m_clickedLMB)
	{
		//m_charForMousePick
		auto fontSize = m_font->GetFontSize();
		bool mouseMove = false;
		if (input->m_mouseDelta.x != 0.f || input->m_mouseDelta.y != 0.f)
			mouseMove = true;

		if (m_charForMousePick != -1)
		{
			auto textCursorBeforeClick = m_textCursorPosition;
			if ((input->m_isLMBHold && mouseMove) || (input->m_isLMBDown && input->m_isLMBDown_window))
			{
				_showTextCursor();
				m_textCursorPosition = m_charForMousePick;
				_findHOffsetFromTextCursorPosition();

				m_history.AddUndo();

				if (input->m_kbm != alKeyboardModifier::Shift)
				{
					DeselectAll();
					if (input->m_isLMBDown && input->m_isLMBDown_window)
					{
						m_textCursorPositionOnClick = m_textCursorPosition;
						m_getCursorColPrefer = true;
					}
				}

				if (mouseMove)
				{
					if (!m_isSelected)
					{
						m_isSelected = true;
						m_selectionStart = m_textCursorPositionOnClick;
					}
					m_selectionEnd = m_textCursorPosition;
					m_getCursorColPrefer = true;
				}
				else
				{
					if (input->m_kbm == alKeyboardModifier::Shift)
					{
						if (m_isSelected)
						{
							m_selectionEnd = m_textCursorPosition;
						}
						else
						{
							m_isSelected = true;
							m_selectionStart = textCursorBeforeClick;
							m_selectionEnd = m_textCursorPosition;
						}
					}
				}
			}
			

		}

		if (input->m_isLMBDown)
		{
			if (m_textCursorPosition != m_textCursorPosOnClick)
			{
				m_2clickCount = 0;
				m_3clickCount = 0;
			}

			++m_2clickCount;
			m_2clickTimer = 0.f;
			++m_3clickCount;
			m_3clickTimer = 0.f;

			m_textCursorPosOnClick = m_textCursorPosition;
		}
		if (m_3clickCount == 3)
		{
			if (m_charForMousePick != -1)
			{
				m_3clickCount = 0;
				//printf("Select\n");
				Select(
					_moveIndexIntoCol1(m_textCursorPosition),
					_moveIndexIntoEndOfTheLine(m_textCursorPosition));
				m_textCursorPosition = m_selectionStart;
				m_textCursorPositionOnClick = m_selectionStart;
				_findHOffsetFromTextCursorPosition();
			}
		}
		else if (m_2clickCount == 2)
		{
			if (m_charForMousePick != -1)
			{
				m_2clickCount = 0;
				//printf("Select\n");
				//m_textData[m_charForMousePick]
				WordInfo wi;
				GetWordInfo(m_textCursorPosition, &wi);

				Select(wi.m_begin, wi.m_end);
				m_textCursorPosition = m_selectionEnd;
				m_textCursorPositionOnClick = m_selectionStart;
				_findHOffsetFromTextCursorPosition();
			}
		}

		if (!m_oneLine)
		{
			// если курсор мышки за пределами области (выше или ниже)
			// то крутим текст
			m_textMouseScroll = false;
			m_textMouseScrollTimerLimit = 0.1f;
			float32_t d = 0.f;
			if (input->m_cursorCoordsForGUI.y < m_editorAreaText.y)
			{
				m_textMouseScroll = true;
				m_textMouseScrollUp = true;

				d = m_editorAreaText.y - input->m_cursorCoordsForGUI.y;
			}
			else if (input->m_cursorCoordsForGUI.y > m_editorAreaText.w)
			{
				m_textMouseScroll = true;
				m_textMouseScrollUp = false;
				d = input->m_cursorCoordsForGUI.y - m_editorAreaText.w;
			}

			if (d > 200.f)
				m_textMouseScrollTimerLimit = 0.001f;
			else if (d > 100.f)
				m_textMouseScrollTimerLimit = 0.01f;
			else if (d > 50.f)
				m_textMouseScrollTimerLimit = 0.05f;
			else if (d > 20.f)
				m_textMouseScrollTimerLimit = 0.08f;
		}
	}
	//if (m_isSelected)
	//{
	//	printf("s");
	//}
	if (m_isEditActive)
	{
		//	printf("limit %f\n", m_textMouseScrollTimerLimit);
		if (m_textMouseScroll && !m_oneLine)
		{
			m_textMouseScrollTimer += dt;
			if (m_textMouseScrollTimer > m_textMouseScrollTimerLimit)
			{
				m_textMouseScrollTimer = 0.f;
				if (m_textMouseScrollUp)
				{
					_moveUpView(1);
				}
				else
				{
					if (!m_isLastCharVisible)
						_moveDownView(1);
				}
			}
		}

		m_textCursorTimer += dt;
		if (m_textCursorTimer > m_textCursorTimerLimit)
		{
			m_textCursorTimer = 0.f;
			m_drawTextCursor = m_drawTextCursor ? false : true;
		}

		auto* text = &m_text;
		auto text_size = text->size();
		if (text_size)
		{
			if (input->IsKeyHit(alInputKey::K_LEFT))
				_goLeft();
			else if (input->IsKeyHit(alInputKey::K_RIGHT))
				_goRight();
			else if (input->IsKeyHit(alInputKey::K_UP))
				_goUp();
			else if (input->IsKeyHit(alInputKey::K_DOWN))
				_goDown();
			else if (input->IsKeyHit(alInputKey::K_DELETE)
				&& input->m_kbm != alKeyboardModifier::Shift)
				_delete();
			else if (input->IsKeyHit(alInputKey::K_BACKSPACE))
				_backspace();
			else if (input->IsKeyHit(alInputKey::K_HOME))
				_goHome();
			else if (input->IsKeyHit(alInputKey::K_END))
				_goEnd();
			else if (input->IsKeyHit(alInputKey::K_PGUP))
				_goPageUp();
			else if (input->IsKeyHit(alInputKey::K_PGDOWN))
				_goPageDown();

		}

		if (input->m_kbm == alKeyboardModifier::Ctrl)
		{
			if (input->IsKeyHit(alInputKey::K_A))
			{
				_goEndOfText();
				SelectAll();
			}
			else if (input->IsKeyHit(alInputKey::K_X))
				OnCtrlX();
			else if (input->IsKeyHit(alInputKey::K_C))
				CopyToClipboard();
			else if (input->IsKeyHit(alInputKey::K_V))
				PasteFromClipboard();
			else if (input->IsKeyHit(alInputKey::K_INSERT))
				CopyToClipboard();
		//	else if (input->IsKeyHit(alInputKey::K_S))
		//		Save();
			else if (input->IsKeyHit(alInputKey::K_Z))
				Undo();
			else if (input->IsKeyHit(alInputKey::K_Y))
				Redo();
		}
		else if (input->m_kbm == alKeyboardModifier::ShiftCtrl)
		{
			//if (input->IsKeyHit(alInputKey::K_S))
			//	SaveAs();
		}
		else
		{
			if (input->m_kbm == alKeyboardModifier::Shift)
			{
				if (input->IsKeyHit(alInputKey::K_INSERT))
					PasteFromClipboard();
				if (input->IsKeyHit(alInputKey::K_DELETE))
					CutToClipboard();
			}
			if (input->m_character)
			{
				if (input->m_character != L'\n'
					&& input->m_character != 8 // backspace
					&& input->m_character != 13 // enter
					&& input->m_character != 27 // escape
					)
				{
					bool ok = true;
					if (ok)
					{

				//		m_isSaved = false;

						if (input->m_character == '\t')
						{
							_onTab();
						}
						else
						{
							if (m_isSelected)
							{
								DeleteSelected();
							}
							_moveViewIntoTextCursor();
							//m_history.AddText(input->m_character, m_textCursorPosition);
							//m_text.Insert(input->m_character, m_textCursorPosition);

							// для Undo
							// перед вставкой надо проверить место куда вставляется.
							// когда вводим текст, вставка идёт поочерёдно.
							// и в историю добавляется текст когда будет встречен символ пробела
							//    \t \n
							// Но, может быть такое что надо например отодвинуть текстовый курсор
							// Например было "qwerty". Отодвинем курсор в центр между qwe и rty
							// и вставим '1' и получим "qwe1rty". Тогда, при Undo мы хотим отменить
							// действие - вставку 1. То есть, перед вводом '1' нужно добавить
							// "qwerty" в историю. Как это сделать?
							// Решение такое. Мы знаем что следующая вставка должна быть в
							// m_textCursorPosition. Надо сохранить это значение после любой вставки
							// символа. Потом проверяем, если мы отодвинули курсор, то будет 
							// не совпадение. Пока так.
							// 

							InsertChar(input->m_character);
							_updateData_textData(_moveIndexIntoCol1(m_textCursorPosition));
						}

						// случай, когда нажимаешь на крайнее место в строке, двигаешь мышкой
						// и происходит выделение. Выделение должно быть диапазоном.
						// А происходило так что m_selectionStart и m_selectionEnd
						// указывали на одно место. Пофиксил метод DeleteSelected.
						// И добавил сюда вот это чтобы выделялось правильно
						if (input->m_isLMBHold)
						{
							// проблема в сохранении индекса при клике.
							// решение-передаём новое значение
							m_textCursorPositionOnClick = m_textCursorPosition;
						}

						m_getCursorColPrefer = true;
						_showTextCursor();
						_findHOffsetFromTextCursorPosition();
						_findCurLineWidth();
					}
				}
			}
		}


		bool enter = input->IsKeyHit(alInputKey::K_ENTER);

		if (input->IsKeyHit(alInputKey::K_ESCAPE))
		{
			if (m_isSelected)
				DeselectAll();
			else if (m_oneLine)
			{
				Deactivate(false);
			}
		}
		else if (enter)
		{
			if (m_isSelected && !m_oneLine)
				DeleteSelected();

			if (m_oneLine)
			{
				DeselectAll();
				Deactivate(true);
			}
			else
			{
			//	m_isSaved = false;
				InsertChar(L'\n');
			}

			if (input->m_isLMBHold)
			{
				m_textCursorPositionOnClick = m_textCursorPosition;
			}

			m_getCursorColPrefer = true;
			_showTextCursor();
			_updateRects();
			_updateData();
			_findHOffsetFromTextCursorPosition();

			// Если текстовый курсор был на последней строке
			// то при вводе \n текст не прокрутится наверх так как
			// условие для прокручивания не выполнено.
			// курсор просто уйдёт вниз за пределы поля.
			// на ум приходит множество решений.
			// одно из
			// 1. определяем, находимся ли на последней строке
			// 2. меняем индекс m_drawLastIndex. _moveViewIntoTextCursor
			//   дальше всё сделает как надо
			if (m_textCursorLine == (m_drawStartIndexLine + m_numOfVisibleLines - 1))
			{
				//printf("last\n");
				m_drawLastIndex = m_textCursorPosition - 1;
				_moveViewIntoTextCursor();
			}
		}
	}
}

void alGUITextInput::_findHOffsetFromTextCursorPosition()
{
	m_find_horizontal_offset = true;
}

void alGUITextInput::Rebuild()
{
	alGUIElement::Rebuild();
	_updateRects();
	_updateData();
	_updateData_textData(0);
	Update(0.f);
}

void alGUITextInput::SetFont(alGUIFont* f, alGUIFont* ui)
{
	m_font = f;
	m_fontUI = ui;
	if (!m_fontUI)
		m_fontUI = f;
}

void alGUITextInput::SetText(const char32_t* text, ...)
{
	char32_t buf[0xF00];

	va_list args;
	va_start(args, text);
	auto wsz = alLib::vsnprintf(buf, 0xF00, text, args);
	va_end(args);

	for (uint32_t i = 0; i < wsz; ++i)
	{
		if (OnCharacter(buf[i]))
		{
			m_text.PushBack(buf[i]);
			m_textData.push_back(0);
		}
	}
	//m_text.Assign(buf);
	_moveViewIntoTextCursor();
	_findHOffsetFromTextCursorPosition();
}

void alGUITextInput::SetText(const char32_t* buf, size_t sz)
{
	for (uint32_t i = 0; i < sz; ++i)
	{
		if (OnCharacter(buf[i]))
		{
			m_text.PushBack(buf[i]);
			m_textData.push_back(0);
		}
	}
	_moveViewIntoTextCursor();
	_findHOffsetFromTextCursorPosition();
}

void alGUITextInput::Clear()
{
	m_text.Clear();
}

void alGUITextInput::Activate()
{
	//m_textCursorPositionWhenClick = 0;
	if (m_context->m_inputTextElementActivated)
	{
		m_context->m_inputTextElementActivated->Deactivate(true);
	}

	m_context->m_inputTextElementActivated = this;
	m_isEditActive = true;
	_updateRects();
	_updateData();
}

void alGUITextInput::Deactivate(bool isEnter)
{
	if (m_context->m_inputTextElementActivated == this)
		m_context->m_inputTextElementActivated = 0;
	m_isEditActive = false;

	m_input->SetKeyHit(alInputKey::K_ESCAPE, false);
	m_input->SetKeyHit(alInputKey::K_ENTER, false);

	if (m_oneLine)
	{
		if (isEnter)
		{
			OnAccept();
		}
		else
		{
			OnCancel();
		}
	}
}

void alGUITextInput::DeleteSelected(bool addToHistory)
{
	if (!m_isSelected)
		return;
	if (m_selectionStart == m_selectionEnd)
	{
		DeselectAll();
		return;
	}

	m_getCursorColPrefer = true;
	auto s1 = m_selectionStart;
	auto s2 = m_selectionEnd;
	if (s1 > s2)
	{
		s1 = s2;
		s2 = m_selectionStart;
	}

	//m_isSaved = false;

	auto num_to_delete = s2 - s1;
	auto str_len = m_text.size();
	if (num_to_delete > str_len)
	{
		DeselectAll();
		return;
	}

	auto buf = m_text.data();
	if (addToHistory)
	{
		m_history.AddUndo();
		m_history.OnDeleteSelected(&buf[s1], num_to_delete, s1);
	}
	//printf("Delete %i %i\n", s1, s2);
	for (size_t i = s2, i2 = s1; i < str_len; ++i, ++i2)
	{
		buf[i2] = buf[i];
		m_textData[i2] = m_textData[i];
	}

	buf[str_len - num_to_delete] = 0;

	m_textCursorPosition = s1;
	m_text._set_size(str_len - num_to_delete);
	m_textData.SetSize(str_len - num_to_delete);

	DeselectAll();

	_findHOffsetFromTextCursorPosition();
	_findCurLineWidth();
	_updateData();
}

void alGUITextInput::DeleteAll()
{
	SelectAll();
	DeleteSelected();
}

void alGUITextInput::DeselectAll()
{
	m_selectionStart = 0;
	m_selectionEnd = 0;
	m_isSelected = false;
}

void alGUITextInput::SelectAll()
{
	if (m_text.Size())
	{
		_goEndOfText();

		m_selectionStart = 0;
		m_selectionEnd = m_text.Size();
		m_isSelected = true;
		_findHOffsetFromTextCursorPosition();
	}
}

void alGUITextInput::CutToClipboard()
{
	if (!m_isSelected)
		return;
	CopyToClipboard();
	this->DeleteSelected();
}

void alGUITextInput::CopyToClipboard()
{
	if (!m_isSelected)
		return;
	alUnicodeString str;
	size_t s1 = m_selectionStart;
	size_t s2 = m_selectionEnd;
	if (s1 > s2)
	{
		s1 = s2;
		s2 = m_selectionStart;
	}
	auto buf = m_text.Data();
	auto num_to_select = s2 - s1;
	for (size_t i = s1; i < s2; ++i)
	{
		str.Append(buf[i]);
	}
	alLib::CopyTextToClipboard(&str);
}

void alGUITextInput::PasteFromClipboard()
{
	m_history.AddUndo();
	alUnicodeString text;
	alLib::GetTextFromClipboard(&text);
	auto text_size = text.size();
	if (text_size)
	{
		if (m_isSelected)
			DeleteSelected();
		m_getCursorColPrefer = true;

		auto ptr = text.data();
		for (size_t i = 0; i < text_size; ++i)
		{
			if (OnCharacter(ptr[i]))
			{
	//			m_isSaved = false;
				m_text.Insert(ptr[i], m_textCursorPosition);
				m_textData.Insert(1, m_textCursorPosition);
				m_history.OnPasteText(ptr[i], m_textCursorPosition);
				++m_textCursorPosition;
			}
		}
		m_history.AddUndo();

		_findHOffsetFromTextCursorPosition();
		_findCurLineWidth();
		_updateData();
		_moveViewIntoTextCursor();
	}
}

void alGUITextInput::_goUp()
{
	auto input = alLib::GetInput();
	if (input->m_kbm == alKeyboardModifier::ShiftCtrl)
		return;

	if (input->m_kbm == alKeyboardModifier::Alt)
	{
	//	_moveTextUp();
		return;
	}

	_showTextCursor();

	if (input->m_kbm == alKeyboardModifier::Ctrl)
	{
		_moveUpView(1);
		if (m_textCursorLine <= (m_drawStartIndexLine + m_numOfVisibleLines - 1))
			return;

	}
	if (input->m_kbm == alKeyboardModifier::Shift)
	{
		if ((m_selectionStart == 0) && (m_selectionEnd == 0))
		{
			m_isSelected = true;
			m_selectionStart = m_textCursorPosition;
		}
	}
	else
	{
		if (m_isSelected)
			DeselectAll();
	}

	m_textCursorPosition = _moveIndexIntoCol1(m_textCursorPosition);
	if (m_textCursorPosition)
		--m_textCursorPosition;
	m_textCursorPosition = _moveIndexIntoCol1(m_textCursorPosition);
	_moveTextCursorToColPrefer();
	_moveViewIntoTextCursor();

	if (input->m_kbm == alKeyboardModifier::Shift)
		m_selectionEnd = m_textCursorPosition;
	_findHOffsetFromTextCursorPosition();
}
void alGUITextInput::_moveTextCursorToColPrefer()
{
	auto textSize = m_text.size();
	if (textSize)
	{
		m_textCursorPosition = _moveIndexIntoCol1(m_textCursorPosition);
		m_textCursorCol = 1;

		uint32_t tabCounter = 0;
		char32_t* textPtr = &m_text.data()[0];
		while (true)
		{
			if (textPtr[m_textCursorPosition] == L'\n')
				break;

			if (m_textCursorPosition == textSize)
				break;

			if (m_textCursorCol >= m_textCursorCol_prefer)
				break;

			auto textCursorCol_t = m_textCursorCol;
			if (textPtr[m_textCursorPosition] == '\t')
			{
				for (size_t o = tabCounter; o < m_tabSize; ++o)
				{
					if (m_textCursorCol == m_textCursorCol_prefer)
					{
						if (o < 2)
						{
							m_textCursorCol = textCursorCol_t;
							return;
						}
						else
						{
							if (o == tabCounter)
								break;
							for (size_t o2 = o; o2 < m_tabSize; ++o2)
							{
								++m_textCursorCol;
							}
							o = m_tabSize;
						}
					}
					++m_textCursorCol;
				}
				tabCounter = 0;
				++m_textCursorPosition;
			}
			else
			{
				++tabCounter;
				if (tabCounter == m_tabSize)
					tabCounter = 0;
				++m_textCursorCol;
				++m_textCursorPosition;
			}

		}
	}
}
void alGUITextInput::_goDown()
{
	auto input = alLib::GetInput();
	if (input->m_kbm == alKeyboardModifier::ShiftCtrl)
		return;

	if (input->m_kbm == alKeyboardModifier::Alt)
	{
	//	_moveTextDown();
		return;
	}

	_showTextCursor();
	if (m_textCursorPosition == m_text.size())
		return;

	if (input->m_kbm == alKeyboardModifier::Ctrl)
	{
		_moveDownView(1);
		if (m_textCursorLine >= m_drawStartIndexLine)
			return;
	}

	if (input->m_kbm == alKeyboardModifier::Shift)
	{
		if ((m_selectionStart == 0) && (m_selectionEnd == 0))
		{
			m_isSelected = true;
			m_selectionStart = m_textCursorPosition;
		}
	}
	else
	{
		if (m_isSelected)
			DeselectAll();
	}

	m_textCursorPosition = _moveIndexIntoEndOfTheLine(m_textCursorPosition);
	if (m_textCursorPosition < m_text.size())
		++m_textCursorPosition;
	_moveTextCursorToColPrefer();
	_moveViewIntoTextCursor();

	if (input->m_kbm == alKeyboardModifier::Shift)
		m_selectionEnd = m_textCursorPosition;
	_findHOffsetFromTextCursorPosition();
}

void alGUITextInput::_goLeft()
{
	_showTextCursor();
	auto input = alLib::GetInput();
	if (m_textCursorPosition)
	{
		if (input->m_kbm == alKeyboardModifier::Shift
			|| input->m_kbm == alKeyboardModifier::ShiftCtrl)
		{
			if ((m_selectionStart == 0) && (m_selectionEnd == 0))
			{
				m_isSelected = true;
				m_selectionStart = m_textCursorPosition;
			}
			m_selectionEnd = m_textCursorPosition - 1;
		}
		else
		{
			if (m_isSelected)
			{
				if (m_selectionStart < m_selectionEnd)
					m_textCursorPosition = m_selectionStart + 1;
				else
					++m_textCursorPosition;
				DeselectAll();
			}
		}

		if (input->m_kbm == alKeyboardModifier::Ctrl
			|| input->m_kbm == alKeyboardModifier::ShiftCtrl)
		{
			WordInfo wi;
			GetWordInfo(m_textCursorPosition, &wi);
			if (wi.m_begin == m_textCursorPosition)
			{
				--m_textCursorPosition;
				GetWordInfo(m_textCursorPosition, &wi);
				m_textCursorPosition = wi.m_begin;

				GetWordInfo(m_textCursorPosition - 1, &wi);
				if (wi.m_type == WordTypeBasic::space)
					m_textCursorPosition = wi.m_begin;
			}
			else
			{
				m_textCursorPosition = wi.m_begin;
				GetWordInfo(m_textCursorPosition - 1, &wi);
				if (wi.m_type == WordTypeBasic::space)
					m_textCursorPosition = wi.m_begin;
			}
			if (input->m_kbm == alKeyboardModifier::ShiftCtrl)
				m_selectionEnd = m_textCursorPosition;
		}
		else
		{
			--m_textCursorPosition;
		}
		m_getCursorColPrefer = true;
	}
	else
	{
		if (m_isSelected && input->m_kbm != alKeyboardModifier::Shift)
			DeselectAll();
	}
	_moveViewIntoTextCursor();
	_findHOffsetFromTextCursorPosition();
}

void alGUITextInput::_goRight()
{
	_showTextCursor();
	auto input = alLib::GetInput();
	if (m_textCursorPosition < m_text.size())
	{
		if (input->m_kbm == alKeyboardModifier::Shift
			|| input->m_kbm == alKeyboardModifier::ShiftCtrl)
		{
			if ((m_selectionStart == 0) && (m_selectionEnd == 0))
			{
				m_isSelected = true;
				m_selectionStart = m_textCursorPosition;
			}
			m_selectionEnd = m_textCursorPosition + 1;
		}
		else
		{
			if (m_isSelected)
			{
				if (m_selectionStart > m_selectionEnd)
					m_textCursorPosition = m_selectionStart - 1;
				else
					--m_textCursorPosition;

				DeselectAll();
			}
		}

		if (input->m_kbm == alKeyboardModifier::Ctrl
			|| input->m_kbm == alKeyboardModifier::ShiftCtrl)
		{
			WordInfo wi;
			GetWordInfo(m_textCursorPosition, &wi);
			if (wi.m_end == m_textCursorPosition)
			{
				++m_textCursorPosition;
				GetWordInfo(m_textCursorPosition, &wi);
				m_textCursorPosition = wi.m_end;

				GetWordInfo(m_textCursorPosition, &wi);
				if (wi.m_type == WordTypeBasic::space)
					m_textCursorPosition = wi.m_end;
			}
			else
			{
				m_textCursorPosition = wi.m_end;
				GetWordInfo(m_textCursorPosition, &wi);
				if (wi.m_type == WordTypeBasic::space)
					m_textCursorPosition = wi.m_end;
			}
			if (input->m_kbm == alKeyboardModifier::ShiftCtrl)
				m_selectionEnd = m_textCursorPosition;
		}
		else
		{
			++m_textCursorPosition;
		}
		m_getCursorColPrefer = true;
	}
	else
	{
		if (m_isSelected && input->m_kbm != alKeyboardModifier::Shift)
			DeselectAll();
	}
	_moveViewIntoTextCursor();
	_findHOffsetFromTextCursorPosition();
}

void alGUITextInput::_delete()
{
	_showTextCursor();
	if (m_textCursorPosition < m_text.size())
	{
		auto input = alLib::GetInput();
		if (input->m_kbm == alKeyboardModifier::Ctrl)
		{
			if (!m_isSelected)
			{
				WordInfo wi;
				GetWordInfo(m_textCursorPosition, &wi);
				m_selectionStart = m_textCursorPosition;
				m_selectionEnd = wi.m_end;
				m_isSelected = true;

				auto oldPos = m_textCursorPosition;
				m_textCursorPosition = wi.m_end;
				GetWordInfo(m_textCursorPosition, &wi);
				if (wi.m_type == WordTypeBasic::space)
					m_selectionEnd = wi.m_end;
				m_textCursorPosition = wi.m_end;

				m_textCursorPosition = oldPos;
			}
		}

		if (m_isSelected)
		{
			DeleteSelected();
		}
		else
		{
			m_history.AddUndo();

			bool ok = false;
			bool needUpdate = false;
			auto buf = m_text.data();
			auto str_len = m_text.size();
			m_history.OnDelete(buf[m_textCursorPosition], m_textCursorPosition, false);

			for (size_t i = m_textCursorPosition; i < str_len; ++i)
			{
				ok = true;
				if (i + 1 == str_len)
					break;
				if (buf[i] == L'\n')
					needUpdate = true;

				buf[i] = buf[i + 1];
				m_textData[i] = m_textData[i + 1];
			}
			if (ok)
			{
			//	m_isSaved = false;
				buf[str_len - 1] = 0;
				m_text._set_size(m_text.size() - 1);
				m_textData.SetSize(m_textData.size() - 1);

				if (needUpdate)
				{
					_updateRects();
					_updateData();
				}
			}
		}

	}
	else
	{
		if (m_isSelected)
		{
			DeleteSelected();
		}
	}
	_findHOffsetFromTextCursorPosition();
	_moveViewIntoTextCursor();
}

void alGUITextInput::_backspace()
{
	_showTextCursor();
	m_getCursorColPrefer = true;
	if (m_isSelected)
	{
		DeleteSelected();
	}
	else
	{
		bool ok = false;
		bool needUpdate = false;
		auto buf = m_text.data();
		auto str_len = m_text.size();
		m_history.AddUndo();

		//	if(m_textCursorPosition && m_textCursorPosition < str_len)
		//		m_history.OnDeleteChar(buf[m_textCursorPosition-1], m_textCursorPosition - 1);
		if (m_textCursorPosition && m_textCursorPosition <= str_len)
			m_history.OnDelete(buf[m_textCursorPosition - 1], m_textCursorPosition - 1, true);

		for (size_t i = m_textCursorPosition; i < str_len; ++i)
		{
			if (i == 0)
				break;
			ok = true;
			if (buf[i - 1] == L'\n')
				needUpdate = true;

			buf[i - 1] = buf[i];
			m_textData[i - 1] = m_textData[i];
		}
		if (m_textCursorPosition == str_len && !ok)
			ok = true;
		if (ok)
		{
		//	m_isSaved = false;
			if (str_len - 1 >= 0)
			{
				if (m_textCursorPosition)
				{
					--m_textCursorPosition;
					buf[str_len - 1] = 0;
					m_text._set_size(m_text.size() - 1);
					m_textData.SetSize(m_textData.size() - 1);
				}
			}

			if (needUpdate)
			{
				_updateRects();
				_updateData();
			}
		}
	}
	_moveViewIntoTextCursor();
	_findHOffsetFromTextCursorPosition();
}

void alGUITextInput::_goHome()
{
	_showTextCursor();
	auto input = alLib::GetInput();
	if (input->m_kbm == alKeyboardModifier::Ctrl
		|| input->m_kbm == alKeyboardModifier::ShiftCtrl)
	{
		_goHomeOfText();
		return;
	}
	if (input->m_kbm == alKeyboardModifier::Shift)
	{
		if ((m_selectionStart == 0) && (m_selectionEnd == 0))
		{
			m_isSelected = true;
			m_selectionStart = m_textCursorPosition;
		}
	}
	else
	{
		if (m_isSelected)
			DeselectAll();
	}
	m_textCursorPosition = _moveIndexIntoCol1(m_textCursorPosition);
	m_getCursorColPrefer = true;
	_moveViewIntoTextCursor();
	if (input->m_kbm == alKeyboardModifier::Shift)
		m_selectionEnd = m_textCursorPosition;
	_findHOffsetFromTextCursorPosition();
}

void alGUITextInput::_goEnd()
{
	_showTextCursor();
	auto input = alLib::GetInput();
	if (input->m_kbm == alKeyboardModifier::Ctrl
		|| input->m_kbm == alKeyboardModifier::ShiftCtrl)
	{
		_goEndOfText();
		return;
	}
	if (input->m_kbm == alKeyboardModifier::Shift)
	{
		if ((m_selectionStart == 0) && (m_selectionEnd == 0))
		{
			m_isSelected = true;
			m_selectionStart = m_textCursorPosition;
		}
	}
	else
	{
		if (m_isSelected)
			DeselectAll();
	}
	m_textCursorPosition = _moveIndexIntoEndOfTheLine(m_textCursorPosition);
	m_getCursorColPrefer = true;
	_moveViewIntoTextCursor();
	if (input->m_kbm == alKeyboardModifier::Shift)
		m_selectionEnd = m_textCursorPosition;
	_findHOffsetFromTextCursorPosition();
}

void alGUITextInput::_goEndOfText()
{
	_showTextCursor();
	auto input = alLib::GetInput();
	if (input->m_kbm == alKeyboardModifier::ShiftCtrl)
	{
		if ((m_selectionStart == 0) && (m_selectionEnd == 0))
		{
			m_isSelected = true;
			m_selectionStart = m_textCursorPosition;
		}
	}
	else
	{
		if (m_isSelected)
			DeselectAll();
	}
	m_textCursorPosition = m_text.size();
	m_getCursorColPrefer = true;
	_moveViewIntoTextCursor();
	if (input->m_kbm == alKeyboardModifier::ShiftCtrl)
		m_selectionEnd = m_textCursorPosition;
	_findHOffsetFromTextCursorPosition();
}

void alGUITextInput::_goHomeOfText()
{
	_showTextCursor();
	auto input = alLib::GetInput();
	if (input->m_kbm == alKeyboardModifier::ShiftCtrl
		|| input->m_kbm == alKeyboardModifier::Shift)
	{
		if ((m_selectionStart == 0) && (m_selectionEnd == 0))
		{
			m_isSelected = true;
			m_selectionStart = m_textCursorPosition;
		}
	}
	else
	{
		if (m_isSelected)
			DeselectAll();
	}
	m_textCursorPosition = 0;
	m_getCursorColPrefer = true;
	_moveViewIntoTextCursor();
	if (input->m_kbm == alKeyboardModifier::Shift
		|| input->m_kbm == alKeyboardModifier::ShiftCtrl)
		m_selectionEnd = m_textCursorPosition;
	_findHOffsetFromTextCursorPosition();
}

void alGUITextInput::_showTextCursor()
{
	m_drawTextCursor = true;
	m_textCursorTimer = 0.f;
}
void alGUITextInput::_moveTextCursorUp(uint32_t num)
{
	for (uint32_t i = 0; i < num; ++i)
	{
		m_textCursorPosition = _moveIndexIntoCol1(m_textCursorPosition);
		if (m_textCursorPosition)
			--m_textCursorPosition;
	}
}

void alGUITextInput::_moveTextCursorDown(uint32_t num)
{
	auto textSize = m_text.size();
	for (uint32_t i = 0; i < num; ++i)
	{
		m_textCursorPosition = _moveIndexIntoEndOfTheLine(m_textCursorPosition);
		if (m_textCursorPosition < textSize)
			++m_textCursorPosition;
	}
}
void alGUITextInput::_goPageUp()
{
	auto input = alLib::GetInput();
	_showTextCursor();
	if (input->m_kbm == alKeyboardModifier::ShiftCtrl
		|| input->m_kbm == alKeyboardModifier::Shift)
	{
		if ((m_selectionStart == 0) && (m_selectionEnd == 0))
		{
			m_isSelected = true;
			m_selectionStart = m_textCursorPosition;
		}
	}
	else
	{
		if (m_isSelected)
			DeselectAll();
	}

	// при удержании Ctrl нужно просто поставить курсор 
	// в начало первой строки
	if (input->m_kbm == alKeyboardModifier::ShiftCtrl
		|| input->m_kbm == alKeyboardModifier::Ctrl)
	{
		m_textCursorPosition = m_drawStartIndex;
	}
	else
	{
		uint32_t numLines = m_drawStartIndexLine;
		if (numLines > m_numOfVisibleLines)
			numLines = m_numOfVisibleLines;

		if (numLines > 1)
		{
			_moveUpView(numLines);
			_moveTextCursorUp(numLines);
		}
		else
		{
			m_drawStartIndex = 0;
			m_textCursorPosition = 0;
		}
	}
	m_textCursorCol = 0;
	_moveTextCursorToColPrefer();

	if (input->m_kbm == alKeyboardModifier::ShiftCtrl
		|| input->m_kbm == alKeyboardModifier::Shift)
		m_selectionEnd = m_textCursorPosition;
	_findHOffsetFromTextCursorPosition();
}

void alGUITextInput::_goPageDown()
{
	auto input = alLib::GetInput();
	_showTextCursor();
	if (input->m_kbm == alKeyboardModifier::ShiftCtrl
		|| input->m_kbm == alKeyboardModifier::Shift)
	{
		if ((m_selectionStart == 0) && (m_selectionEnd == 0))
		{
			m_isSelected = true;
			m_selectionStart = m_textCursorPosition;
		}
	}
	else
	{
		if (m_isSelected)
			DeselectAll();
	}

	// здесь при удержании Ctrl нужно поставить курсор 
	// в конец последней строки + будет найден m_textCursorCol
	if (input->m_kbm == alKeyboardModifier::ShiftCtrl
		|| input->m_kbm == alKeyboardModifier::Ctrl)
	{
		m_textCursorPosition = m_drawLastIndex;
	}
	else
	{
		auto linesRemain = m_lineNum - m_drawStartIndexLine;

		if (linesRemain > m_numOfVisibleLines)
		{
			_moveDownView(m_numOfVisibleLines);
			_moveTextCursorDown(m_numOfVisibleLines);
		}
		else
		{
			m_textCursorPosition = m_drawLastIndex;
		}
	}
	m_textCursorCol = 0;
	_moveTextCursorToColPrefer();

	if (input->m_kbm == alKeyboardModifier::ShiftCtrl
		|| input->m_kbm == alKeyboardModifier::Shift)
		m_selectionEnd = m_textCursorPosition;
	_findHOffsetFromTextCursorPosition();
}
void alGUITextInput::_moveViewToLine(uint32_t line)
{
	if (line < m_drawStartIndexLine)
	{
		_moveUpView(m_drawStartIndexLine - line);
	}
	else if (line > m_drawStartIndexLine)
	{
		_moveDownView(line - m_drawStartIndexLine);
	}
}

void alGUITextInput::_moveUpView(uint32_t num)
{
	if (m_drawStartIndex)
	{
		for (uint32_t i = 0; i < num; ++i)
		{
			// move up on the end of the line above
			--m_drawStartIndex;
			--m_drawStartIndexLine;
			if (!m_drawStartIndex)
				break;
			m_drawStartIndex = _moveIndexIntoCol1(m_drawStartIndex);

			if (!m_drawStartIndex)
				break;
		}

		if (!m_drawStartIndex)
			m_drawStartIndexLine = 1;
	}
}

void alGUITextInput::_moveDownView(uint32_t num)
{
	auto textSize = m_text.Size();
	for (uint32_t i = 0; i < num; ++i)
	{
		m_drawStartIndex = _moveIndexIntoEndOfTheLine(m_drawStartIndex);
		if (m_drawStartIndex == textSize)
			break;

		++m_drawStartIndex;
		++m_drawStartIndexLine;
		if (m_drawStartIndex == textSize)
			break;
	}

	// Если m_drawStartIndex перекрутился в самый конец массива
	// то перекрутим его вначало строки.
	if (m_drawStartIndex == textSize)
	{
		m_drawStartIndex = _moveIndexIntoCol1(m_drawStartIndex);
	}
}

void alGUITextInput::_updateData()
{
	m_numOfVisibleLines = (uint32_t)floorf(m_textAreaHeight / m_nextLinePositionOffset) + 1;
	if (!m_numOfVisibleLines)
		m_numOfVisibleLines = 1;
	else if (m_numOfVisibleLines > 1)
		m_numOfVisibleLines -= 1;

	//m_drawLastIndex = -1;
	m_maxLineWidth = 0.f;
	m_drawStartIndexLine = 1;
	m_lineNum = 1;
	if (m_text.Size())
	{
		float32_t lineWidth = 0.f;
		auto textSize = m_text.size();
		auto textPtr = m_text.c_str();
		size_t col = 0;
		for (size_t i = 0; i < textSize; ++i)
		{
			char32_t currChar = textPtr[i];

			if (!col)
				_updateData_textData(i);

			if (i == m_drawStartIndex)
			{
				m_drawStartIndexLine = m_lineNum;
			}

			lineWidth += m_nextCharPositionOffset;

			if (currChar == '\n')
			{
				++m_lineNum;
				lineWidth = 0.f;
				col = 0;
			}
			else
			{
				++col;
			}

			if (lineWidth > m_maxLineWidth)
				m_maxLineWidth = lineWidth;
		}
		_findNeedToDrawHScroll();
	}
}
void alGUITextInput::_findNeedToDrawHScroll()
{
	auto textAreaSizeX = m_editorAreaText.z - m_editorAreaText.x;

	if (m_maxLineWidth > (textAreaSizeX - 100.f))
	{
		if(m_useHorizontalScrollbar)
			m_scrollH.m_needToDraw = true;
		m_hscroll_widthLen = m_maxLineWidth - (textAreaSizeX - 100.f);
	}
	else
		m_scrollH.m_needToDraw = false;
	
	if (!m_useHorizontalScrollbar)
		m_scrollH.m_needToDraw = false;
}
void alGUITextInput::_updateData_textData(size_t index)
{
	auto textSize = m_text.size();
	auto textPtr = m_text.c_str();
	auto textDataPtr = m_textData.data();
	char32_t* wordCurrChar = &textDataPtr[0];
	WordTypeBasic wordType = WordTypeBasic::unknown;
	for (size_t i = index; i < textSize; ++i)
	{
		char32_t currChar = textPtr[i];
		textDataPtr[i] = 0;

		if (currChar == '\n')
		{
			wordType = WordTypeBasic::new_line;
			textDataPtr[i] &= ~0x700;
			textDataPtr[i] |= 0x500;
			break;
		}
		else
		{
			if (currChar < 127)
			{
				if (isspace(currChar))
				{
					//if (wordType == WordTypeBasic::unknown)
					{
						wordType = WordTypeBasic::space;
					}
				}
				else if (isdigit(currChar))
				{
					if (wordType != WordTypeBasic::word)
					{
						wordType = WordTypeBasic::number;
					}
				}
				else if (ispunct(currChar))
				{
					if (currChar == '_')
					{
						wordType = WordTypeBasic::word;
					}
					else
					{
						wordType = WordTypeBasic::punct;
					}
				}
				else
				{
					wordType = WordTypeBasic::word;
				}
			}
			else
			{
				wordType = WordTypeBasic::word;
			}

			switch (wordType)
			{
			case WordTypeBasic::unknown:
			default:
				break;
			case WordTypeBasic::space:
				textDataPtr[i] &= ~0x700;
				textDataPtr[i] |= 0x100;
				break;
			case WordTypeBasic::punct:
				textDataPtr[i] &= ~0x700;
				textDataPtr[i] |= 0x200;
				break;
			case WordTypeBasic::word:
				textDataPtr[i] &= ~0x700;
				textDataPtr[i] |= 0x300;
				break;
			case WordTypeBasic::number:
				textDataPtr[i] &= ~0x700;
				textDataPtr[i] |= 0x400;
				break;
			}
		}
	}
}
void alGUITextInput::_showTextEditPopup()
{
	//если ввели символ и не нажали например пробел то
	// символ будет висеть в буфере и не попадёт в m_undoData
	// но Undo уже должен работать поэтому вызывая AddUndo
	// засовываем то что в буфере в m_undoData
	m_history.AddUndo();

	/*HMENU menu = CreatePopupMenu();
	if (m_history.IsHasUndo())
		AppendMenu(menu, MF_BYPOSITION | MF_STRING, NOTEPAD_MENUID_UNDO, L"Undo\tCtrl+Z");
	if (m_history.IsHasRedo())
		AppendMenu(menu, MF_BYPOSITION | MF_STRING, NOTEPAD_MENUID_REDO, L"Redo\tCtrl+Y");
	AppendMenu(menu, MF_SEPARATOR, 0, 0);
	AppendMenu(menu, MF_BYPOSITION | MF_STRING, NOTEPAD_MENUID_CUT, L"Cut\tCtrl+X");
	AppendMenu(menu, MF_BYPOSITION | MF_STRING, NOTEPAD_MENUID_COPY, L"Copy\tCtrl+C");
	AppendMenu(menu, MF_BYPOSITION | MF_STRING, NOTEPAD_MENUID_PASTE, L"Paste\tCtrl+V");
	AppendMenu(menu, MF_BYPOSITION | MF_STRING, NOTEPAD_MENUID_DELETE, L"Delete\tDelete");
	AppendMenu(menu, MF_SEPARATOR, 0, 0);
	AppendMenu(menu, MF_BYPOSITION | MF_STRING, NOTEPAD_MENUID_SELECTALL, L"Select All\tCtrl+A");

	HWND hWnd = (HWND)g_app->m_hwnd;
	SetForegroundWindow(hWnd);
	POINT pt;
	auto input = alLib::GetInput();
	pt.x = input->m_cursorCoords.x;
	pt.y = input->m_cursorCoords.y;
	ClientToScreen(hWnd, &pt);
	TrackPopupMenu(menu, TPM_TOPALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL);
	DestroyMenu(menu);*/
}

void alGUITextInput::GoTo(uint32_t line, uint32_t col, bool tabs)
{
	if (m_text.size())
	{
		float32_t lineWidth = 0.f;
		auto textSize = m_text.size();
		auto textPtr = m_text.c_str();
		uint32_t lineCounter = 1;
		size_t textCursorPosition = 0;
		for (size_t i = 0; i < textSize; ++i)
		{
			if (lineCounter == line)
				break;
			if (textPtr[i] == '\n')
				++lineCounter;
			++textCursorPosition;
		}

		// find col
		uint32_t tabCounter = 0;
		uint32_t colCounter = 1;
		for (size_t i = textCursorPosition; i < textSize; ++i)
		{
			if (textPtr[i] == '\n')
				break;
			if (textCursorPosition == textSize)
				break;
			if (colCounter >= col)
				break;
			if (textPtr[textCursorPosition] == '\t')
			{
				if (tabs)
				{
					for (size_t o = tabCounter; o < m_tabSize; ++o)
					{
						++colCounter;
					}
					tabCounter = 0;
				}
				else
				{
					++colCounter;
				}
				++textCursorPosition;
			}
			else
			{
				++tabCounter;
				if (tabCounter == m_tabSize)
					tabCounter = 0;
				++colCounter;
				++textCursorPosition;
			}
		}

		m_textCursorPosition = textCursorPosition;
		_moveViewIntoTextCursor();
		_findHOffsetFromTextCursorPosition();
	}
}
void alGUITextInput::Select(size_t begin, size_t end) {
	if (m_text.size() && (begin != end))
	{
		m_selectionStart = begin;
		m_selectionEnd = end;
		if (end < begin)
		{
			m_selectionStart = end;
			m_selectionEnd = begin;
		}
		if (m_selectionEnd > m_text.size())
			m_selectionEnd = m_text.size();
		m_isSelected = true;
	}
}

void alGUITextInput::Select(size_t num) {
	if (m_text.size())
	{
		if (m_textCursorPosition < m_text.size())
		{
			m_selectionStart = m_textCursorPosition;
			m_selectionEnd = m_textCursorPosition + num;
			if (m_selectionEnd > m_text.size())
			{
				m_selectionEnd = m_text.size();
			}
			m_isSelected = true;
		}
	}
}
void alGUITextInput::_moveViewIntoTextCursor()
{
	if (m_textCursorPosition < m_drawStartIndex)
	{
		m_drawStartIndex = _moveIndexIntoCol1(m_textCursorPosition);
		_updateData();
	}
	else if (m_textCursorPosition > m_drawLastIndex)
	{
		// перемещаем m_drawStartIndex на строку где находится курсор
		// значение m_textCursorPosition двигаем в начало строки, вот
		// он и m_drawStartIndex
		m_drawStartIndex = _moveIndexIntoCol1(m_textCursorPosition);

		// прокручиваем текст вниз на количество видимых строк
		_moveUpView(m_numOfVisibleLines - 1);

		_updateData();
	}
	else
	{

	}
}
void alGUITextInput::GetWordInfo(size_t pos, WordInfo* out)
{
	auto textCursor = pos;
	auto textSize = m_text.size();
	auto textPtr = m_text.data();
	auto textDataPtr = m_textData.data();
	if (textSize)
	{
		auto col1 = _moveIndexIntoCol1(textCursor);
		auto colEnd = _moveIndexIntoEndOfTheLine(textCursor);
		WordTypeBasic wordType = WordTypeBasic::unknown;
		out->m_begin = col1;
		out->m_end = colEnd + 1;
		auto type = (textDataPtr[textCursor] & 0x700) >> 8;
		switch (type)
		{
		default:
		case 0x3:
			out->m_type = WordTypeBasic::word;
			break;
		case 0x1:
			out->m_type = WordTypeBasic::space;
			break;
		case 0x2:
			out->m_type = WordTypeBasic::punct;
			break;
		case 0x4:
			out->m_type = WordTypeBasic::number;
			break;
		case 0x5:
			out->m_type = WordTypeBasic::new_line;
			break;
		}

		out->m_color = textDataPtr[textCursor] & 0xFF;

		if (textCursor == textSize)
			--textCursor;

		// нахождение начала.
		for (size_t i = textCursor; i >= col1;)
		{
			char32_t currChar = textPtr[i];
			auto currType = (textDataPtr[i] & 0x700) >> 8;
			if (currType != type)
				break;

			out->m_begin = i;

			if (i == col1)
				break;
			--i;
		}


		// нахождение конца.
		for (size_t i = textCursor; i < colEnd + 1; ++i)
		{
			char32_t currChar = textPtr[i];
			auto currType = (textDataPtr[i] & 0x700) >> 8;
			if (currType != type)
				break;

			out->m_end = i + 1;
		}

	}
}
void alGUITextInput::OnCtrlX()
{
	if (!m_isSelected)
	{
		Select(
			_moveIndexIntoCol1(m_textCursorPosition),
			_moveIndexIntoEndOfTheLine(m_textCursorPosition) + 1);
		m_textCursorPosition = m_selectionEnd;
	}

	if (m_isSelected)
	{
		CutToClipboard();
	}
}
void alGUITextInput::Undo()
{
	HistoryNode node;
	if (m_history.Undo(&node))
	{
		if (node.m_type == node.type_insert)
		{
			m_isSelected = true;
			m_selectionStart = node.m_charPosition;
			m_selectionEnd = node.m_charPosition + node.m_charNum;
			DeleteSelected(false);
		}
		else if (node.m_type == node.type_delete)
		{
			if (node.m_string)
			{
				for (size_t i = 0; i < node.m_charNum; ++i)
				{
					m_text.Insert(node.m_string[i], node.m_charPosition);
					m_textData.Insert(1, node.m_charPosition);
					m_textCursorPosition = node.m_charPosition;
					if (node.m_backspace)
						++m_textCursorPosition;
					m_textCursorPositionNext = m_textCursorPosition;
				}
			}
		}
		else if (node.m_type == node.type_tabsInsert)
		{
			if (node.m_tabsNum > 1)
			{
				DeselectAll();
				m_textCursorPosition = node.m_tabsIndex;
				m_selectionStart = m_textCursorPosition;
				_onTab_multiDelete(node.m_tabsNum, 0);
				m_textCursorPositionNext = m_textCursorPosition;
			}
		}
		else if (node.m_type == node.type_tabsDelete)
		{
			if (node.m_tabsNum > 1)
			{
				DeselectAll();
				m_textCursorPosition = node.m_tabsIndex;
				m_selectionStart = m_textCursorPosition;
				_onTab_multiInsert(node.m_tabsNum, 0, node.m_tabsBuffer);
				m_textCursorPositionNext = m_textCursorPosition;
			}
		}

		_updateRects();
		_updateData();
		_moveViewIntoTextCursor();
	}
}

void alGUITextInput::Redo()
{
	HistoryNode node;
	if (m_history.Redo(&node))
	{
		if (m_isSelected)
			DeselectAll();

		if (node.m_type == node.type_insert)
		{
			m_textCursorPosition = node.m_charPosition;
			auto ptr = node.m_string;
			for (size_t i = 0; i < node.m_charNum; ++i)
			{
				m_text.Insert(ptr[i], m_textCursorPosition);
				m_textData.Insert(1, m_textCursorPosition);
				++m_textCursorPosition;
			}
			m_textCursorPositionNext = m_textCursorPosition;
		}
		else if (node.m_type == node.type_delete)
		{
			if (node.m_charNum > 1)
			{
				m_isSelected = true;
				m_selectionStart = node.m_charPosition;
				m_selectionEnd = node.m_charPosition + node.m_charNum;
				DeleteSelected(false);
			}
			else
			{
				// придётся вставить цикл удаления символа
				auto str_len = m_text.size();
				bool needUpdate = false;
				bool ok = false;
				auto buf = m_text.data();
				for (size_t i = node.m_charPosition; i < str_len; ++i)
				{
					ok = true;
					if (i + 1 == str_len)
						break;
					if (buf[i] == L'\n')
						needUpdate = true;

					buf[i] = buf[i + 1];
					m_textData[i] = m_textData[i + 1];
				}
				if (ok)
				{
				//	m_isSaved = false;
					buf[str_len - 1] = 0;
					m_text._set_size(m_text.size() - 1);
					m_textData.SetSize(m_textData.size() - 1);

					if (needUpdate)
					{
						_updateRects();
						_updateData();
					}
				}
			}
			m_textCursorPosition = node.m_charPosition;
			m_textCursorPositionNext = m_textCursorPosition;
		}
		else if (node.m_type == node.type_tabsInsert)
		{
			if (node.m_tabsNum > 1)
			{
				DeselectAll();
				m_textCursorPosition = node.m_tabsIndex;
				m_selectionStart = m_textCursorPosition;
				_onTab_multiInsert(node.m_tabsNum, 0, node.m_tabsBuffer);
				m_textCursorPositionNext = m_textCursorPosition;
			}
		}
		else if (node.m_type == node.type_tabsDelete)
		{
			if (node.m_tabsNum > 1)
			{
				DeselectAll();
				m_textCursorPosition = node.m_tabsIndex;
				m_selectionStart = m_textCursorPosition;
				_onTab_multiDelete(node.m_tabsNum, 0);
				m_textCursorPositionNext = m_textCursorPosition;

			}
		}

		_moveViewIntoTextCursor();
	}
}
void alGUITextInput::_onTab()
{
	if (m_isSelected)
	{
		// надо определить сколько строк попало под выделение
		// если одна - то удаляем выделенное
		// если больше, то вставляем вначало каждой строки символ \t

		if (m_selectionEnd < m_selectionStart)
		{
			auto s = m_selectionStart;
			m_selectionStart = m_selectionEnd;
			m_selectionEnd = s;
		}

		uint32_t lineNum = 1;
		auto textSize = m_text.size();
		auto textPtr = m_text.data();
		for (size_t i = m_selectionStart; i < m_selectionEnd; ++i)
		{
			if (textPtr[i] == '\n')
			{
				if (i + 1 < m_selectionEnd)
					++lineNum;
			}
		}

		if (lineNum == 1)
		{
			DeleteSelected();
			InsertChar('\t');
		}
		else
		{
			uint8_t* tabsBuffer = (uint8_t*)calloc(lineNum, lineNum);

			auto input = alLib::GetInput();
			// если зажали шифт то нужно удалить первые встречные \t в строках
			if (input->m_kbm == alKeyboardModifier::Shift)
			{
				m_textCursorPosition = m_selectionStart;
				if (_onTab_multiDelete(lineNum, tabsBuffer))
				{
					HistoryNode historyNode;
					historyNode.m_type = historyNode.type_tabsDelete;
					historyNode.m_tabsIndex = m_selectionStart;
					historyNode.m_tabsNum = lineNum;
					historyNode.m_tabsBuffer = tabsBuffer;
					m_history.AddUndo(historyNode);
				}
				//_updateRects();
				//_updateData();
			}
			else
			{

				m_textCursorPosition = m_selectionStart;
				_onTab_multiInsert(lineNum, tabsBuffer, 0);
				HistoryNode historyNode;
				historyNode.m_type = historyNode.type_tabsInsert;
				historyNode.m_tabsIndex = m_selectionStart;
				historyNode.m_tabsNum = lineNum;
				historyNode.m_tabsBuffer = tabsBuffer;
				m_history.AddUndo(historyNode);
			}

		}
	}
	else
	{
		InsertChar('\t');
	}
	_moveViewIntoTextCursor();
	_findHOffsetFromTextCursorPosition();
	m_textCursorPositionNext = m_textCursorPosition;
}
void alGUITextInput::_onTab_multiInsert(size_t lineNum, uint8_t* set, uint8_t* get)
{
	auto textSize = m_text.size();
	m_textCursorPosition = _moveIndexIntoCol1(m_selectionStart);
	m_selectionStart = m_textCursorPosition;
	auto savePosition = m_textCursorPosition;
	for (uint32_t i = 0; i < lineNum; ++i)
	{
		// тут уже прибавляем
		if (m_textCursorPosition < m_drawStartIndex)
			++m_drawStartIndex;

		// вставка делается 100% всем строкам
		if (set)
		{
			set[i] = 1;
			m_text.Insert('\t', m_textCursorPosition);
			m_textData.Insert(1, m_textCursorPosition);
			++textSize;
			++m_textCursorPosition;
			m_textCursorPositionNext = m_textCursorPosition;
		}
		else if (get)
		{
			// здесь уже используется информация
			// если 0 то игнор вставки
			if (get[i] == 1)
			{
				m_text.Insert('\t', m_textCursorPosition);
				m_textData.Insert(1, m_textCursorPosition);
				++textSize;
				m_textCursorPositionNext = m_textCursorPosition;
			}
		}
		m_textCursorPosition = _moveIndexIntoEndOfTheLine(m_textCursorPosition);

		if (i == lineNum - 1)
			m_selectionEnd = m_textCursorPosition;

		++m_textCursorPosition;
		if (m_textCursorPosition > textSize)
			break;
	}
	m_textCursorPosition = m_selectionEnd;
	m_isSelected = true;
}

bool alGUITextInput::_onTab_multiDelete(size_t lineNum, uint8_t* set)
{
	auto textSize = m_text.size();
	auto textPtr = m_text.data();
	m_textCursorPosition = _moveIndexIntoCol1(m_textCursorPosition);
	m_selectionStart = m_textCursorPosition;
	bool result = false;

	for (uint32_t i = 0; i < lineNum; ++i)
	{
		if (set)
		{
			set[i] = 0;
		}

		if (textPtr[m_textCursorPosition] == '\t')
		{
			bool ok = false;
			for (size_t k = m_textCursorPosition; k < textSize; ++k)
			{
				if (k + 1 == textSize)
					break;
				if (set)
				{
					set[i] = 1;
				}
				textPtr[k] = textPtr[k + 1];
				m_textData[k] = m_textData[k + 1];
				ok = true;
			}
			if (ok)
			{
				// если удалили до индекса m_drawStartIndex
				// то надо его убавить иначе начнёт рисовать
				// со следующего символа
				if (m_textCursorPosition < m_drawStartIndex)
					--m_drawStartIndex;
				result = true;
				//	m_selectionEnd -= 1;
			}
			if (textSize)
			{
				textPtr[textSize - 1] = 0;
				m_text._set_size(m_text.size() - 1);
				m_textData.SetSize(m_textData.size() - 1);
				textSize = m_text.size();
			}
		}

		m_textCursorPosition = _moveIndexIntoEndOfTheLine(m_textCursorPosition);
		++m_textCursorPosition;
		if (m_textCursorPosition > textSize)
			break;
		if (i == lineNum - 1)
			--m_textCursorPosition;
	}
	if (m_textCursorPosition > textSize)
		m_textCursorPosition = textSize;
	m_selectionEnd = m_textCursorPosition;
	m_isSelected = true;
	return result;
}
void alGUITextInput::InsertChar(char32_t c)
{
	if (m_textCursorPositionNext != m_textCursorPosition)
		m_history.AddUndo();

//	m_isSaved = false;

	if (OnCharacter(c))
	{
		m_history.OnAddChar(
			c,
			m_textCursorPosition);

		m_text.Insert(c, m_textCursorPosition);
		m_textData.Insert(1, m_textCursorPosition);
		++m_textCursorPosition;
		m_textCursorPositionNext = m_textCursorPosition;
	}
}
void alGUITextInput::_findCurLineWidth()
{
	size_t textSize = m_text.size();
	auto index = _moveIndexIntoCol1(m_textCursorPosition);
	uint32_t tabCounter = 0;
	float32_t width = 0.f;
	char32_t* textPtr = &m_text.data()[0];
	while (true)
	{
		if (index >= textSize)
			break;

		if (textPtr[index] == L'\n')
			break;

		if (textPtr[index] == '\t')
		{
			for (size_t o = tabCounter; o < m_tabSize; ++o)
			{
				width += m_nextCharPositionOffset;
			}
			tabCounter = 0;
		}
		else
		{
			++tabCounter;
			if (tabCounter == m_tabSize)
				tabCounter = 0;
			width += m_nextCharPositionOffset;
		}


		++index;
	}

	if (width > m_maxLineWidth)
		m_maxLineWidth = width;
	_findNeedToDrawHScroll();
}

size_t alGUITextInput::_moveIndexIntoCol1(size_t index)
{
	if (!index)
		return 0;

	size_t result = index;

	char32_t* textPtr = &m_text.data()[0];
	while (true)
	{
		if (result == 0)
			break;

		if (textPtr[result - 1] == L'\n')
			break;

		--result;
	}
	return result;
}

size_t alGUITextInput::_moveIndexIntoEndOfTheLine(size_t index)
{
	auto textSize = m_text.size();
	if (index == textSize)
		return textSize;

	size_t result = index;
	char32_t* textPtr = &m_text.data()[0];
	while (true)
	{
		if (textPtr[result] == L'\n')
			break;
		++result;
		if (result == textSize)
			break;
	}
	return result;
}
