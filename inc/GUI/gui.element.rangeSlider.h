#ifndef _AL_GUIRANGESLIDERH_
#define _AL_GUIRANGESLIDERH_

class alGUIRangeSlider1 : public alGUIElement
{
	class TextInput : public alGUITextInput
	{
	public:
		TextInput(alGUIContext* ct) :
			alGUITextInput(ct)
		{}
		virtual ~TextInput() {}

		virtual void OnRMBRelease() override;
		virtual void OnAccept() override;
		virtual void OnCancel() override;
		virtual bool OnCharacter(char32_t c)override;
	};
	TextInput* m_textInput = 0;
	bool m_textInputActive = false;
	void _onTextInputAccept();
	void _onTextInputCancel();

protected:
	alGUIFont* m_font = 0;
	union
	{
		float32_t m_old_onEnter_f;
		int32_t m_old_onEnter_i; // search this if you need
	};

	union
	{
		float32_t m_old_onDrag_f;
		int32_t m_old_onDrag_i; // search this if you need

	};
	void _checkLimits();
	alGSMesh* m_meshGPU = nullptr;
	alGSMesh* m_borderMeshGPU = nullptr;
	alGSMesh* m_limitMeshGPU = nullptr;

	uint32_t m_clickCount = 0;
	float32_t m_clickTimer = 0;

	alUnicodeString m_text;
	alVec4f m_limitClipRect;
	void _calculate_limit_rectangle();

	float32_t m_mouseDeltaCounter = 0.f;

	
	bool m_isClicked = false;
public:
	alGUIRangeSlider1(alGUIContext* ct);
	virtual ~alGUIRangeSlider1();

	virtual void Draw(float32_t dt) override;
	virtual void Update(float32_t dt) override;
	virtual void Rebuild() override;

	void SetFont(alGUIFont*);
	bool m_useText = true;
	bool m_useTextInput = true;

	float32_t m_valueMultiplerNormal = 0.1f;
	float32_t m_valueMultiplerShift = 10.f;
	float32_t m_valueMultiplerAlt = 0.01f;

	union
	{
		float32_t m_minMax_f[2];
		int32_t m_minMax_i[2];
	};

	union
	{
		float32_t* m_ptr_f;
		int32_t* m_ptr_i;
	};
	void SetPtri(int32_t*);
	void SetPtrf(float32_t*);
	void UpdateText();

	enum class Type
	{
		type_IntLimits,
		type_FloatLimits,
		type_Int,
		type_Float,
	};

	Type m_type = Type::type_Int;

	virtual void OnSliderValueChanged() {}

};



#endif

