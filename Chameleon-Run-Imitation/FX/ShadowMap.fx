
//
// Constant Buffers
//

cbuffer CBPerObject
{
    float4x4 world;
    float4x4 world_inv_trans;
	
    float4x4 tex_mapping;

	// For Skinned objects.
    float4x4 bone_transforms[96];
};
Texture2D diffuse_map;

cbuffer CBPerFrame
{
    float4x4 light_view;
	float4x4 light_proj;
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

RasterizerState RSDepthBias
{
    DepthBias = 5000;
    DepthBiasClamp = 0.0f;
    SlopeScaledDepthBias = 1.5f;
};


//
// Input Layouts
//

struct VSIn
{
    float3 pos_l : POSITION;
    float3 normal_l : NORMAL;
    float2 tex : TEXCOORD;
};

struct SkinnedVSIn
{
    float3 pos_l : POSITION;
    float3 normal_l : NORMAL;
    float2 tex : TEXCOORD;
    float3 tangent_l : TANGENT;
    float3 blend_weights : WEIGHTS;
    uint4 bone_indices : BONEINDICES;
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

    vs_out.pos_h = mul(float4(vs_in.pos_l, 1.0f), mul(mul(world, light_view), light_proj));
    vs_out.tex = mul(float4(vs_in.tex, 0.0f, 1.0f), tex_mapping).xy;

    return vs_out;
}

VSOut SkinnedVS(SkinnedVSIn vs_in)
{
    float4 weights;
    weights[0] = vs_in.blend_weights[0];
    weights[1] = vs_in.blend_weights[1];
    weights[2] = vs_in.blend_weights[2];
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

    float3 pos_l = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 4; ++i)
    {
        pos_l += weights[i] * mul(float4(vs_in.pos_l, 1.0f), bone_transforms[vs_in.bone_indices[i]]).xyz;
    }

    VSOut vs_out;

    vs_out.pos_h = mul(float4(pos_l, 1.0f), mul(mul(world, light_view), light_proj));
    vs_out.tex = mul(float4(vs_in.tex, 0.0f, 1.0f), tex_mapping).xy;

    return vs_out;
}

void PS(VSOut ps_in)
{
    float a = diffuse_map.Sample(SSLinear, ps_in.tex).a;

    clip(a - 0.15f);
}


//
// Techniques For Normal Objects
//

technique11 BuildShadowMap
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(NULL);
	
        SetRasterizerState(RSDepthBias);
    }
}

technique11 BuildShadowMapAlphaClip
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));

        SetRasterizerState(RSDepthBias);
    }
}


//
// Techniques For Skinned Objects
//
technique11 BuildShadowMapSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(NULL);
	
        SetRasterizerState(RSDepthBias);
    }
}

technique11 BuildShadowMapAlphaClipSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));

        SetRasterizerState(RSDepthBias);
    }
}