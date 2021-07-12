// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "Math/Viewport.h"
#include "Core/String.h"
#include <spdlog/fmt/fmt.h>

namespace Alimer
{
    std::string Viewport::ToString() const
    {
        return fmt::format("{} {} {} {} {} {}", x, y, width, height, minDepth, maxDepth);
    }
}
