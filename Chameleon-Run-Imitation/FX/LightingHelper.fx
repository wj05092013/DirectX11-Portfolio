//
// 3 Kinds Of Lights
//

struct DirectionalLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;

    float3 direction;
    float pad;
};

struct PointLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;

    float3 position;
    float range;

    float3 attenuation;
    float pad;
};

struct SpotLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;

    float3 position;
    float range;

    float3 direction;
    float spot;

    float3 attenuation;
    float pad;
};


//
// Material
//

struct Material
{
    float4 ambient;
    float4 diffuse;
    float4 specular;		// 'w' is shininess.
    float4 reflection;
};


//
// Functions To Compute The 3 Components Of Light.
//

void ComputeDirectionalLight(
	Material mat, DirectionalLight light,
	float3 normal, float3 to_eye,
	out float4 ambient,
	out float4 diffuse,
	out float4 specular
)
{
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    ambient = light.ambient * mat.ambient;

    float3 to_light = -light.direction;

    float diffuse_factor = dot(to_light, normal);

	[flatten]
    if (diffuse_factor > 0.0f)
    {
        diffuse = diffuse_factor * light.diffuse * mat.diffuse;

        float3 reflect_vec = reflect(light.direction, normal);
        float specular_factor = pow(max(dot(reflect_vec, to_eye), 0.0f), mat.specular.w);

        specular = specular_factor * light.specular * mat.specular;
    }
}

void ComputePointLight(
	Material mat, PointLight light,
	float3 pos, float3 normal, float3 to_eye,
	out float4 ambient,
	out float4 diffuse,
	out float4 specular
)
{
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float3 to_light = light.position - pos;
    float dist = length(to_light);

    if (light.range < dist)
        return;

    to_light /= dist;

    ambient = light.ambient * mat.ambient;

    float diffuse_factor = dot(to_light, normal);

	[flatten]
    if (diffuse_factor > 0.0f)
    {
        diffuse = diffuse_factor * light.diffuse * mat.diffuse;

        float3 reflect_vec = reflect(-to_light, normal);
        float specular_factor = pow(max(dot(reflect_vec, to_eye), 0.0f), light.specular.w);

        specular = specular_factor * light.specular * mat.specular;
    }

    float attenuation = 1.0f / dot(light.attenuation, float3(1.0f, dist, dist * dist));

    diffuse *= attenuation;
    specular *= attenuation;
}

void ComputeSpotLight(
	Material mat, SpotLight light,
	float3 pos, float3 normal, float3 to_eye,
	out float4 ambient,
	out float4 diffuse,
	out float4 specular
)
{
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float3 to_light = light.position - pos;

    float dist = length(to_light);

    if (light.range < dist)
        return;

    to_light /= dist;

    float diffuse_factor = dot(to_light, normal);

	[flatten]
    if (diffuse_factor > 0.0f)
    {
        diffuse = diffuse_factor * light.diffuse * mat.diffuse;

        float3 reflect_vec = reflect(-to_light, normal);
        float specular_factor = pow(max(dot(reflect_vec, to_eye), 0.0f), mat.specular.w);

        specular = specular_factor * light.specular * mat.specular;
    }

    float spot_factor = pow(max(dot(-to_light, light.direction), 0.0f), light.spot);
    float attenuation = spot_factor / dot(light.attenuation, float3(1.0f, dist, dist * dist));

    ambient *= spot_factor;
    diffuse *= attenuation;
    specular = attenuation;
}


// Calculate a shadow factor on 'shadow_pos_h'.
//
float CalcShadowFactor(
	Texture2D shadow_map,
	float shadow_map_size,
	SamplerComparisonState state,
	float4 shadow_pos_h
)
{
    float3 shadow_pos = shadow_pos_h.xyz / shadow_pos_h.w;
    float depth = shadow_pos.z;

    float dx = 1.0f / shadow_map_size;
    float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx, dx), float2(0.0f, dx), float2(dx, dx)
    };

    float result = 0.0f;
	[unroll]
    for (int i = 0; i < 9; ++i)
    {
        result += shadow_map.SampleCmpLevelZero(state, shadow_pos.xy + offsets[i], depth).r;
    }

    return result / 9.0f;
}
//__


// Transform a sampled normal vector from texture space to world space.
//
float3 SampledNormalToWorldSpace(
	float3 sampled_normal,
	float3 unit_basis_n_w,
	float3 basis_t_w
)
{
    float3 normal_t = 2.0f * sampled_normal - 1.0f;

    float3 n = unit_basis_n_w;
    float3 t = normalize(basis_t_w - dot(basis_t_w, n) * n);
    float3 b = cross(n, t);

    float3x3 to_world = float3x3(t, b, n);

    return mul(normal_t, to_world);
}
//__
