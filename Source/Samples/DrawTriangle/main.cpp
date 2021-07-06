// Copyright (c) Amer Koleci and contributors.
// Distributed under the MIT license. See the LICENSE file in the project root for more information.

#include <Alimer.h>
using namespace Alimer;

class TriangleApp final : public Application
{
private:
public:
    TriangleApp()
        : Application()
    {
        //config.title = "Triangle";
    }

    void Initialize() override
    {
        
    }

    void OnDraw() override
    {
    }
};

ALIMER_DEFINE_APPLICATION(TriangleApp);
