// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "Core/String.h"
#include "Core/Types.h"
#include <sstream>
#include <cstdio>
#include "PlatformInclude.h"

namespace Alimer
{
    const String kEmptyString{};

    size_t CStringLength(const char* str)
    {
        return str ? strlen(str) : 0;
    }

    String ToLower(const String& str)
    {
        String result;
        for (const char& ch : str)
        {
            result += static_cast<char>(::tolower(ch));
        }

        return result;
    }

    String ToUpper(const String& str)
    {
        String result;
        for (const char& ch : str)
        {
            result += static_cast<char>(::toupper(ch));
        }

        return result;
    }

    String& LTrim(String& str, const String& chars)
    {
        str.erase(0, str.find_first_not_of(chars));
        return str;
    }

    String& RTrim(String& str, const String& chars)
    {
        str.erase(str.find_last_not_of(chars) + 1);
        return str;
    }

    String& Trim(String& str, const String& chars)
    {
        return LTrim(RTrim(str, chars), chars);
    }

    String ReplaceAll(const String& source, const String& replaceWhat, const String& replaceWithWhat)
    {
        String result = source;
        String::size_type pos = 0;
        while (1)
        {
            pos = result.find(replaceWhat, pos);
            if (pos == String::npos)
                break;
            result.replace(pos, replaceWhat.size(), replaceWithWhat);
            pos += replaceWithWhat.size();
        }
        return result;
    }

#ifdef _WIN32
    String ToUtf8(const wchar_t* wstr, size_t len)
    {
        Vector<char> char_buffer;
        auto ret = WideCharToMultiByte(CP_UTF8, 0, wstr, static_cast<int>(len), nullptr, 0, nullptr, nullptr);
        if (ret < 0)
            return "";

        char_buffer.resize(ret);
        WideCharToMultiByte(CP_UTF8, 0, wstr, static_cast<int>(len), char_buffer.data(), static_cast<int>(char_buffer.size()), nullptr, nullptr);
        return std::string(char_buffer.data(), static_cast<uint32_t>(char_buffer.size()));
    }

    String ToUtf8(const std::wstring& str)
    {
        return ToUtf8(str.data(), str.length());
    }

    WString ToUtf16(const char* str, size_t len)
    {
        Vector<wchar_t> wchar_buffer;
        auto ret = MultiByteToWideChar(CP_UTF8, 0, str, static_cast<int>(len), nullptr, 0);
        if (ret < 0)
            return L"";
        wchar_buffer.resize(ret);
        MultiByteToWideChar(CP_UTF8, 0, str, static_cast<int>(len), wchar_buffer.data(), static_cast<int>(wchar_buffer.size()));
        return std::wstring(wchar_buffer.data(), wchar_buffer.size());
    }

    WString ToUtf16(const String& str)
    {
        return ToUtf16(str.data(), str.length());
    }

    WString ToUtf16(const StringView& str)
    {
        return ToUtf16(str.data(), str.length());
    }
#endif
}
