Texture2D tex2d_1; 
SamplerState tex2D_sampler_1; 
Texture2D tex2d_2;
SamplerState tex2D_sampler_2;

cbuffer cbVertex
{
	double4x4 WVP;
	double4x4 W;

	float4 ViewDir;
};

struct VSIn
{
	float3 position : POSITION;
	float2 uv1 : TEXCOORD0;
	float2 uv2 : TEXCOORD1;
	float3 normal : NORMAL;
	float3 binormal : BINORMAL;
	float3 tangent : TANGENT;
	float4 color : COLOR;
}; 

struct VSOut
{
//	float4 pos : SV_POSITION;
//	float2 uv : TEXCOORD0;
//	float4 color : COLOR0;
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	//float2 uvSpheremap : TEXCOORD1;
	float3 normal : NORMAL0;
//	float4 fragPos : NORMAL1;
}; 

struct PSOut
{
	float4 color : SV_Target; 
};



VSOut VSMain(VSIn input)
{
	VSOut output; 
	output.pos   = mul(WVP, double4(input.position.x, input.position.y, input.position.z, 1.0));
//	output.fragPos = mul(W, float4(input.position.x, input.position.y, input.position.z, 1.f));
	output.uv = input.uv1;
	output.normal = mul((float3x3)W, input.normal);

//	output.uv    = float2(0.f, 0.f);
//	output.color = float4(1.f, 0.f, 0.f, 1.f);

	

	

	return output; 
}

PSOut PSMain(VSOut input)
{
	float3 u = normalize(ViewDir.xyz);
	float3 r = reflect(u, input.normal);
	float m = 2.0 * sqrt(r.x * r.x + r.y * r.y + (r.z + 1.0) * (r.z + 1.0));

	float2 uvSpheremap;
	uvSpheremap.x = r.x / m + 0.5;
	uvSpheremap.y = 1.f - (r.y / m + 0.5);

	float4 spheremap = saturate(tex2d_2.Sample(tex2D_sampler_2, uvSpheremap) * 0.11f);

	PSOut output;
	float3 SunPosition = normalize(float3(3.1f, 4.f, 0.f));

	float3 lightDir = normalize(SunPosition.xyz - ViewDir.xyz);
	float diff = max(dot(input.normal, lightDir), 0.0);

	output.color = tex2d_1.Sample(tex2D_sampler_1, input.uv);
	output.color.xyz += spheremap.xyz;
	output.color.xyz *= diff * 1.21f;

	return output; 
}

//[maxvertexcount(4)] 
//void GSMain(point VSOut input[1], inout TriangleStream<VSOut> TriStream )
//{
//	float4 v1 = float4(Corners.x, Corners.y, 0.f, 1.f); 
//	float4 v2 = float4(Corners.z, Corners.w, 0.f, 1.f); 
//	VSOut Out; 
//	Out.pos   = float4(v2.x, v1.y, 0.f, 1.f); 
//	Out.uv = float2(UVs.z,UVs.y); 
//	Out.color = float4(1.f, 0.f, 0.f, 1.f);
//	TriStream.Append(Out); 
//
//	Out.pos   = float4(v2.x, v2.y, 0.f, 1.f); 
//	Out.uv = float2(UVs.z,UVs.w); 
//	Out.color = float4(1.f, 0.f, 0.f, 1.f);
//	TriStream.Append(Out); 
//
//	Out.pos   = float4(v1.x, v1.y, 0.f, 1.f); 
//	Out.uv = float2(UVs.x,UVs.y); 
//	Out.color = Color1; 
//	TriStream.Append(Out); 
//
//	Out.pos   = float4(v1.x, v2.y, 0.f, 1.f); 
//	Out.uv = float2(UVs.x,UVs.w); 
//	Out.color = Color2; 
//	TriStream.Append(Out); 
//
//	TriStream.RestartStrip(); 
//
//	return; 
//} 
