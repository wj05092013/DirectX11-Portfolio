#include "LightingHelper.fx"

//
// Constant Buffers
//

cbuffer CBPerObject
{
    float4x4 world;
    float4x4 world_inv_trans;
    
    float4x4 tex_mapping;
    Material material;

	// For skinned objects.
    float4x4 bone_transforms[96];
};
Texture2D diffuse_map;
Texture2D normal_map;

cbuffer CBPerFrame
{
    float3 eye_pos;
    float4x4 view;
    float4x4 shadow_transform;
};
Texture2D shadow_map;
Texture2D ssao_map;

cbuffer CBPerResize
{
    float4x4 proj;
};

cbuffer CBChangeRarely
{
    DirectionalLight directional_lights[3];

    float fog_start;
    float fog_range;
    float4 fog_color;

    float shadow_map_size;

    float4x4 to_tex;
};
TextureCube cube_map;


//
// States
//

SamplerState SSLinear
{
    Filter = MIN_MAG_MIP_LINEAR;

    AddressU = WRAP;
    AddressV = WRAP;
};

SamplerComparisonState SCSShadow
{
    Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
    BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

    ComparisonFunc = LESS;
};


//
// Input Layouts
//

struct VSIn
{
    float3 pos_l : POSITION;
    float3 normal_l : NORMAL;
    float2 tex : TEXCOORD;
    float3 tangent_l : TANGENT;
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
    float3 pos_w : POSITION;
    float3 normal_w : NORMAL;
    float3 tangent_w : TANGENT;
    float2 tex : TEXCOORD0;
    float4 shadow_pos_h : TEXCOORD1;
    float4 screen_tex_h : TEXCOORD2;
};


VSOut VS(VSIn vs_in)
{
    VSOut vs_out;
	
    float4x4 view_proj = mul(view, proj);

    vs_out.pos_h = mul(float4(vs_in.pos_l, 1.0f), mul(world, view_proj));

    vs_out.pos_w = mul(float4(vs_in.pos_l, 1.0f), world).xyz;
    vs_out.normal_w = mul(vs_in.normal_l, (float3x3) world_inv_trans);
    vs_out.tangent_w = mul(vs_in.tangent_l, (float3x3) world);
    vs_out.tex = mul(float4(vs_in.tex, 0.0f, 1.0f), tex_mapping).xy;
    vs_out.shadow_pos_h = mul(float4(vs_out.pos_w, 1.0f), shadow_transform);
    vs_out.screen_tex_h = mul(float4(vs_out.pos_w, 1.0f), mul(view_proj, to_tex));

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
    float3 normal_l = float3(0.0f, 0.0f, 0.0f);
    float3 tangent_l = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 4; ++i)
    {
        pos_l += weights[i] * mul(float4(vs_in.pos_l, 1.0f), bone_transforms[vs_in.bone_indices[i]]).xyz;
        normal_l += weights[i] * mul(vs_in.normal_l, (float3x3) bone_transforms[vs_in.bone_indices[i]]);
        tangent_l += weights[i] * mul(vs_in.tangent_l, (float3x3) bone_transforms[vs_in.bone_indices[i]]);
    }

    VSOut vs_out;

    float4x4 view_proj = mul(view, proj);

    vs_out.pos_h = mul(float4(pos_l, 1.0f), mul(world, view_proj));

    vs_out.pos_w = mul(float4(pos_l, 1.0f), world).xyz;
    vs_out.normal_w = mul(normal_l, (float3x3) world_inv_trans);
    vs_out.tangent_w = mul(tangent_l, (float3x3) world);
    vs_out.tex = mul(float4(vs_in.tex, 0.0f, 1.0f), tex_mapping).xy;
    vs_out.shadow_pos_h = mul(float4(vs_out.pos_w, 1.0f), shadow_transform);
    vs_out.screen_tex_h = mul(float4(vs_out.pos_w, 1.0f), mul(view_proj, to_tex));

    return vs_out;
}


