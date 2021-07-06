// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "AlimerConfig.h"
#include <string.h>
#include <unordered_map>

#include <cstdarg>
#include <cstring>
#include <cctype>

namespace Alimer
{
    static constexpr uint32_t kConversionBufferLength = 128;
    static constexpr uint32_t kMatrixConversionBufferLength = 256;

    using String = std::string;
    using StringView = std::string_view;

    ALIMER_API extern const String kEmptyString;

#if _WIN32
    using WideChar = wchar_t;
    using WString = std::wstring;
#else
    using WideChar = char16_t;
    using WString = std::string16;
#endif

    /// Return length of a C string.
    ALIMER_API size_t CStringLength(const char* str);

    // Convert to lower-case.
    ALIMER_API String ToLower(const String& str);

    // Convert to upper-case.
    ALIMER_API String ToUpper(const String& str);

    ALIMER_API String& LTrim(String& str, const String& chars = "\t\n\v\f\r ");

    ALIMER_API String& RTrim(String& str, const String& chars = "\t\n\v\f\r ");

    ALIMER_API String& Trim(String& str, const String& chars = "\t\n\v\f\r ");

    /// Replace all instances of a sub-string with a another sub-string.
    ALIMER_API String ReplaceAll(const String& source, const String& replaceWhat, const String& replaceWithWhat);

#ifdef _WIN32
    ALIMER_API String ToUtf8(const wchar_t* wstr, size_t len);
    ALIMER_API String ToUtf8(const WString& str);

    ALIMER_API WString ToUtf16(const char* str, size_t len);
    ALIMER_API WString ToUtf16(const String& str);
    ALIMER_API WString ToUtf16(const StringView& str);
#endif
}
