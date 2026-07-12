#pragma once

#include "al.h"
#include "System/alSystemWindow.h"
#include "GS/alGS.h"
#include "GUI/alGUI.h"




class alDemo;

class Shader_Simple3D : public alGSShaderCallback
{
	alDemo* m_app = 0;
public:
	Shader_Simple3D(alDemo* app) : m_app(app) {}
	virtual ~Shader_Simple3D();
	virtual void OnSetConstants() override;

	bool Create();
	alGSShaderConstantBuffer* m_constantBuffer = 0;
	alGSShader* m_shader = 0;

	struct cbVertex
	{
		alMat4 WVP;
		alMat4 W;
		alVec4f ViewDir;
	}
	m_cbVertexData;

	alGSTexture* m_texture1 = 0;
};


class alDemoExample;
class alDemoDirectory
{
public:
	alDemoDirectory() {}
	~alDemoDirectory()
	{		
	}

	alDemoExample* m_example = 0;

	//alUnicodeString m_description;

	alUnicodeString m_name;

	alDemoDirectory* m_parent = 0;

	size_t m_currEntry = 0;
	
	alArray<alDemoDirectory*> m_entries;

	alDemoDirectory* GetDirByName(const char32_t*);
	void AddExample(alDemoExample*);
};

class alDemoExample
{
public:
	alDemoExample(const char32_t* title, const char32_t* desc)
	{
		m_title = title;
		m_description = desc;
	}
	virtual ~alDemoExample() {}

	virtual bool Init() = 0;
	virtual void Shutdown() = 0;
	virtual bool Run() = 0;
	
	virtual void OnPopupCommand(uint32_t cmd) {}
	virtual void OnWindowSizeChanged() {}

	alUnicodeString m_title;
	alUnicodeString m_description;
};

class SystemWindowCallback;
class alDemo
{
	SystemWindowCallback* m_windowCallback = 0;
	alArray<alDemoExample*> m_examples;

	alDemoExample* m_currExample = 0;
	
	alDemoDirectory m_mainDemoDir;
	alDemoDirectory* m_currDir = 0;
	
	alArray<alDemoExample*> m_allExamples;
	alGUIFont* m_defaultFontMini = 0;

	alInput* m_input = 0;
	alUnicodeString m_directoryNameDrawString;
public:
	alDemo();
	~alDemo();

	alArray<alDemoDirectory*> m_allDirectories;
	
	alGS* m_gs = 0;
	alGUIContext* m_GUI = 0;
	alGUIFont* m_guiFont = 0;
	alGUIFont* m_guiFontMini = 0;
	bool m_run = true;
	float32_t* m_dt = 0;
	alSystemWindow* m_mainWindow = 0;

	Shader_Simple3D m_shaderSimple3D = Shader_Simple3D(this);

	bool Init();
	void Run();
	void Run_demo();

	void AddExample(const char* path, alDemoExample*);

	alUnicodeString m_pathText;
	void UpdatePathText();

	void OnPopupCommand(uint32_t cmd);
	void OnWindowSizeChanged();
};

