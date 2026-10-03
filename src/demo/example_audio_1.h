#pragma once

class DemoExample_audio_1 : public alDemoExample
{
	alGS* m_gs = 0;
	alInput* m_input = 0;

	struct _audio_data
	{
		alAudioBuffer* m_buffer = 0;
		alAudioObject* m_audioObject = 0;
	};
	alArray<_audio_data> m_audioBuffers;

	alAudioObject* m_audioObject = 0;
	alAudioObject* m_audioObject2 = 0;

public:
	DemoExample_audio_1(const char32_t* title, const char32_t* desc)
	:
		alDemoExample(title, desc)
	{}
	virtual ~DemoExample_audio_1() {}

	virtual bool Init() override;
	virtual void Shutdown() override;
	virtual bool Run() override;

	class _combo : public alGUIComboBox
	{
	public:
		_combo(alGUIContext* ct, const alVec2f& position, const alVec2f& size)
			:alGUIComboBox(ct, position, size) {}
		virtual ~_combo() {}
		virtual void OnComboSelectItem(size_t) override;
	};

	_combo* m_combo1 = 0;
	_combo* m_combo2 = 0;

	struct directory_files
	{
		uint32_t m_flags = 0;
		uint32_t m_importantData[4];
		char32_t m_name[100];
		uint32_t m_importantData2[4];

	};
	alArray<directory_files> m_dirFiles;
	alGUIPanel* m_GUIPanel = 0;
};

