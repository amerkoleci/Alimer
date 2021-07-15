#include "Common.hlsli"

struct VSInput
{
	float3 Position : ATTRIBUTE0;
	float4 Color : ATTRIBUTE1;
};

struct VSOutput
{
	float4 Position : SV_POSITION;
  	float4 Color : COLOR0;
};

VSOutput VSMain(VSInput input)
{
	VSOutput output = (VSOutput)0;
	output.Position = float4(input.Position.xyz, 1.0);
	output.Color = input.Color;
	return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
  return input.Color;
}