#include "al.h"
#include "GUI/alGUI.h"
#include "GS/alGS.h"
#include "GS/alGSTexture.h"
#include "Classes/alColor.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

bool alGUIRangeSlider1::TextInput::OnCharacter(char32_t c)
{
	if(c >= '0' && c <= '9')
		return true;
	if (c == '.')
		return true;
	if (c == '-')
		return true;
	return false;
}
void alGUIRangeSlider1::TextInput::OnRMBRelease()
{
}
void alGUIRangeSlider1::TextInput::OnAccept()
{
	alGUIRangeSlider1* slider = (alGUIRangeSlider1*)GetUserData();
	slider->_onTextInputAccept();
	m_text.Clear();
}
void alGUIRangeSlider1::TextInput::OnCancel()
{
	alGUIRangeSlider1* slider = (alGUIRangeSlider1*)GetUserData();
	slider->_onTextInputCancel();
	m_text.Clear();
}
void alGUIRangeSlider1::_onTextInputAccept()
{
	m_textInputActive = false;
	m_isClicked = false;

	switch (m_type)
	{
	default:
	case Type::type_Float:
	case Type::type_FloatLimits:
		*m_ptr_f = m_textInput->m_text.ToFloat();
		_checkLimits();
		m_old_onEnter_f = *m_ptr_f;
		break;
	case Type::type_Int:
	case Type::type_IntLimits:
		*m_ptr_i = m_textInput->m_text.ToInt();
		_checkLimits();
		m_old_onEnter_i = *m_ptr_i;
		break;
	}
	_calculate_limit_rectangle();

	OnSliderValueChanged();
}
void alGUIRangeSlider1::_onTextInputCancel()
{
	m_textInputActive = false;
	m_isClicked = false;
}

alGUIRangeSlider1::alGUIRangeSlider1(alGUIContext* ct)
	:
	alGUIElement(ct)
{
	m_textInput = alCreate<TextInput>(ct);
	m_textInput->m_oneLine = true;
	m_textInput->m_useBottombar = false;
	m_textInput->m_useHorizontalScrollbar = false;
	m_textInput->m_useLinebar = false;
	m_textInput->m_useVerticalScrollbar = false;
	m_textInput->SetUserData(this);

	SetFont(alLib::GetDefaultFont());
	m_old_onEnter_i = 0;
}

alGUIRangeSlider1::~alGUIRangeSlider1()
{
	AL_DESTROY(m_textInput);

	AL_DESTROY(m_meshGPU);
	AL_DESTROY(m_borderMeshGPU);
	AL_DESTROY(m_limitMeshGPU);
}

void alGUIRangeSlider1::_checkLimits()
{
	if (!m_ptr_i)
		return;
	switch (m_type)
	{
	case Type::type_IntLimits:
		if (*m_ptr_i < m_minMax_i[0]) *m_ptr_i = m_minMax_i[0];
		if (*m_ptr_i > m_minMax_i[1]) *m_ptr_i = m_minMax_i[1];
		break;
	case Type::type_FloatLimits:
		if (*m_ptr_f < m_minMax_f[0]) *m_ptr_f = m_minMax_f[0];
		if (*m_ptr_f > m_minMax_f[1]) *m_ptr_f = m_minMax_f[1];
		break;
	case Type::type_Int:
	case Type::type_Float:
	default:
		break;
	}
}

void alGUIRangeSlider1::Draw(float32_t dt)
{
	if (m_clickCount)
	{
		m_clickTimer += dt;
		if (m_clickTimer > 0.4f)
		{
			m_clickTimer = 0.f;
			m_clickCount = 0;
		}
	}

	m_gs->SetScissorRect(m_clipArea);
	OnDraw();
	auto input = alLib::GetInput();

	m_gs->DrawRectangle(
		m_buildArea,
		m_colorTheme->m_slider_bg);

	m_gs->SetScissorRect(m_limitClipRect);
	m_gs->DrawRectangle(
		m_buildArea,
		m_colorTheme->m_slider_color);
	
	if (m_textInputActive)
	{
		m_textInput->Draw(dt);
	}
	else
	{
		if (m_useText)
		{
			auto sz = m_text.size();
			m_gs->SetScissorRect(m_clipArea);

			m_gs->DrawText(
				m_text.data(),
				sz,
				m_font,
				alVec2f(m_buildArea.x, m_buildArea.y),
				m_colorTheme->m_slider_text);
		}
	}
}

