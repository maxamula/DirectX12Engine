struct VSOutput
{
	noperspective float4 pos : SV_POSITION;
	noperspective float2 tex : TEXCOORD0;
	
};

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}