#define D3D_SHADER_REQUIRES_RESOURCE_HEAP_INDEXING  0x02000000

struct Params
{
	uint mainBufferIndex;
};

ConstantBuffer<Params> params : register(b1);


float4 main(in noperspective float4 Position : SV_Position, in noperspective float2 UV : TEXCOORD) : SV_Target0
{
    Texture2D Texture = ResourceDescriptorHeap[params.mainBufferIndex];
    return float4(Texture[Position.xy].xyz, 1.0f);
}