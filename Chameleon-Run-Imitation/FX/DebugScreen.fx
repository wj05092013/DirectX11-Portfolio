
//
// Constant Buffers
//

cbuffer CBPerObject
{
    float4x4 proj;
};

Texture2D debug_tex;


//
// States
//

SamplerState SSLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};


//
// Input Layout
//

struct VSIn
{
    float3 pos_l : POSITION;
    float3 normal_l : NORMAL;
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

    vs_out.pos_h = mul(float4(vs_in.pos_l, 1.0f), proj);
    vs_out.tex = vs_in.tex;

    return vs_out;
}

float4 PS(VSOut ps_in) : SV_Target
{
    return debug_tex.Sample(SSLinear, ps_in.tex);
}

float4 PS(VSOut ps_in, uniform uint index) : SV_Target
{
    float c = debug_tex.Sample(SSLinear, ps_in.tex)[index];

    return float4(c, c, c, 1.0f);
}


//
// Techniques
//

technique11 ViewRGBA
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}

technique11 ViewRed
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(0)));
    }
}

technique11 ViewGreen
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1)));
    }
}

technique11 ViewBlue
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2)));
    }
}

technique11 ViewAlpha
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3)));
    }
}