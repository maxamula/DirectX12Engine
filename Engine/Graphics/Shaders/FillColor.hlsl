float4 main(in noperspective float4 Position : SV_Position, in noperspective float2 UV : TEXCOORD) : SV_Target0
{
    float4 color = float4(1.0f, 0.0f, 1.0f, 1.0f);
    return color;
}