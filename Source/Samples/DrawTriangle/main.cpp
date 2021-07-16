// Copyright (c) Amer Koleci and contributors.
// Distributed under the MIT license. See the LICENSE file in the project root for more information.

#include <Alimer.h>
#include <Graphics/ShaderCompiler.h>
using namespace Alimer;

class TriangleGame final : public Game
{
private:
    BufferRef vertexBuffer;
    ShaderRef vertexShader;
    ShaderRef pixelShader;
    PipelineRef renderPipeline;
    SamplerRef sampler;

public:
    TriangleGame()
    {
        config.title = "Triangle";
        config.backendType = GPUBackendType::Vulkan;
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

        sampler = Sampler::Create({});

        float vertices[] = {
            /* positions        colors */
             0.0f, 0.5f, 0.5f,      1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f
        };

        vertexBuffer = Buffer::Create(vertices, BufferUsage::Vertex, sizeof(vertices));

        vertexShader = ShaderCompiler::Compile(ShaderStage::Vertex, "Assets/Shaders/triangle.hlsl");
        pixelShader = ShaderCompiler::Compile(ShaderStage::Pixel, "Assets/Shaders/triangle.hlsl");

        RenderPipelineStateCreateInfo pipelineInfo = {};
        pipelineInfo.label = "Triangle";
        pipelineInfo.vertexShader = vertexShader;
        pipelineInfo.fragmentShader = pixelShader;
        pipelineInfo.vertexLayout.attributes[0].format = VertexFormat::Float3;
        pipelineInfo.vertexLayout.attributes[1].format = VertexFormat::Float4;

        pipelineInfo.colorFormats[0] = GetWindow()->GetSwapChain()->GetColorFormat();
        //pipelineInfo.depthStencilFormat = mainWindow->GetDepthStencilFormat();
        renderPipeline = Pipeline::Create(pipelineInfo);
    }

    void OnDraw([[maybe_unused]] CommandBuffer* commandBuffer) override
    {
        commandBuffer->SetPipeline(renderPipeline);
        commandBuffer->SetVertexBuffer(0, vertexBuffer);
        commandBuffer->Draw(0, 3);
    }
};

ALIMER_DEFINE_GAME(TriangleGame);
