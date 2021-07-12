// Copyright (c) Amer Koleci and contributors.
// Distributed under the MIT license. See the LICENSE file in the project root for more information.

#include <Alimer.h>
using namespace Alimer;

class TriangleGame final : public Game
{
private:
public:
    TriangleGame()
    {
        //config.title = "Triangle";
    }

    void Initialize() override
    {
        auto usage = RHITextureUsage::ShaderReadWrite | RHITextureUsage::RenderTarget;
        auto descriptor = RHITextureDescription::TextureCube(PixelFormat::RGBA8UNorm, 256, 1, 1, usage);
        descriptor.name = "CUBEMAP";
        auto texture = RHICreateTexture(descriptor);
        auto view = texture->GetView({});

        float vertices[] = {
            /* positions        colors */
             0.0f, 0.5f, 0.5f,      1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f
        };

        RHIBufferDescription bufferDesc = {};
        bufferDesc.size = sizeof(vertices);
        bufferDesc.usage = RHIBufferUsage::Vertex;
        auto vertexBuffer = RHICreateBuffer(bufferDesc, vertices);
    }

    void OnDraw([[maybe_unused]] RHICommandBuffer* commandBuffer) override
    {
    }
};

ALIMER_DEFINE_GAME(TriangleGame);
