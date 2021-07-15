// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "IO/FileStream.h"
#include "IO/FileSystem.h"
#include "Core/Log.h"
#include "Core/Assert.h"

#include <cstdio>
#ifdef _WIN32
#	include "PlatformInclude.h"
#endif

namespace Alimer
{
#ifndef _WIN32
    static const char* openModes[] =
    {
        "rb",
        "r+b",
        "wb",
        "w+b"
    };
#endif

    FileStream::FileStream(const std::string& fileName, FileMode mode)
    {
        Open(fileName, mode);
    }

    FileStream::~FileStream()
    {
        Close();
    }

    bool FileStream::Open(const std::string& fileName, FileMode fileMode)
    {
        Close();

        if (fileName.empty())
            return false;

#ifdef _WIN32
        auto wideFileName = ToUtf16(fileName);
        DWORD access = 0;
        DWORD creation = 0;
        switch (mode)
        {
        case FileMode::OpenRead:
            access = GENERIC_READ;
            creation = OPEN_EXISTING;
            break;
        case FileMode::Open:
            access = GENERIC_READ | GENERIC_WRITE;
            creation = OPEN_EXISTING;
            break;
        case FileMode::CreateWrite:
            access = GENERIC_WRITE;
            creation = CREATE_ALWAYS;
            break;
        case FileMode::Create:
            access = GENERIC_READ | GENERIC_WRITE;
            creation = CREATE_ALWAYS;
            break;
        }

        handle = CreateFileW(wideFileName.c_str(), access, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, creation, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (handle == INVALID_HANDLE_VALUE)
        {
            LOGE("Win32: Failed to access file: {}", fileName);
            return false;
        }
#else
        handle = fopen(Path::NativePath(fileName).c_str(), openModes[(uint32_t)fileMode]);
        if (!handle)
            return false;

#endif

        name = fileName;
        mode = fileMode;
        return true;
    }


    size_t FileStream::Read(void* buffer, size_t length)
    {
        ALIMER_VERIFY(CanRead());

#ifdef _WIN32
        static const DWORD read_step = 65536;

        size_t read = 0;

        while (read < length)
        {
            DWORD to_read = read_step;
            if (to_read > length - read)
                to_read = (DWORD)(length - read);

            DWORD moved = 0;
            if (ReadFile(handle, (uint8_t*)buffer + read, to_read, &moved, NULL))
                read += moved;

            if (moved < to_read)
                break;
        }

        return read;
#else
        return fread(buffer, numBytes, sizeof(char), length);
#endif
    }

    size_t FileStream::Seek(size_t position)
    {
        if (handle == nullptr)
            return 0;

#ifdef _WIN32
        LARGE_INTEGER move;
        move.QuadPart = position;

        LARGE_INTEGER result;
        if (SetFilePointerEx(handle, move, &result, FILE_BEGIN))
        {
            return result.QuadPart;
        }

        return 0;
#else
        fseek((FILE*)handle, (long)position, SEEK_SET);
        size_t position = ftell((FILE*)handle);
        return position;
#endif
    }

    size_t FileStream::Write(const void* buffer, size_t length)
    {
        if (!length)
            return 0;

        ALIMER_VERIFY(CanWrite());

#ifdef _WIN32
        static const DWORD write_step = 65536;

        size_t written = 0;

        while (written < length)
        {
            DWORD to_write = write_step;
            if (to_write > length - written)
                to_write = (DWORD)(length - written);

            DWORD moved = 0;
            if (WriteFile(handle, (const uint8_t*)buffer + written, to_write, &moved, NULL))
                written += moved;

            if (moved < to_write)
                break;
        }

        return written;
#else
        return fwrite(buffer, sizeof(char), length, (FILE*)handle);
#endif
    }

    uint64_t FileStream::Position() const
    {
#ifdef _WIN32
        LARGE_INTEGER move;
        LARGE_INTEGER result;

        move.QuadPart = 0;
        result.QuadPart = 0;

        SetFilePointerEx(handle, move, &result, FILE_CURRENT);
        return static_cast<uint64_t>(result.QuadPart);
#else
        size_t position = ftell((FILE*)handle);
        return position;
#endif
    }

    uint64_t FileStream::Length() const
    {
#ifdef _WIN32
        LARGE_INTEGER result;
        GetFileSizeEx(handle, &result);
        return static_cast<uint64_t>(result.QuadPart);
#else
        fseek((FILE*)handle, 0, SEEK_END);
        size_t length = ftell((FILE*)handle);
        fseek((FILE*)handle, 0, SEEK_SET);
        return length;
#endif
    }

    void FileStream::Close()
    {
#ifdef _WIN32
        if (handle != INVALID_HANDLE_VALUE)
        {
            CloseHandle(handle);
            handle = INVALID_HANDLE_VALUE;
        }
#else
        if (handle != nullptr)
        {
            fclose((FILE*)handle);
            handle = nullptr;
        }
#endif
    }

    void FileStream::Flush()
    {
#ifdef _WIN32
        if (handle != INVALID_HANDLE_VALUE)
        {
            FlushFileBuffers(handle);
        }
#else
        if (handle)
        {
            fflush((FILE*)handle);
        }
#endif
}

    bool FileStream::CanRead() const
    {
        return handle != nullptr && mode != FileMode::CreateWrite;
    }

    bool FileStream::CanWrite() const
    {
        return handle != nullptr && mode != FileMode::OpenRead;
    }

    bool FileStream::IsOpen() const
    {
        return handle != nullptr;
    }
}
