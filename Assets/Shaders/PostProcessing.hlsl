//
//  PostProcessing.hlsl
//  Boardeerlands
//
//  Copyright 2020 by SlinDev. All rights reserved.
//

SamplerState linearRepeatSampler;
Texture2D framebufferTexture;

cbuffer vertexUniforms
{
	float4 diffuseColor;
	float4 ambientColor;
};

struct InputVertex
{
	[[vk::location(0)]] float3 position : POSITION;
	[[vk::location(5)]] float2 texCoords : TEXCOORD0;
};

struct FragmentVertex
{
	float4 position : SV_POSITION;
	float2 texCoords : TEXCOORD0;
};

FragmentVertex pp_vertex(InputVertex vert)
{
	FragmentVertex result;
	result.position = float4(vert.position, 1.0f).xyww;
	result.texCoords = vert.texCoords * diffuseColor.zw + diffuseColor.xy;

	return result;
}

float4 pp_blit_fragment(FragmentVertex vert) : SV_TARGET
{
	float4 color = framebufferTexture.Sample(linearRepeatSampler, vert.texCoords).rgba * ambientColor.rgba;
	return color;
}
