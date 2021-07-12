// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Math/Vector2.h"

namespace Alimer
{
	struct ALIMER_API Rect
	{
	public:
		/// Specifies the x-coordinate of the rectangle.
		int32_t x;

		/// Specifies the y-coordinate of the rectangle.
		int32_t y;

		/// Specifies the width of the extent.
		int32_t width;

		/// Specifies the height of the extent.
		int32_t height;

		constexpr Rect() noexcept
			: x(0)
			, y(0)
			, width(0)
			, height(0)
		{
		}

		constexpr Rect(int32_t x_, int32_t y_, int32_t width_, int32_t height_) noexcept
			: x(x_)
			, y(y_)
			, width(width_)
			, height(height_)
		{
		}

		constexpr Rect(int32_t width_, int32_t height_) noexcept
			: x(0)
			, y(0)
			, width(width_)
			, height(height_)
		{
		}

		constexpr Rect(const Int2& location, const Int2& size) noexcept
			: x(location.x)
			, y(location.y)
			, width(size.x)
			, height(size.y)
		{
		}

		Rect(const Rect&) = default;
		Rect& operator=(const Rect&) = default;

		Rect(Rect&&) = default;
		Rect& operator=(Rect&&) = default;

		bool operator ==(const Rect& r) const noexcept { return (x == r.x) && (y == r.y) && (width == r.width) && (height == r.height); }
		bool operator != (const Rect& r) const noexcept { return (x != r.x) || (y != r.y) || (width != r.width) || (height != r.height); }

		int32_t Left() const noexcept { return x; }
		int32_t Right() const noexcept { return x + width;; }
		int32_t Top() const noexcept { return y; }
		int32_t Bottom() const noexcept { return y + height; }

		/// Gets a value that indicates whether the rectangle is empty.
		bool IsEmpty() const noexcept { return (width == 0 && height == 0 && x == 0 && y == 0); }

		// Rectangle operations
		Int2 Location() const noexcept;
		Int2 Center() const noexcept;

		void Offset(int32_t offsetX, int32_t offsetY) noexcept
		{
			x += offsetX;
			y += offsetY;
		}

		void Inflate(int32_t horizAmount, int32_t vertAmount) noexcept
		{
			x -= horizAmount;
			y -= vertAmount;
			width += horizAmount;
			height += vertAmount;
		}

		bool Contains(int32_t ix, int32_t iy) const noexcept { return (x <= ix) && (ix < (x + width)) && (y <= iy) && (iy < (y + height)); }
		bool Contains(const Int2& point) const noexcept { return (x <= point.x) && (point.x < (x + width)) && (y <= point.y) && (point.y < (y + height)); }
		bool Contains(const Rect& rhs) const noexcept { return (x <= rhs.x) && ((rhs.x + rhs.width) <= (x + width)) && (y <= rhs.y) && ((rhs.y + rhs.height) <= (y + height)); }

		bool Intersects(const Rect& r) const noexcept
		{
			return (r.x < (x + width)) && (x < (r.x + r.width)) && (r.y < (y + height)) && (y < (r.y + r.height));
		}

		static Rect Intersect(const Rect& a, const Rect& b) noexcept;
		static Rect Union(const Rect& a, const Rect& b) noexcept;

		/// Return as string.
		String ToString() const;

        /// Return hash value of the rectangle.
        size_t ToHash() const;

		// Constants
		static const Rect Empty;
	};

	struct ALIMER_API RectF
	{
	public:
		/// Specifies the x-coordinate of the rectangle.
		float x;

		/// Specifies the y-coordinate of the rectangle.
		float y;

		/// Specifies the width of the extent.
		float width;

		/// Specifies the height of the extent.
		float height;

		constexpr RectF() noexcept
			: x(0.0f)
			, y(0.0f)
			, width(0.0f)
			, height(0.0f)
		{
		}

		constexpr RectF(float x_, float y_, float width_, float height_) noexcept
			: x(x_)
			, y(y_)
			, width(width_)
			, height(height_) 
		{
		}

		constexpr RectF(float width_, float height_) noexcept
			: x(0.0f)
			, y(0.0f)
			, width(width_)
			, height(height_)
		{
		}

		constexpr RectF(const Vector2& location, const Vector2& size) noexcept
			: x(location.x)
			, y(location.y)
			, width(size.x)
			, height(size.y)
		{
		}

		RectF(const RectF&) = default;
		RectF& operator=(const RectF&) = default;

		RectF(RectF&&) = default;
		RectF& operator=(RectF&&) = default;

		bool operator ==(const RectF& r) const noexcept { return (x == r.x) && (y == r.y) && (width == r.width) && (height == r.height); }
		bool operator != (const RectF& r) const noexcept { return (x != r.x) || (y != r.y) || (width != r.width) || (height != r.height); }

		float Left() const noexcept { return x; }
		float Right() const noexcept { return x + width;; }
		float Top() const noexcept { return y; }
		float Bottom() const noexcept { return y + height; }

		/// Gets a value that indicates whether the rectangle is empty.
		bool IsEmpty() const noexcept { return (width == 0 && height == 0 && x == 0 && y == 0); }

		// Rectangle operations
		Vector2 Location() const noexcept;
		Vector2 Center() const noexcept;

		void Offset(float offsetX, float offsetY) noexcept
		{
			x += offsetX;
			y += offsetY;
		}

		void Inflate(float horizAmount, float vertAmount) noexcept
		{
			x -= horizAmount;
			y -= vertAmount;
			width += horizAmount;
			height += vertAmount;
		}

		bool Contains(float ix, float iy) const noexcept { return (x <= ix) && (ix < (x + width)) && (y <= iy) && (iy < (y + height)); }
		bool Contains(const Vector2& point) const noexcept { return (x <= point.x) && (point.x < (x + width)) && (y <= point.y) && (point.y < (y + height)); }
		bool Contains(const RectF& rhs) const noexcept { return (x <= rhs.x) && ((rhs.x + rhs.width) <= (x + width)) && (y <= rhs.y) && ((rhs.y + rhs.height) <= (y + height)); }

		bool Intersects(const RectF& r) const noexcept 
		{
			return (r.x < (x + width)) && (x < (r.x + r.width)) && (r.y < (y + height)) && (y < (r.y + r.height)); 
		}

		static RectF Intersect(const RectF& a, const RectF& b) noexcept;
		static RectF Union(const RectF& a, const RectF& b) noexcept;

		/// Return as string.
		String ToString() const;

        /// Return hash value of the rectangle.
        size_t ToHash() const;

		// Constants
		static const RectF Empty;
	};
}

namespace std
{
	template <>
	struct hash<Alimer::Rect> {
		size_t operator()(const Alimer::Rect& value) const noexcept {
            return value.ToHash();
		}
	};

	template <>
	struct hash<Alimer::RectF> {
		size_t operator()(const Alimer::RectF& value) const noexcept {
			return value.ToHash();
		}
	};
}
