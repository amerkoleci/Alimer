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

    /// Return whether a char is an alphabet letter.
    inline bool IsAlpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
    /// Return whether a char is a digit.
    inline bool IsDigit(char c) { return c >= '0' && c <= '9'; }

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

    // Check if the string str ends with the given suffix.
    // The comparison is case sensitive.
    ALIMER_API bool EndsWith(const String& str, const String& suffix);

    // Check if the string str ends with the given suffix.
    // Suffix may not be NULL and needs to be NULL terminated.
    // The comparison is case sensitive.
    ALIMER_API bool EndsWith(const String& str, const char* suffix);

    // Check if the string str ends with the given suffix.
    // str and suffix may not be NULL and need to be NULL terminated.
    // The comparison is case sensitive.
    ALIMER_API bool EndsWith(const char* str, const char* suffix);

    /// Return substrings split by a separator char.
    ALIMER_API std::vector<String> Split(const char* str, char separator);

#ifdef _WIN32
    ALIMER_API String ToUtf8(const wchar_t* wstr, size_t len);
    ALIMER_API String ToUtf8(const WString& str);

    ALIMER_API WString ToUtf16(const char* str, size_t len);
    ALIMER_API WString ToUtf16(const String& str);
    ALIMER_API WString ToUtf16(const StringView& str);
#endif
}
