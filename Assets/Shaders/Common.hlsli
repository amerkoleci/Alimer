#ifndef _COMMON_SHADERS_H
#define _COMMON_SHADERS_H

#ifdef SPIRV

#define PUSH_CONSTANT(type, name) [[vk::push_constant]] type name;

#else /* DXIL */

#define PUSH_CONSTANT(type, name) ConstantBuffer<type> name : register(b999)

#endif

// Bindless descriptors
Texture2D Texture2DTable[] : register(t0, space1);
//ByteAddressBuffer SRVBuffers[] : register(t0, space1);
//Texture2DArray Tex2DArrayTable[] : register(space2);

/* Samplers */ 
SamplerState LinearSampler : register(s0);


#endif // _COMMON_SHADERS_H