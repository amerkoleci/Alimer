// Copyright (c) Amer Koleci and contributors.
// Distributed under the MIT license. See the LICENSE file in the project root for more information.

#include <Alimer.h>
#include <Graphics/ShaderCompiler.h>
using namespace Alimer;

class TriangleGame final : public Game
{
private:
    GPUBuffer vertexBuffer;
    RHIShader vertexShader;
    RHIShader pixelShader;
    InputLayout inputLayout;
    PipelineState renderPipeline;

public:
    TriangleGame()
    {
        config.title = "Triangle";
        //config.backendType = RHIBackendType::Vulkan;
    }

    void Initialize() override
    {
#if TODO
        //{
//    RHISamplerDescription samplerDesc{};
//    RHISampler sampler;
//    GRHIDevice->CreateSampler(&samplerDesc, &sampler);
//}
//
//{
//    RHISamplerDescription samplerDesc{};
//    RHISampler sampler;
//    GRHIDevice->CreateSampler(&samplerDesc, &sampler);
//}

        auto usage = RHITextureUsage::ShaderReadWrite | RHITextureUsage::RenderTarget;
        auto descriptor = RHITextureDescription::TextureCube(PixelFormat::RGBA8UNorm, 256, 1, 1, usage);
        descriptor.name = "CUBEMAP";
        auto texture = RHICreateTexture(descriptor);
        auto view = texture->GetView({});
#endif // TODO

        float vertices[] = {
            /* positions        colors */
             0.0f, 0.5f, 0.5f,      1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f
        };

        BufferDescriptor bufferDesc = {};
       // bufferDesc.label = "Triangle VertexBuffer";
        bufferDesc.size = sizeof(vertices);
        bufferDesc.usage = BufferUsage::Vertex;
        ALIMER_ASSERT(GDevice->CreateBuffer(&bufferDesc, vertices, &vertexBuffer));

        ShaderCompiler::Compile(ShaderStage::Vertex, "Assets/Shaders/triangle.hlsl", &vertexShader);
        ShaderCompiler::Compile(ShaderStage::Pixel, "Assets/Shaders/triangle.hlsl", &pixelShader);

        inputLayout.elements =
        {
            { "ATTRIBUTE", 0, FORMAT_R32G32B32_FLOAT, 0, InputLayout::APPEND_ALIGNED_ELEMENT, INPUT_PER_VERTEX_DATA },
            { "ATTRIBUTE", 1, FORMAT_R32G32B32A32_FLOAT, 0, InputLayout::APPEND_ALIGNED_ELEMENT, INPUT_PER_VERTEX_DATA },
        };

        PipelineStateDesc renderPipelineDesc;
        renderPipelineDesc.vertex = &vertexShader;
        renderPipelineDesc.pixel = &pixelShader;
        renderPipelineDesc.il = &inputLayout;
        ALIMER_ASSERT(GDevice->CreatePipelineState(&renderPipelineDesc, &renderPipeline));
    }

    void OnDraw([[maybe_unused]] CommandList& commandBuffer) override
    {
        const GPUBuffer* vbs[] = {
            &vertexBuffer,
        };
        const uint32_t strides[] = {
            28,
        };

        GDevice->BindVertexBuffers(vbs, 0, 1, strides, nullptr, commandBuffer);
        GDevice->BindPipelineState(&renderPipeline, commandBuffer);
        GDevice->Draw(3, 0, commandBuffer);
    }
};

ALIMER_DEFINE_GAME(TriangleGame);
