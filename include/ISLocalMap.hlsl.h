R"(
// This is only the pixel shader part of the shader

float RGBToLuminanceAlternative(float3 color)
{
    return dot(color, float3(0.3, 0.59, 0.11));
}

struct PS_INPUT
{
    float4 Position : SV_POSITION0;
    float2 TexCoord : TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 Color : SV_Target0;
};

SamplerState ImageSampler : register(s0);

Texture2D<float4> ImageTex : register(t0);

cbuffer PerGeometry : register(b2)
{
	float4 TexelSize : packoffset(c0);
};

PS_OUTPUT main(PS_INPUT input)
{
	PS_OUTPUT output;

    float3 weight = float3(0.5, 0.25, 1);
    float3 colorLR = 0;
    float3 colorBT = 0;
	[unroll]
    for (int j = -1; j <= 1; ++j)
    {
		[unroll]
        for (int i = -1; i <= 1; ++i)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }

            float3 currentColor =
				ImageTex.Sample(ImageSampler, input.TexCoord + float2(i * TexelSize.x, j * TexelSize.y))
					.xyz;

            float centerMul = 1;
            if (i == 0 || j == 0)
            {
                centerMul = 2;
            }

            colorLR += -i * (centerMul * weight) * currentColor;
            colorBT += -j * (centerMul * weight) * currentColor;
        }
    }

    float4 colorCC = ImageTex.Sample(ImageSampler, input.TexCoord);
    float luminance = RGBToLuminanceAlternative(colorCC.xyz);

    float alpha = (dot(4 * (pow(colorLR, 2) + pow(colorBT, 2)), 1.75) + luminance);
    
#ifndef NO_FOG_OF_WAR
    alpha *= 1 - colorCC.w;
#endif
    
    float2 coord = 2 * abs(input.TexCoord - 0.5);
    
#ifndef ROUND
    float2 edgeFadeFactor = 1 - pow(2 * abs(input.TexCoord - 0.5), 5);
#else
    float2 edgeFadeFactor = clamp(1 - pow(coord.x * coord.x + coord.y * coord.y, 7), 0, 1);
#endif
	
#ifdef COLOR
    output.Color.xyz = colorCC.xyz;
#else
    output.Color.xyz = 1.04 * luminance;
#endif
    output.Color.w = alpha * edgeFadeFactor.x * edgeFadeFactor.y;

    return output;
})"