float4 PS(
	VSOut ps_in,
	uniform int lit_cnt,
	uniform bool texture_enabled,
	uniform bool alpha_clip_enabled,
	uniform bool cubemap_reflection_enabled,
	uniform bool fog_enabled
) : SV_Target
{
	// Texture sampling.
    float4 tex_color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    if (texture_enabled)
    {
        tex_color = diffuse_map.Sample(SSLinear, ps_in.tex);

		// Apply alpha clip.
        if (alpha_clip_enabled)
        {
            clip(tex_color.a - 0.1f);
        }
    }

    float3 to_eye = eye_pos - ps_in.pos_w;
    float distance_to_eye = length(to_eye);

    float4 lit_color = tex_color;

	// Apply directional lights.
    if (lit_cnt > 0)
    {
        ps_in.normal_w = normalize(ps_in.normal_w);

        to_eye /= distance_to_eye;

		// Calculate bumped normal.
        float3 sampled_normal = normal_map.Sample(SSLinear, ps_in.tex).rgb;
        float3 bumped_normal = SampledNormalToWorldSpace(sampled_normal, ps_in.normal_w, ps_in.tangent_w);

		// Calculate shadow factor of the first directional light only.
        float3 shadow_factor = float3(1.0f, 1.0f, 1.0f);
        shadow_factor[0] = CalcShadowFactor(shadow_map, shadow_map_size, SCSShadow, ps_in.shadow_pos_h);

		// Sample SSAO map.
        ps_in.screen_tex_h /= ps_in.screen_tex_h.w;
        float ambient_access = ssao_map.SampleLevel(SSLinear, ps_in.screen_tex_h.xy, 0.0f).r;

		// Apply multiple directional lights to each components.
        float4 ambient_res = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 diffuse_res = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 specular_res = float4(0.0f, 0.0f, 0.0f, 0.0f);

        float4 ambient, diffuse, specular;
		
		[unroll]
        for (int i = 0; i < lit_cnt; ++i)
        {
            ComputeDirectionalLight(
				material, directional_lights[i], bumped_normal, to_eye,
				ambient, diffuse, specular
			);
            ambient_res += ambient_access * ambient;
            diffuse_res += shadow_factor[i] * diffuse;
            specular_res += shadow_factor[i] * specular;
        }

		// Apply texture color.
        lit_color = tex_color * (ambient_res + diffuse_res) + specular_res;

		// Apply cube map reflection.
        if (cubemap_reflection_enabled)
        {
            float3 reflection = reflect(-to_eye, bumped_normal);
            float4 reflection_color = cube_map.Sample(SSLinear, reflection);

            float4 t = material.reflection;
            lit_color = (float4(1.0f, 1.0f, 1.0f, 1.0f) - t) * lit_color + t * reflection_color;
        }
    }

	// Apply fog effect.
    if (fog_enabled)
    {
        float fog_lerp = saturate((distance_to_eye - fog_start) / fog_range);

        lit_color = lerp(lit_color, fog_color, fog_lerp);
    }

	// Apply alpha component.
    lit_color.a = material.diffuse.a * tex_color.a;

    return lit_color;
}


//
// Techniques
//
technique11 Light1
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, false, false, false, false)));
    }
}

technique11 Light2
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, false, false, false, false)));
    }
}

technique11 Light3
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, false, false, false, false)));
    }
}


technique11 Light0Tex
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(0, true, false, false, false)));
    }
}

technique11 Light1Tex
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, true, false, false, false)));
    }
}

technique11 Light2Tex
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, true, false, false, false)));
    }
}

technique11 Light3Tex
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, true, false, false, false)));
    }
}

technique11 Light0TexAlphaClip
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(0, true, true, false, false)));
    }
}

technique11 Light1TexAlphaClip
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, true, true, false, false)));
    }
}

technique11 Light2TexAlphaClip
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, true, true, false, false)));
    }
}

technique11 Light3TexAlphaClip
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, true, true, false, false)));
    }
}

technique11 Light1Fog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, false, false, false, true)));
    }
}

technique11 Light2Fog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, false, false, false, true)));
    }
}

technique11 Light3Fog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, false, false, false, true)));
    }
}


technique11 Light0TexFog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(0, true, false, false, true)));
    }
}

technique11 Light1TexFog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, true, false, false, true)));
    }
}

technique11 Light2TexFog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, true, false, false, true)));
    }
}

technique11 Light3TexFog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, true, false, false, true)));
    }
}

technique11 Light0TexAlphaClipFog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(0, true, true, false, true)));
    }
}

technique11 Light1TexAlphaClipFog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, true, true, false, true)));
    }
}

technique11 Light2TexAlphaClipFog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, true, true, false, true)));
    }
}

technique11 Light3TexAlphaClipFog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, true, true, false, true)));
    }
}

technique11 Light1Reflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, false, false, true, false)));
    }
}

technique11 Light2Reflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, false, false, true, false)));
    }
}

technique11 Light3Reflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, false, false, true, false)));
    }
}


technique11 Light0TexReflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(0, true, false, true, false)));
    }
}

technique11 Light1TexReflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, true, false, true, false)));
    }
}

technique11 Light2TexReflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, true, false, true, false)));
    }
}

technique11 Light3TexReflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, true, false, true, false)));
    }
}

technique11 Light0TexAlphaClipReflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(0, true, true, true, false)));
    }
}

