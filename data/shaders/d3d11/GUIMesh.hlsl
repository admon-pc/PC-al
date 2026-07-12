//Texture2D tex2d_1;
//SamplerState tex2D_sampler_1;

struct VSIn
{
	float2 position : POSITION;
	float2 uv : TEXCOORD;
};

cbuffer cbVertex
{
	float4x4 ProjMtx;
	float2 Offset;
	float2 Padding;
};

cbuffer cbPixel
{
	float4 Color;
};

struct VSOut
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};

struct PSOut
{
    float4 color : SV_Target;
};

VSOut VSMain(VSIn input)
{
	VSOut output;
	output.pos   = mul(ProjMtx, float4(input.position.x + Offset.x, input.position.y + Offset.y, 0.0f, 1.f));
	output.uv    = input.uv;
	return output;
}

PSOut PSMain(VSOut input)
{
    PSOut output;
    output.color = Color;
    return output;
}
