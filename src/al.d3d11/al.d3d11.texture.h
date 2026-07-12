#ifndef _ALD3D11T_H_
#define _ALD3D11T_H_

class alD3D11Texture : public alGSTexture
{
public:
	alD3D11Texture();
	virtual ~alD3D11Texture();

	//virtual void Reload() override;
	//virtual void Unload() override;

	void OnCreate();

	ID3D11Texture2D* m_texture = 0;
	ID3D11ShaderResourceView* m_textureResView = 0;
	ID3D11SamplerState* m_samplerState = 0;

	// for RTT
	ID3D11RenderTargetView* m_RTV = 0;
	ID3D11Texture2D* m_depthStencilBuffer = 0;
	ID3D11DepthStencilView* m_depthStencilView = 0;
};

#endif