technique11 Light1TexAlphaClipReflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, true, true, true, false)));
    }
}

technique11 Light2TexAlphaClipReflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, true, true, true, false)));
    }
}

technique11 Light3TexAlphaClipReflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, true, true, true, false)));
    }
}

technique11 Light1FogReflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, false, false, true, true)));
    }
}

technique11 Light2FogReflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, false, false, true, true)));
    }
}

technique11 Light3FogReflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, false, false, true, true)));
    }
}


technique11 Light0TexFogReflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(0, true, false, true, true)));
    }
}

technique11 Light1TexFogReflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, true, false, true, true)));
    }
}

technique11 Light2TexFogReflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, true, false, true, true)));
    }
}

technique11 Light3TexFogReflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, true, false, true, true)));
    }
}

technique11 Light0TexAlphaClipFogReflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(0, true, true, true, true)));
    }
}

technique11 Light1TexAlphaClipFogReflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, true, true, true, true)));
    }
}

technique11 Light2TexAlphaClipFogReflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, true, true, true, true)));
    }
}

technique11 Light3TexAlphaClipFogReflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, true, true, true, true)));
    }
}

technique11 Light1Skinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, false, false, false, false)));
    }
}

technique11 Light2Skinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, false, false, false, false)));
    }
}

technique11 Light3Skinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, false, false, false, false)));
    }
}


technique11 Light0TexSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(0, true, false, false, false)));
    }
}

technique11 Light1TexSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, true, false, false, false)));
    }
}

technique11 Light2TexSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, true, false, false, false)));
    }
}

technique11 Light3TexSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, true, false, false, false)));
    }
}

technique11 Light0TexAlphaClipSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(0, true, true, false, false)));
    }
}

technique11 Light1TexAlphaClipSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, true, true, false, false)));
    }
}

technique11 Light2TexAlphaClipSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, true, true, false, false)));
    }
}

technique11 Light3TexAlphaClipSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, true, true, false, false)));
    }
}

technique11 Light1FogSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, false, false, false, true)));
    }
}

technique11 Light2FogSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, false, false, false, true)));
    }
}

technique11 Light3FogSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, false, false, false, true)));
    }
}


technique11 Light0TexFogSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(0, true, false, false, true)));
    }
}

technique11 Light1TexFogSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, true, false, false, true)));
    }
}

technique11 Light2TexFogSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, true, false, false, true)));
    }
}

technique11 Light3TexFogSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, true, false, false, true)));
    }
}

technique11 Light0TexAlphaClipFogSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(0, true, true, false, true)));
    }
}

technique11 Light1TexAlphaClipFogSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, true, true, false, true)));
    }
}

technique11 Light2TexAlphaClipFogSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, true, true, false, true)));
    }
}

technique11 Light3TexAlphaClipFogSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, true, true, false, true)));
    }
}

technique11 Light1ReflectSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, false, false, true, false)));
    }
}

technique11 Light2ReflectSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, false, false, true, false)));
    }
}

technique11 Light3ReflectSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, false, false, true, false)));
    }
}


technique11 Light0TexReflectSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(0, true, false, true, false)));
    }
}

technique11 Light1TexReflectSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, true, false, true, false)));
    }
}

technique11 Light2TexReflectSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, true, false, true, false)));
    }
}

technique11 Light3TexReflectSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, true, false, true, false)));
    }
}

technique11 Light0TexAlphaClipReflectSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(0, true, true, true, false)));
    }
}

technique11 Light1TexAlphaClipReflectSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, true, true, true, false)));
    }
}

technique11 Light2TexAlphaClipReflectSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, true, true, true, false)));
    }
}

technique11 Light3TexAlphaClipReflectSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, true, true, true, false)));
    }
}

technique11 Light1FogReflectSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, false, false, true, true)));
    }
}

technique11 Light2FogReflectSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, false, false, true, true)));
    }
}

technique11 Light3FogReflectSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, false, false, true, true)));
    }
}


technique11 Light0TexFogReflectSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(0, true, false, true, true)));
    }
}

technique11 Light1TexFogReflectSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, true, false, true, true)));
    }
}

technique11 Light2TexFogReflectSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, true, false, true, true)));
    }
}

technique11 Light3TexFogReflectSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, true, false, true, true)));
    }
}

technique11 Light0TexAlphaClipFogReflectSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(0, true, true, true, true)));
    }
}

technique11 Light1TexAlphaClipFogReflectSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(1, true, true, true, true)));
    }
}

technique11 Light2TexAlphaClipFogReflectSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(2, true, true, true, true)));
    }
}

technique11 Light3TexAlphaClipFogReflectSkinned
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(3, true, true, true, true)));
    }
}