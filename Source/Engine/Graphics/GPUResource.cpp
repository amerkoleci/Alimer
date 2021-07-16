// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "Graphics/GPUResource.h"
#include "Graphics/Graphics.h"
#include "Core/Log.h"

namespace Alimer
{
    void GPUObject::OnCreated()
    {
        gGraphics().AddGPUObject(this);
    }

    void GPUObject::OnDestroyed()
    {
        gGraphics().RemoveGPUObject(this);
    }

	GPUResource::GPUResource(Type type_)
		: type(type_)
	{

	}
}
