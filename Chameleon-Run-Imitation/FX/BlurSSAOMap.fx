
//
// Constant Buffers
//

cbuffer CBFixed
{
    static const float kWeights[11] =
    {
        0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f
    };

    static const int kBlurRadius = 5;
};

cbuffer CBPerResize
{
    float texel_width;
    float texel_height;
};

Texture2D normal_depth_map;
Texture2D input_image;


//
// States
//

SamplerState SSNormalDepthMap
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;

    AddressU = CLAMP;
    AddressV = CLAMP;
};

SamplerState SSInputImage
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;

    AddressU = CLAMP;
    AddressV = CLAMP;
};


//
// Input Layout
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
    vs_out.tex = vs_in.tex;

    return vs_out;
}

float4 PS(VSOut ps_in, uniform bool horizontal_blur) : SV_Target
{
    float2 tex_offset;

    if (horizontal_blur)
        tex_offset = float2(texel_width, 0.0f);
    else
        tex_offset = float2(0.0f, texel_height);

    float4 color = kWeights[5] * input_image.Sample(SSInputImage, ps_in.tex);
    float total_weight = kWeights[5];

    float4 center_normal_depth = normal_depth_map.Sample(SSNormalDepthMap, ps_in.tex);
	
    for (int i = -kBlurRadius; i <= kBlurRadius; ++i)
    {
        if (i == 0)
            continue;

        float2 tex = ps_in.tex + i * tex_offset;
        float4 neighbor_normal_depth = normal_depth_map.Sample(SSNormalDepthMap, tex);

        if (dot(neighbor_normal_depth.xyz, center_normal_depth.xyz) >= 0.8f &&
		    abs(neighbor_normal_depth.w - center_normal_depth.w) <= 0.2f)
        {
            float weight = kWeights[i + kBlurRadius];
            color += weight * input_image.Sample(SSInputImage, tex);

            total_weight += weight;
        }
    }

    return color / total_weight;
}


//
// Techniques
//

technique11 HorizontalBlur
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(true)));
    }
}

technique11 VerticalBlur
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(false)));
    }
}