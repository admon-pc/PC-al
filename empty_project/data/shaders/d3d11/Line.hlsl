struct VSIn
{
	uint vertexID : SV_VertexID;
};

cbuffer cbVertex  : register(b0)
{
	double4x4 VP;
	double4 P1;
	double4 P2;
	float4 Color;
};

struct VSOut
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
};

struct PSOut
{
    float4 color : SV_Target;
};

VSOut VSMain(VSIn input)
{
	double4 vertices[2] =
	{
		double4( P1.xyz, 1.0),
		double4( P2.xyz, 1.0)
	};
	VSOut output;
	output.pos   = mul(VP, vertices[input.vertexID]);
	output.color = Color;
	return output;
}

PSOut PSMain(VSOut input)
{
    PSOut output;
    output.color = input.color;
    return output;
}
