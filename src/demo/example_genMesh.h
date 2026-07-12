#pragma once


class DemoExample_genMesh : public alDemoExample
{
	alGS* m_gs = 0;
	alInput* m_input = 0;

	alGSTexture* m_texture = 0;

	alGUIPanel* m_GUIPanel = 0;

	alGSMesh* m_meshCube = 0;
	alGSMesh* m_meshSphere = 0;
	alGSMesh* m_meshTriangles = 0;

	float32_t m_angle = 0.f;

public:
	DemoExample_genMesh(const char32_t* title, const char32_t* desc)
	:
		alDemoExample(title, desc)
	{}
	virtual ~DemoExample_genMesh() {}

	virtual bool Init() override;
	virtual void Shutdown() override;

	virtual bool Run() override;
};

