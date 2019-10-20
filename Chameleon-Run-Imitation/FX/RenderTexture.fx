
//
// Constant Buffers
//

cbuffer CBPerObject
{
    float4x4 tex_mapping;
};
Texture2D input_image;


//
// States
//

SamplerState SSInputImage
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;

    AddressU = WRAP;
    AddressV = WRAP;
};


//
// Input Layouts
//

struct VSIn
{
    float3 pos_ndc : POSITION;
    float3 unused : NORMAL;
    float2 tex : TEXCOORD;
};


//
// Shaders
//

struct VSOut
{
    float4 pos_h : SV_POSITION;
    float2 tex : TEXCOORD;
};

VSOut VS(VSIn vs_in)
{
    VSOut vs_out;

    vs_out.pos_h = float4(vs_in.pos_ndc, 1.0f);
    vs_out.tex = mul(float4(vs_in.tex, 0.0f, 1.0f), tex_mapping).xy;

    return vs_out;
}

float4 PS(VSOut ps_in) : SV_Target
{
    float4 color = input_image.Sample(SSInputImage, ps_in.tex);
    
	clip(color.a - 0.1f);

    return color;
}


//
// Technique
//

technique11 RenderTexture
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}