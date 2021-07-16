// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Graphics/Types.h"

namespace Alimer
{
	class Graphics;

    /// Base class for objects that allocate GPU object.
    class ALIMER_API GPUObject
    {
    public:
        /// Destructor. 
        virtual ~GPUObject() = default;

        /// Unconditionally destroy the GPU resource.
        virtual void Destroy() = 0;

        /// Get the object name.
        const String& GetName() const { return name; }

        /// Set the object name.
        void SetName(const String& name_) { name = name_; ApiSetName(); }

    protected:
        /// Constructor. 
        GPUObject() = default;

        void OnCreated();
        void OnDestroyed();
        virtual void ApiSetName() {}

        String name;
    };

	/// Base class for objects that allocate GPU resources.
	class ALIMER_API GPUResource : public GPUObject
	{
	public:
		enum class Type
		{
			Buffer,
			Texture,
		};

		/// Unconditionally destroy the GPU resource.
		virtual void Destroy() = 0;

	protected:
		/// Constructor. 
		GPUResource(Type type);

		Type _type;
		uint64_t allocatedSize{ 0 };
	};
}
