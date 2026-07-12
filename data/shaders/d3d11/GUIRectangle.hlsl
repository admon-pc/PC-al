cbuffer cbVertex
{
	float4x4 ProjMtx;
	float4 Corners;
	float4 Color1;
	float4 Color2;
	float4 UVs;
};

struct VSIn
{
   float3 position : POSITION;
   float4 color : COLOR;
};

struct VSOut
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
};

struct PSOut
{
	float4 color : SV_Target;
};

VSOut VSMain(VSIn input)
{
	VSOut output;
	output.pos   = float4(0.f,0.f,0.f,0.f);
	output.uv = float2(0.f,0.f);
	output.color = float4(0.f,0.f,0.f,0.f);
	return output;
}

[maxvertexcount(4)]
void GSMain(point VSOut input[1], inout TriangleStream<VSOut> TriStream )
{
   float4 v1 = mul(ProjMtx, float4(Corners.x, Corners.y, 0.f, 1.f) );
   float4 v2 = mul(ProjMtx, float4(Corners.z, Corners.w, 0.f, 1.f) );

	VSOut Out;

	Out.pos   = float4(v2.x, v1.y, 0.f, 1.f);
	Out.uv = float2(UVs.z,UVs.y);
	Out.color = Color1;
	TriStream.Append(Out);

	Out.pos   = float4(v2.x, v2.y, 0.f, 1.f);
	Out.uv = float2(UVs.z,UVs.w);
	Out.color = Color2;
	TriStream.Append(Out);

	Out.pos   = float4(v1.x, v1.y, 0.f, 1.f);
	Out.uv = float2(UVs.x,UVs.y);
	Out.color = Color1;
	TriStream.Append(Out);

	Out.pos   = float4(v1.x, v2.y, 0.f, 1.f);
	Out.uv = float2(UVs.x,UVs.w);
	Out.color = Color2;
	TriStream.Append(Out);

	TriStream.RestartStrip();

	return;
}

Texture2D tex2d_1;
SamplerState tex2D_sampler_1;
PSOut PSMain(VSOut input)
{
    PSOut output;
    float4 textureColor = tex2d_1.Sample(tex2D_sampler_1, input.uv);
    
	//output.color = float4(input.color.xyz, 1.f) * textureColor;
    //output.color.w = input.color.w * textureColor.w;

	float alpha = textureColor.w;
	output.color = lerp(textureColor, float4(input.color.xyz, 1.f), alpha);
	output.color.xyz = output.color.xyz * textureColor.xyz;

    return output;
}

