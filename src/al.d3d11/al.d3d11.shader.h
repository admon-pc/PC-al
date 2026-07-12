#ifndef _ALD3D11SDR_H_
#define _ALD3D11SDR_H_

class alD3D11GSShaderConstantBuffer : public alGSShaderConstantBuffer
{
public:
	alD3D11GSShaderConstantBuffer();
	virtual ~alD3D11GSShaderConstantBuffer();

	virtual void MapData(void* data, uint32_t dataSize) override;
	virtual void VSSetConstantBuffers(uint32_t slot) override;
	virtual void PSSetConstantBuffers(uint32_t slot) override;
	virtual void GSSetConstantBuffers(uint32_t slot) override;

	ID3D11Buffer* m_cb = 0;
};

class alD3D11Shader : public alGSShader
{
	alArray<alD3D11GSShaderConstantBuffer*> m_constantBuffers;
public:
	alD3D11Shader();
	virtual ~alD3D11Shader();
	virtual alGSShaderConstantBuffer* CreateConstantBuffer(uint32_t size) override;
	virtual void SetTexture(alGSTexture*, uint32_t slot) override;

	bool Create(const alGSShaderCreationInfo&);

	ID3D11VertexShader* m_vShader = 0;
	ID3D11PixelShader* m_pShader = 0;
	ID3D11GeometryShader* m_gShader = 0;
	ID3D11InputLayout* m_vLayout = 0;
	alGSShaderCreationInfo m_info;
};

#endif
