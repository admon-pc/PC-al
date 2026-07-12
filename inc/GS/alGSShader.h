#ifndef _AL_GSSDRH_
#define _AL_GSSDRH_

class alGSShaderCallback
{
public:
	alGSShaderCallback() {}
	virtual ~alGSShaderCallback() {}
	
	virtual void OnSetShader() {}
	virtual void OnSetConstants() {}
};

struct alGSShaderCreationInfo
{
	alMeshVertexType m_vertexType = alMeshVertexType::Null;
	alGSShaderCallback* m_callback = 0;

	const char* m_shaderFile_GS = 0;
	const char* m_shaderFile_VS = 0;
	const char* m_shaderFile_PS = 0;

	const char* m_shaderEntry_GS = 0;
	const char* m_shaderEntry_VS = 0;
	const char* m_shaderEntry_PS = 0;

	const char* m_shaderModel_GS = 0;
	const char* m_shaderModel_VS = 0;
	const char* m_shaderModel_PS = 0;

	const char* m_saveShaderToFile_GS = 0;
	const char* m_saveShaderToFile_VS = 0;
	const char* m_saveShaderToFile_PS = 0;
};

class alGSShaderConstantBuffer
{
public:
	alGSShaderConstantBuffer() {}
	virtual ~alGSShaderConstantBuffer() {}

	virtual void MapData(void* data, uint32_t dataSize) = 0;
	virtual void VSSetConstantBuffers(uint32_t slot) = 0;
	virtual void PSSetConstantBuffers(uint32_t slot) = 0;
	virtual void GSSetConstantBuffers(uint32_t slot) = 0;
};

// For simplicity, one class containes Vertex Pixel And Geom shaders.
class alGSShader
{
public:
	alGSShader() {}
	virtual ~alGSShader() {}

	virtual alGSShaderConstantBuffer* CreateConstantBuffer(uint32_t size) = 0;
	virtual void SetTexture(alGSTexture*, uint32_t slot) = 0;
};

#endif

