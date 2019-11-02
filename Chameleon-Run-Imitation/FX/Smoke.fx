
//
// Constant Buffers
//

cbuffer CBPerFrame
{
    float3 eye_pos;
    float4x4 view;

    float3 emit_pos;

    float game_time;
    float delta_time;
};

cbuffer CBPerResize
{
    float4x4 proj;
};

cbuffer CBChangeRarely
{
    int tex_arr_size;
};
Texture2DArray tex_arr;
Texture1D rand_tex; // All components of texels have a value in the range [-1, 1].

cbuffer CBFixed
{
    float3 kAccelerationW = { 0.0f, 0.2f, 0.0f };

    float2 kQuadTexCrd[4] =
    {
        float2(0.0f, 0.0f),
		float2(0.0f, 1.0f),
		float2(1.0f, 0.0f),
		float2(1.0f, 1.0f)
    };

    float kEmitTimeInterval = 0.1f;
    float kMaxLifeTime = 3.0f;

    float2 kPtcSize = float2(3.0f, 3.0f);

    float kPtcInitVelScale = 1.0f;
    float kPtcInitVelScaleX = 0.1f;
    float kPtcInitVelScaleY = 0.1f;
    float kPtcInitVelScaleZ = 0.1f;

	// Types of particles.
	//
    uint kTypeEmitter = 0;
    uint kTypeSmoke = 1;
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

DepthStencilState DSDisableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};

DepthStencilState DSDisableDepthWrite
{
    DepthEnable = TRUE;
    DepthWriteMask = ZERO;
};

BlendState BSAdditiveBlending
{
    AlphaToCoverageEnable = FALSE;
	
    BlendEnable[0] = TRUE;
    SrcBlend[0] = SRC_ALPHA;
    DestBlend[0] = ONE;
    BlendOp[0] = ADD;
    SrcBlendAlpha[0] = ZERO;
    DestBlendAlpha[0] = ZERO;
    BlendOpAlpha[0] = ADD;
    RenderTargetWriteMask[0] = 0x0f;
};

//
// Helpers
//

// Return value is in the range [-1, 1].
float RandF(float offset)
{
    float u = game_time + offset;
    float r = rand_tex.SampleLevel(SSLinear, u, 0.0f).x;

    return r;
}

// Return value is in the range [min, max].
float RandF(float min, float max, float offset)
{
    float r = RandF(offset);
    r = 0.5f * (r + 1.0f);
    r = r * (max - min) + min;

    return r;
}

float3 RandUnitVec3(float offset)
{
    float u = game_time + offset;
    float3 vec = rand_tex.SampleLevel(SSLinear, u, 0.0f).xyz;

    return normalize(vec);
}

//
// Input Layouts
//

struct Particle
{
    float3 init_pos_w : POSITION;
    float3 init_vel_w : VELOCITY;
    float2 size_w : SIZE;
    float age : AGE;
    uint type : TYPE;
};


//
// Shaders for Stream Out
//

Particle StreamOutVS(Particle vs_in)
{
    return vs_in;
}

[maxvertexcount(2)]
void StreamOutGS
(
	point Particle gs_in[1],
	inout PointStream<Particle> stream
)
{
    gs_in[0].age += delta_time;

    if (gs_in[0].type == kTypeEmitter)
    {
        if (gs_in[0].age > kEmitTimeInterval)
        {
			// Create a new particle.
            float3 vel = RandUnitVec3(0.0f);
            vel.x *= kPtcInitVelScaleX;
            vel.y *= kPtcInitVelScaleY;
            vel.z *= kPtcInitVelScaleZ;
            vel *= kPtcInitVelScale;
			
            Particle p;
            p.init_pos_w = emit_pos;
            p.init_vel_w = vel;
            p.size_w = kPtcSize;
            p.age = 0.0f;
            p.type = kTypeSmoke;

            stream.Append(p);
			//__

            gs_in[0].age = 0.0f;
        }

        stream.Append(gs_in[0]);
    }
    else
    {
		// Maintain this particle.
        if (gs_in[0].age <= kMaxLifeTime)
            stream.Append(gs_in[0]);

		// Destroy this particle if its life time was ended.
    }
}

GeometryShader gs_so = ConstructGSWithSO(
	CompileShader(gs_5_0, StreamOutGS()),
	"POSITION.xyz; VELOCITY.xyz; SIZE.xy; AGE.x; TYPE.x"
);

//
// Stream Out Technique
//

technique11 StreamOutTech
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, StreamOutVS()));
        SetGeometryShader(gs_so);

        SetPixelShader(NULL);
		
        SetDepthStencilState(DSDisableDepth, 0);
    }
}

//
// Shaders for Drawing Particle
//

struct VSOut
{
    float3 pos_w : POSITION;
    float2 size_w : SIZE;
    float age : AGE;
    uint type : TYPE;
};

VSOut DrawingVS(Particle vs_in)
{
    VSOut vs_out;

	// A particle's position is depend on its life time.
    float t = vs_in.age;
    vs_out.pos_w = 0.5f * t * t * kAccelerationW + t * vs_in.init_vel_w + vs_in.init_pos_w;

    vs_out.size_w = vs_in.size_w;
    vs_out.age = vs_in.age;
    vs_out.type = vs_in.type;

    return vs_out;
}

struct GSOut
{
    float4 pos_h : SV_POSITION;
    float2 tex : TEXCOORD;
    float age : AGE;
};

[maxvertexcount(4)]
void DrawingGS(
	point VSOut gs_in[1],
	inout TriangleStream<GSOut> stream
)
{
	// Emitter is not drawn.
    if (gs_in[0].type != kTypeEmitter)
    {
        float3 to_eye = normalize(eye_pos - gs_in[0].pos_w);
        float3 right = normalize(cross(float3(0.0f, 1.0f, 0.0f), to_eye));
        float3 up = cross(to_eye, right);

        float half_width = 0.5f * gs_in[0].size_w.x;
        float half_height = 0.5f * gs_in[0].size_w.y;

        float4 pos[4];
        pos[0] = float4(gs_in[0].pos_w + half_width * right + half_height * up, 1.0f);
        pos[1] = float4(gs_in[0].pos_w - half_width * right + half_height * up, 1.0f);
        pos[2] = float4(gs_in[0].pos_w + half_width * right - half_height * up, 1.0f);
        pos[3] = float4(gs_in[0].pos_w - half_width * right - half_height * up, 1.0f);

        GSOut gs_out;
		[unroll]
        for (int i = 0; i < 4; ++i)
        {
            gs_out.pos_h = mul(pos[i], mul(view, proj));
            gs_out.tex = kQuadTexCrd[i];
            gs_out.age = gs_in[0].age;

            stream.Append(gs_out);
        }
    }
}

float4 DrawingPS(GSOut ps_in) : SV_Target
{
    return tex_arr.Sample(SSLinear, float3(ps_in.tex, tex_arr_size * ps_in.age / kMaxLifeTime));
}

//
// Drawing Particle Technique
//

technique11 DrawingTech
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, DrawingVS()));
        SetGeometryShader(CompileShader(gs_5_0, DrawingGS()));
        SetPixelShader(CompileShader(ps_5_0, DrawingPS()));

        SetBlendState(BSAdditiveBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetDepthStencilState(DSDisableDepthWrite, 0);
    }
}