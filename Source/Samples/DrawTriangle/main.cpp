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
        
    }

    void OnDraw([[maybe_unused]] RHICommandBuffer* commandBuffer) override
    {
    }
};

ALIMER_DEFINE_GAME(TriangleGame);
