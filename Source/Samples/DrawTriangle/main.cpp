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

public:
    TriangleGame()
    {
        config.title = "Triangle";
        //config.backendType = GPUBackendType::Vulkan;
    }

    void Initialize() override
    {
        auto usage = TextureUsage::ShaderRead | TextureUsage::RenderTarget;
        auto descriptor = TextureCreateInfo::Tex2D(PixelFormat::Depth32Float, 256, 256, 1, 1, usage);
        descriptor.label = "DepthTexture";
        auto texture = Texture::Create(descriptor);
        //auto view = texture->GetView({});

        float vertices[] = {
            /* positions        colors */
             0.0f, 0.5f, 0.5f,      1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f
        };

        vertexBuffer = Buffer::Create(vertices, BufferUsage::InputAssembly, sizeof(vertices));

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
