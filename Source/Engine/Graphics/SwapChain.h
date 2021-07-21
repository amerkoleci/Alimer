// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/RefPtr.h"
#include "Graphics/Texture.h"

namespace Alimer
{
    struct SwapChainCreateInfo {
        const char* label = nullptr;
        uint32_t width = 0;
        uint32_t height = 0;
        PixelFormat colorFormat = PixelFormat::BGRA8UNormSrgb;
        bool verticalSync = true;
        bool fullscreen = false;
    };

	class ALIMER_API SwapChain : public GPUObject, public RefCounted
	{
	public:
        /// Create new SwapChain
        [[nodiscard]] static SwapChainRef Create(void* window, const SwapChainCreateInfo& info);

		void Resize(uint32_t width, uint32_t height);

		/// Gets the current texture (null if not available or minimized).
		virtual Texture* GetCurrentTexture() const = 0;

		/// Gets the backbuffer color format.
		PixelFormat GetColorFormat() const { return colorFormat; }

	private:
		virtual void ResizeBackBuffer(uint32_t width, uint32_t height) = 0;

	protected:
		/// Constructor.
		SwapChain(const SwapChainCreateInfo& info);

        uint32_t width;
        uint32_t height;
		PixelFormat colorFormat;
		bool verticalSync;
	};
}
