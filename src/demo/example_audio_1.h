#pragma once

class DemoExample_audio_1 : public alDemoExample
{
	alGS* m_gs = 0;
	alInput* m_input = 0;
	alAudio* m_audio = 0;
public:
	DemoExample_audio_1(const char32_t* title, const char32_t* desc)
	:
		alDemoExample(title, desc)
	{}
	virtual ~DemoExample_audio_1() {}

	virtual bool Init() override;
	virtual void Shutdown() override;
	virtual bool Run() override;
};

