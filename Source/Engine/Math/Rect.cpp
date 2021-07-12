// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "Math/Rect.h"
#include "Core/String.h"

namespace Alimer
{
	const Rect Rect::Empty = { 0, 0, 0, 0 };

	Int2 Rect::Location() const noexcept
	{
		return Int2(x, y);
	}

	Int2 Rect::Center() const noexcept
	{
		return Int2(x + (width / 2), y + (height / 2));
	}

	Rect Rect::Intersect(const Rect& a, const Rect& b) noexcept
	{
		int32_t righta = a.x + a.width;
		int32_t rightb = b.x + b.width;

		int32_t bottoma = a.y + a.height;
		int32_t bottomb = b.y + b.height;

		int32_t maxX = a.x > b.x ? a.x : b.x;
		int32_t maxY = a.y > b.y ? a.y : b.y;

		int32_t minRight = righta < rightb ? righta : rightb;
		int32_t minBottom = bottoma < bottomb ? bottoma : bottomb;

		if ((minRight > maxX) && (minBottom > maxY))
		{
			return Rect(maxX, maxY, minRight - maxX, minBottom - maxY);
		}
		
		return Empty;
	}

	Rect Rect::Union(const Rect& a, const Rect& b) noexcept
	{
		int32_t righta = a.x + a.width;
		int32_t rightb = b.x + b.width;

		int32_t bottoma = a.y + a.height;
		int32_t bottomb = b.y + b.height;

		int32_t minX = a.x < b.x ? a.x : b.x;
		int32_t minY = a.y < b.y ? a.y : b.y;

		int32_t maxRight = righta > rightb ? righta : rightb;
		int32_t maxBottom = bottoma > bottomb ? bottoma : bottomb;

		return Rect(minX, minY, maxRight - minX, maxBottom - minY);
	}

	String Rect::ToString() const
	{
        return fmt::format("{} {} {} {}", x, y, width, height);
	}

    size_t Rect::ToHash() const
    {
        size_t hash = 0;
        Alimer::HashCombine(hash, x, y, width, height);
        return hash;
    }

	const RectF RectF::Empty = { 0.0f, 0.0f, 0.0f, 0.0f };

	Vector2 RectF::Location() const noexcept
	{
		return Vector2(x, y);
	}

	Vector2 RectF::Center() const noexcept
	{
		return Vector2(x + (width / 2.0f), y + (height / 2.0f));
	}

	RectF RectF::Intersect(const RectF& a, const RectF& b) noexcept
	{
		float righta = a.x + a.width;
		float rightb = b.x + b.width;

		float bottoma = a.y + a.height;
		float bottomb = b.y + b.height;

		float maxX = a.x > b.x ? a.x : b.x;
		float maxY = a.y > b.y ? a.y : b.y;

		float minRight = righta < rightb ? righta : rightb;
		float minBottom = bottoma < bottomb ? bottoma : bottomb;

		if ((minRight > maxX) && (minBottom > maxY))
		{
			return RectF(maxX, maxY, minRight - maxX, minBottom - maxY);
		}

		return Empty;
	}

	RectF RectF::Union(const RectF& a, const RectF& b) noexcept
	{
		float righta = a.x + a.width;
		float rightb = b.x + b.width;

		float bottoma = a.y + a.height;
		float bottomb = b.y + b.height;

		float minX = a.x < b.x ? a.x : b.x;
		float minY = a.y < b.y ? a.y : b.y;

		float maxRight = righta > rightb ? righta : rightb;
		float maxBottom = bottoma > bottomb ? bottoma : bottomb;

		return RectF(minX, minY, maxRight - minX, maxBottom - minY);
	}

	String RectF::ToString() const
	{
        return fmt::format("{} {} {} {}", x, y, width, height);
	}


    size_t RectF::ToHash() const
    {
        size_t hash = 0;
        Alimer::HashCombine(hash, x, y, width, height);
        return hash;
    }
}
