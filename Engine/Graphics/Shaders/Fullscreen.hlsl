struct VSOutput
{
	noperspective float4 Position : SV_Position;
	noperspective float2 UV : TEXCOORD;
};

VSOutput main(in uint VertexIdx : SV_VertexID)
{
	VSOutput output;
	float2 tex;
	float2 pos;
	if (VertexIdx == 0)
	{
		tex = float2 (0.0f, 0.0f);
		pos = float2 (-1.0f, 1.0f);
	}
	else if (VertexIdx == 1)
	{
		tex = float2 (0.0f, 0.4f);
		pos = float2 (-1.0f, -1.0f);
	}
	else if (VertexIdx == 2)
	{
		tex = float2 (0.4f, 0.0f);
		pos = float2 (1.0f, 1.0f);
	}
	output.Position = float4(pos, 0, 1);
	output.UV = tex;
	return output;
}