void alGUIRangeSlider1::Update(float32_t dt)
{
	auto input = alLib::GetInput();
	alGUIElement::Update(dt);

	if (m_textInputActive)
	{
		m_textInput->m_buildArea = m_buildArea;
		m_textInput->m_clipArea = m_clipArea;
		m_textInput->m_sensorArea = m_sensorArea;

	//	m_textInput->m_buildAreaTransformed = m_buildAreaTransformed;
	//	m_textInput->m_clipAreaTransformed = m_clipAreaTransformed;
	//	m_textInput->m_sensorAreaTransformed = m_sensorAreaTransformed;

	//	m_textInput->m_offset = m_offset;
	//	m_textInput->m_offsetTransformed = m_offsetTransformed;
		m_textInput->Update(dt);
		return;
	}
	
	if (m_context->m_inputTextElementActivated)
		return;
	if (!m_ptr_i)
		return;

	// this is if values was changed from other places for example from callback
	if (m_old_onDrag_i != *m_ptr_i)
	{
		m_old_onDrag_i = *m_ptr_i;
		switch (m_type)
		{
		case Type::type_Int:
		case Type::type_IntLimits:
			if (m_type == Type::type_IntLimits)
			{
				_checkLimits();
				_calculate_limit_rectangle();
			}
			m_text.Clear();
			m_text.Append(*m_ptr_i);
			m_old_onEnter_i = *m_ptr_i;
			break;
		case Type::type_Float:
		case Type::type_FloatLimits:
			if (m_type == Type::type_FloatLimits) {
				_checkLimits();
				_calculate_limit_rectangle();
			}
			m_text.Clear();
			m_text.AppendFloat(*m_ptr_f);
			m_old_onEnter_f = *m_ptr_f;
			break;
		default:
			break;
		}
	}

	static alVec2f cursor_coord_on_click;
	if (m_isCursorInSensorArea)
	{
		if (input->m_isLMBDown)
		{
			m_isClicked = true;
			++m_clickCount;
			//printf("m_clickCount: %u\n", m_clickCount);
			if (m_clickCount > 1)
			{
				m_textInputActive = true;

				m_textInput->m_buildArea = m_buildArea;
				m_textInput->m_clipArea = m_clipArea;
				m_textInput->m_sensorArea = m_sensorArea;

				//m_textInput->m_offset = m_offset;
				//m_textInput->m_offsetTransformed = m_offsetTransformed;

				m_textInput->SetText(m_text.data());
				m_textInput->Activate();
				m_textInput->SelectAll();
				input->m_isLMBDown = false;
			}
			else
			{
				cursor_coord_on_click = input->m_cursorCoordsForGUI;

				alVec4i clp;
				clp.x = cursor_coord_on_click.x - 1.f;
				clp.y = cursor_coord_on_click.y - 1.f;
				clp.z = cursor_coord_on_click.x + 1.f;
				clp.w = cursor_coord_on_click.y + 1.f;
				alLib::SetCursorClip(&clp, 0, m_context->GetWindow());
				alLib::ShowCursor(false);
			}
		}
	}

	if (input->m_isLMBUp && m_isClicked)
	{
		m_isClicked = false;
		//m_clickCount = 0;

		alLib::SetCursorClip(0, 0, 0);
		alLib::ShowCursor(true);
	}

	if (m_isClicked)
	{
		bool need_update = false;
		float32_t value_add = 0;
		float32_t vvv = 1.f;
		float32_t slower = 20.f;
		m_mouseDeltaCounter += input->m_mouseDelta.x;
		if (m_mouseDeltaCounter >= slower)
		{
			need_update = true;
			value_add = vvv;
			m_mouseDeltaCounter = 0.f;
		}
		if (m_mouseDeltaCounter <= -slower)
		{
			need_update = true;
			value_add = -vvv;
			m_mouseDeltaCounter = 0.f;
		}
		switch (m_type)
		{
		case Type::type_Int:
		case Type::type_IntLimits:
			if (m_valueMultiplerNormal < 1.f)
				m_valueMultiplerNormal = 1.f;
			break;
		}
		value_add *= m_valueMultiplerNormal;
		switch (m_type)
		{
		case Type::type_Int:
		case Type::type_IntLimits:
			break;
		case Type::type_Float:
		case Type::type_FloatLimits:
			if (input->m_kbm == alKeyboardModifier::Shift)
				value_add *= m_valueMultiplerShift;
			else if (input->m_kbm == alKeyboardModifier::Alt)
				value_add *= m_valueMultiplerAlt;
			break;
		}
		m_old_onDrag_i = *m_ptr_i;

		if (need_update)
		{
			switch (m_type)
			{
			case Type::type_Int:
			case Type::type_IntLimits:
				*m_ptr_i += (int32_t)value_add;
				if (value_add != 0.f)
				{
					if (m_type == Type::type_IntLimits)
					{
						_checkLimits();
						_calculate_limit_rectangle();
					}
					m_old_onEnter_i = *m_ptr_i;

					m_text.Clear();
					m_text.Append(*m_ptr_i);
					OnSliderValueChanged();
				}
				break;
			case Type::type_Float:
			case Type::type_FloatLimits:
				*m_ptr_f += value_add;

				if (value_add != 0.f)
				{
					if (m_type == Type::type_FloatLimits) {
						_checkLimits();
						_calculate_limit_rectangle();
					}

					m_old_onEnter_f = *m_ptr_f;

					m_text.Clear();
					m_text.AppendFloat(*m_ptr_f);
					OnSliderValueChanged();
				}
				break;
			default:
				break;
			}
		}
	}
}

