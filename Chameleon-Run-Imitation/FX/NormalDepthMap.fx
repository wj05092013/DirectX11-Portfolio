
//
// Constant Buffers
//

cbuffer CBPerObect
{
    float4x4 world;
    float4x4 world_inv_trans;
    float4x4 tex_mapping;
};
Texture2D diffuse_map;

cbuffer CBPerFrame
{
    float4x4 view;
};

cbuffer CBPerResize
{
    float4x4 proj;
};


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
    float3 pos_v : POSITION;
    float3 normal_v : NORMAL;
    float2 tex : TEXCOORD;
};

VSOut VS(VSIn vs_in)
{
    VSOut vs_out;

    float4x4 world_view = mul(world, view);
    float4x4 world_view_proj = mul(world_view, proj);

    vs_out.pos_h = mul(float4(vs_in.pos_l, 1.0f), world_view_proj);
    vs_out.pos_v = mul(float4(vs_in.pos_l, 1.0f), world_view).xyz;
    vs_out.normal_v = mul(vs_in.normal_l, (float3x3) mul(world_inv_trans, view));
    vs_out.tex = mul(float4(vs_in.tex, 0.0f, 1.0f), tex_mapping).xy;

    return vs_out;
}


float4 PS(VSOut ps_in, uniform bool alpha_clip_enabled) : SV_Target
{
    if (alpha_clip_enabled)
    {
        float a = diffuse_map.Sample(SSLinear, ps_in.tex).a;
        clip(a - 0.1f);
    }

    ps_in.normal_v = normalize(ps_in.normal_v);

    return float4(ps_in.normal_v, ps_in.pos_v.z);
}


//
// Techniques
//

technique11 NormalDepthMap
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(false)));
    }
}

technique11 NormalDepthMapAlphaClip
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(true)));
    }
}
