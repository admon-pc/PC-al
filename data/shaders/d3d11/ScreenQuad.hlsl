Texture2D tex2d_1; 
SamplerState tex2D_sampler_1; 
Texture2D tex2d_gui;
SamplerState tex2D_sampler_gui;

//cbuffer cbVertex
//{
//	float4x4 ProjMtx;
//	float4 Corners;
//	float4 Color1;
//	float4 Color2;
//	float4 UVs;
//};

struct VSIn
{
//	float3 position : POSITION;
//	float4 color : COLOR;
	   float2 position : POSITION;
		float2 uv : TEXCOORD;
}; 

struct VSOut
{
//	float4 pos : SV_POSITION;
//	float2 uv : TEXCOORD0;
//	float4 color : COLOR0;
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
	output.pos   = float4(input.position.x, input.position.y, 0.5f, 1.f); 
	output.uv = input.uv;
//	output.uv    = float2(0.f, 0.f);
//	output.color = float4(1.f, 0.f, 0.f, 1.f);
	return output; 
}

PSOut PSMain(VSOut input)
{
	PSOut output; 
	float4 sceneTexel = tex2d_1.Sample(tex2D_sampler_1, input.uv);
	float4 guiTexel = tex2d_gui.Sample(tex2D_sampler_gui, input.uv);
	float guiAlpha = guiTexel.w;

	output.color = lerp(sceneTexel, guiTexel, guiAlpha);
	
	// TRY THIS FOR TEXT!!!
	//output.color.xyz = saturate(sceneTexel.xyz + (guiTexel.xyz * guiAlpha));
	//output.color.w = 1.f;

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