void alGUIRangeSlider1::Rebuild()
{
	alGUIElement::Rebuild();
	_calculate_limit_rectangle();
}

void alGUIRangeSlider1::SetFont(alGUIFont* f)
{
	m_font = f;
	m_textInput->SetFont(f,f);
}

void alGUIRangeSlider1::_calculate_limit_rectangle()
{
	if (!m_ptr_i)
		return;

	float32_t range_value = 0.f;
	switch (m_type)
	{
	case Type::type_IntLimits:
	{
		float32_t range = (float32_t)m_minMax_i[1] - (float32_t)m_minMax_i[0];
		if (range == 0.f) range = 1.f;
		range_value = ((float32_t)*m_ptr_i - (float32_t)m_minMax_i[0]) * (1.f / range);
	}break;
	case Type::type_FloatLimits:
	{
		float32_t range = m_minMax_f[1] - m_minMax_f[0];
		if (range == 0.f) range = 1.f;
		range_value = (*m_ptr_f - m_minMax_f[0]) * (1.f / range);
	}break;
	}

	//if (m_vertical)
	//{

	//}
	//else
	{
		m_limitClipRect.x = m_buildArea.x;
		m_limitClipRect.y = m_buildArea.y;
		m_limitClipRect.w = m_buildArea.w;
		m_limitClipRect.z = m_buildArea.x + (range_value / (1.f / (m_buildArea.z - m_buildArea.x)));
	}
}

void alGUIRangeSlider1::SetPtri(int32_t* ptr)
{
	m_ptr_i = ptr;
	UpdateText();
	_calculate_limit_rectangle();
}

void alGUIRangeSlider1::SetPtrf(float32_t* ptr)
{
	m_ptr_f = ptr;
	UpdateText();
	_calculate_limit_rectangle();
}

void alGUIRangeSlider1::UpdateText()
{
	if (!m_ptr_i)
		return;

	switch (m_type)
	{
	case Type::type_Int:
	case Type::type_IntLimits:
		m_text.Clear();
		m_text.Append(*m_ptr_i);
		break;
	case Type::type_Float:
	case Type::type_FloatLimits:
		m_text.Clear();
		m_text.AppendFloat(*m_ptr_f);
		break;
	default:
		break;
	}
}

