// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "AlimerConfig.h"
#include "Core/RefPtr.h"
//#include "Core/Callback.h"

namespace Alimer
{
	RefCounted::RefCounted()
		: refCount(new RefCount())
	{
		// Hold a weak ref to self to avoid possible double delete of the refcount
		(void)refCount->weakRefs.fetch_add(+1, std::memory_order_relaxed);
	}

	RefCounted::~RefCounted()
	{
		ALIMER_ASSERT(refCount != nullptr);
		ALIMER_ASSERT(refCount->refs == 0);
		ALIMER_ASSERT(refCount->weakRefs > 0);

		//Scripting::Callback(CALLBACK_ADD_DELETE, this);

		// Mark object as expired, release the self weak ref and delete the refcount if no other weak refs exist
		refCount->refs = -1;

		if (refCount->weakRefs.fetch_add(-1, std::memory_order_relaxed) == 0)
		{
			delete refCount;
		}

		refCount = nullptr;
	}

	int32_t RefCounted::AddRef()
	{
		(void)refCount->refs.fetch_add(+1, std::memory_order_relaxed);

		ALIMER_ASSERT(Refs() > 0);
		//Scripting::Callback(CALLBACK_ADD_REF, this);

		return Refs();
	}

	int32_t RefCounted::Release()
	{
		(void)refCount->refs.fetch_add(-1, std::memory_order_relaxed);

        const int32_t refs = Refs();

		ALIMER_ASSERT(refs >= 0);
		if (refs == 0)
		{
			delete this;
            return 0;
		}

		return refs;
	}

	int32_t RefCounted::Refs() const
	{
		return refCount->refs.load(std::memory_order_relaxed);
	}

	int32_t RefCounted::WeakRefs() const
	{
		// Subtract one to not return the internally held reference
		return refCount->weakRefs.load(std::memory_order_relaxed) - 1;
	}
}
