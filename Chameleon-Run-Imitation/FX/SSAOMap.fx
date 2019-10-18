
//
// Constant Buffers
//

cbuffer CBFixed
{
    static const float kOcclusionRadius = 0.5f;

    static const float kOcclusionFadeStart = 0.2f;
    static const float kOcclusionFadeEnd = 2.0f;
    static const float kSurfaceEpsilon = 0.05f;

    static const float kContrastExp = 4.0f;
};

cbuffer CBPerFrame
{
    float4 far_plane_corners[4];
    float4 offset_vectors[14];
};

cbuffer CBPerResize
{
    float4x4 proj;
};

cbuffer CBChangeRarely
{
    float4x4 to_tex;
};
Texture2D normal_depth_map;
Texture2D random_vector_map;


//
// States
//

SamplerState SSNormalDepthMap
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;

    AddressU = BORDER;
    AddressV = BORDER;

	// Sample a very far depth value on the outside of the normal-depth map.
    BorderColor = float4(0.0f, 0.0f, 0.0f, 1e5f);
};

SamplerState SSRandomVectorMap
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;
    AddressU = WRAP;
};


//
// Helper Functions
//

float CalcOcclusion(float dist)
{
    if (dist <= kSurfaceEpsilon)
        return 0.0f;

    return saturate((kOcclusionFadeEnd - dist) / (kOcclusionFadeEnd - kOcclusionFadeStart));
}


//
// Input Layouts
//

struct VSIn
{
    float3 pos_ndc : POSITION;
    float3 corner_idx : NORMAL;
    float2 tex : TEXCOORD;
};


//
// Shaders
//

struct VSOut
{
    float4 pos_h : SV_POSITION;
    float3 to_far_plane : TEXCOORD0;
    float2 tex : TEXCOORD1;
};

VSOut VS(VSIn vs_in)
{
    VSOut vs_out;

    vs_out.pos_h = float4(vs_in.pos_ndc, 1.0f);
    vs_out.to_far_plane = far_plane_corners[vs_in.corner_idx.x].xyz;
    vs_out.tex = vs_in.tex;

    float4 a = 3.0f;

    return vs_out;
}

float4 PS(VSOut ps_in, uniform int sample_count) : SV_Target
{
    float4 normal_depth = normal_depth_map.Sample(SSNormalDepthMap, ps_in.tex);
    float3 normal = normal_depth.xyz;
    float p_z = normal_depth.w;

    float t = p_z / ps_in.to_far_plane.z;
    float3 p = t * ps_in.to_far_plane;

	// Map the range [0, 1] to [-1, 1].
    float3 random_vector = 2.0f * random_vector_map.Sample(SSRandomVectorMap, ps_in.tex).xyz - 1.0f;

    float occlusion_sum = 0.0f;

	[unroll]
    for (int i = 0; i < sample_count; ++i)
    {
        float3 offset = reflect(offset_vectors[i].xyz, random_vector);
        float flip = sign(dot(offset, normal));

        float3 q = p + flip * kOcclusionRadius * offset;

        float4 tex_q = mul(float4(q, 1.0f), mul(proj, to_tex));
        tex_q /= tex_q.w;

        float r_z = normal_depth_map.Sample(SSNormalDepthMap, tex_q.xy).w;
        float3 r = (r_z / q.z) * q;

        //float dist = length(p - r);
        float dist = p.z - r.z;
        float scale = max(dot(normalize(r - p), normal), 0.0f);

        occlusion_sum += scale * CalcOcclusion(dist);
    }

    float access = 1.0f - occlusion_sum / float(sample_count);

    return saturate(pow(access, kContrastExp));
}


//
// Technique
//

technique11 SSAOMap
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(14)));
    }
}