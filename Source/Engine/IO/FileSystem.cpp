// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "IO/FileStream.h"
#include "IO/FileSystem.h"
#include "Core/Log.h"
#include "Core/Assert.h"

#include <filesystem>
#include <cstdio>
#include <cstring>
#include <sys/stat.h>

#ifdef _WIN32
#	include "PlatformInclude.h"
#	include <sys/types.h>
#	include <sys/utime.h>
#else
#	include <dirent.h>
#	include <errno.h>
#	include <unistd.h>
#	include <utime.h>
#	include <sys/wait.h>
#	define MAX_PATH 256
#endif

#if defined(__APPLE__)
#	include <mach-o/dyld.h>
#endif

namespace Alimer
{
	FilePath Path::Normalize(const FilePath& path)
	{
		return ReplaceAll(path, "\\", "/");
	}

	std::string Path::NativePath(const FilePath& path)
	{
#ifdef _WIN32
		return ReplaceAll(path, "/", "\\");
#else
		return pathName;
#endif
	}

	std::wstring Path::WideNativePath(const FilePath& path)
	{
#ifdef _WIN32
		return ToUtf16(ReplaceAll(path, "/", "\\"));
#else
		return WString(pathName);
#endif
	}

	bool Path::IsAbsolute(const FilePath& path)
	{
		if (path.empty())
			return false;

		FilePath result = Path::Normalize(path);

		if (result[0] == '/')
			return true;

#ifdef _WIN32
		if (result.length() > 1 && IsAlpha(result[0]) && result[1] == ':')
			return true;
#endif

		return false;
	}

	FilePath Path::Join(const FilePath& path1, const FilePath& path2)
	{
		if (path1.empty())
			return path2;
		if (path2.empty())
			return path1;

		FilePath normalizedPath1 = Normalize(path1);
		return normalizedPath1.append("/").append(Normalize(path2));
	}

	bool Directory::SetCurrent(const FilePath& path)
	{
#ifdef _WIN32
		if (SetCurrentDirectoryW(Path::WideNativePath(path).c_str()) == FALSE)
			return false;
#else
		if (chdir(Path::NativePath(path).c_str()) != 0)
			return false;
#endif

		return true;
	}


    std::string Path::AddTrailingSlash(const std::string& pathName)
    {
        std::string ret = pathName;
        Trim(ret);

        ret = ReplaceAll(ret, "\\", "/");
        if (!ret.empty() && ret.back() != '/')
        {
            ret += '/';
        }

        return ret;
    }

    std::string RemoveTrailingSlash(const std::string& pathName)
    {
        std::string ret = pathName;
        Trim(ret);
        ret = ReplaceAll(ret, "\\", "/");
        if (!ret.empty() && ret.back() == '/')
        {
            ret.resize(ret.length() - 1);
        }

        return ret;
    }

	FilePath Directory::GetCurrent()
	{
#ifdef _WIN32
		wchar_t path[MAX_PATH];
		path[0] = 0;
		GetCurrentDirectoryW(MAX_PATH, path);
		return ToUtf8(path);
#else
		char path[MAX_PATH];
		path[0] = 0;
		getcwd(path, MAX_PATH);
		return FilePath(path);
#endif
	}

	bool Directory::Create(const FilePath& path)
	{
        try
        {
            if (std::filesystem::create_directory(path))
                return true;
        }
        catch (std::filesystem::filesystem_error& e)
        {
            LOGW("%s, %s", e.what(), path.c_str());
        }

        return false;
	}

	bool Directory::Exists(const FilePath& path)
	{
        return std::filesystem::exists(path);
	}

	bool File::Exists(const FilePath& path)
	{
		std::string fixedName = Path::NativePath(RemoveTrailingSlash(path));

#ifdef _WIN32
		DWORD attributes = GetFileAttributesW(ToUtf16(fixedName).c_str());
		if (attributes == INVALID_FILE_ATTRIBUTES || attributes & FILE_ATTRIBUTE_DIRECTORY)
			return false;
#else
		struct stat st;
		if (stat(fixedName.c_str(), &st) || st.st_mode & S_IFDIR)
			return false;
#endif

		return true;
	}

	bool File::Delete(const FilePath& path)
	{
#ifdef _WIN32
		return DeleteFileW(Path::WideNativePath(path).c_str()) != 0;
#else
		return remove(Path::NativePath(path).CString()) == 0;
#endif
	}

	std::string File::ReadAllText(const FilePath& path)
	{
		if (!File::Exists(path))
			return {};

		FileStream stream(path, FileMode::OpenRead);
		std::string result = stream.ReadString();
		return result;
	}

	Vector<uint8_t> File::ReadAllBytes(const FilePath& path)
	{
		if (!File::Exists(path))
			return {};

		FileStream stream(path, FileMode::OpenRead);
		return stream.ReadBytes();
	}

	bool CopyFile(const std::string& srcFileName, const std::string& destFileName)
	{
		FileStream srcFile(srcFileName, FileMode::OpenRead);
		if (!srcFile.IsOpen())
			return false;
		FileStream destFile(destFileName, FileMode::CreateWrite);
		if (!destFile.IsOpen())
			return false;

		/// \todo Should use a fixed-size buffer to allow copying very large files
		size_t fileSize = srcFile.Length();
		auto buffer = std::make_unique<uint8_t[]>(fileSize);

		size_t bytesRead = srcFile.Read(buffer.get(), fileSize);
		size_t bytesWritten = destFile.Write(buffer.get(), fileSize);
		return bytesRead == fileSize && bytesWritten == fileSize;
	}

	bool RenameFile(const std::string& srcFileName, const std::string& destFileName)
	{
#ifdef _WIN32
		return MoveFileW(Path::WideNativePath(srcFileName).c_str(), Path::WideNativePath(destFileName).c_str()) != 0;
#else
		return rename(Path::NativePath(srcFileName).c_str(), Path::NativePath(destFileName).c_str()) == 0;
#endif
	}

	uint32_t GetLastModifiedTime(const std::string& fileName)
	{
		if (fileName.empty())
			return 0;

#ifdef _WIN32
		struct _stat st;
		if (!_stat(fileName.c_str(), &st))
			return (uint32_t)st.st_mtime;
#else
		struct stat st;
		if (!stat(fileName.CString(), &st))
			return (uint32_t)st.st_mtime;
#endif

		return 0;
	}

	bool SetLastModifiedTime(const std::string& fileName, unsigned newTime)
	{
		if (fileName.empty())
			return false;

#ifdef WIN32
		struct _stat oldTime;
		struct _utimbuf newTimes;
		if (_stat(fileName.c_str(), &oldTime) != 0)
			return false;
		newTimes.actime = oldTime.st_atime;
		newTimes.modtime = newTime;
		return _utime(fileName.c_str(), &newTimes) == 0;
#else
		struct stat oldTime;
		struct utimbuf newTimes;
		if (stat(fileName.c_str(), &oldTime) != 0)
			return false;
		newTimes.actime = oldTime.st_atime;
		newTimes.modtime = newTime;
		return utime(fileName.c_str(), &newTimes) == 0;
#endif
	}

    static void ScanDirInternal(Vector<String>& result, String path, const String& startPath,
        const String& filter, ScanDirMode mode, bool recursive)
    {
        path = Path::AddTrailingSlash(path);
        String deltaPath;
        if (path.length() > startPath.length())
            deltaPath = path.substr(startPath.length());

        String filterExtension = filter.substr(filter.find('.'));
        if (filterExtension.find('*') != String::npos)
            filterExtension.clear();

#ifdef _WIN32
        WIN32_FIND_DATAW info;
        HANDLE handle = FindFirstFileW(ToUtf16(path + "*").c_str(), &info);
        if (handle != INVALID_HANDLE_VALUE)
        {
            do
            {
                String fileName = ToUtf8(info.cFileName);
                if (!fileName.empty())
                {
                    if (info.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN && !Any(mode, ScanDirMode::Hidden))
                        continue;
                    if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    {
                        if (Any(mode, ScanDirMode::Directories))
                            result.push_back(deltaPath + fileName);
                        if (recursive && fileName != "." && fileName != "..")
                            ScanDirInternal(result, path + fileName, startPath, filter, mode, recursive);
                    }
                    else if (Any(mode, ScanDirMode::Files))
                    {
                        if (filterExtension.empty() || EndsWith(fileName, filterExtension))
                        {
                            result.push_back(deltaPath + fileName);
                        }
                    }
                }
            } while (FindNextFileW(handle, &info));

            FindClose(handle);
        }
#else
        DIR* dir;
        struct dirent* de;
        struct stat st;
        dir = opendir(NativePath(path).CString());
        if (dir)
        {
            while ((de = readdir(dir)))
            {
                /// \todo Filename may be unnormalized Unicode on Mac OS X. Re-normalize as necessary
                String fileName(de->d_name);
                bool normalEntry = fileName != "." && fileName != "..";
                if (normalEntry && !any(mode & ScanDirMode::Hidden) && fileName.StartsWith("."))
                    continue;
                String pathAndName = path + fileName;
                if (!stat(pathAndName.CString(), &st))
                {
                    if (st.st_mode & S_IFDIR)
                    {
                        if (flags & SCAN_DIRS)
                            result.Push(deltaPath + fileName);
                        if (recursive && normalEntry)
                            ScanDirInternal(result, path + fileName, startPath, filter, flags, recursive);
                    }
                    else if (flags & SCAN_FILES)
                    {
                        if (filterExtension.IsEmpty() || fileName.EndsWith(filterExtension))
                            result.Push(deltaPath + fileName);
                    }
                }
            }
            closedir(dir);
        }
#endif
    }

    void ScanDir(std::vector<String>& result, const String& pathName, const String& filter, ScanDirMode mode, bool recursive)
    {
        String initialPath = Path::AddTrailingSlash(pathName);
        ScanDirInternal(result, initialPath, initialPath, filter, mode, recursive);
    }

    std::string GetExecutableDir()
    {
        std::string ret;

#if defined(_WIN32)
        wchar_t exeName[MAX_PATH];
        exeName[0] = 0;
        GetModuleFileNameW(0, exeName, MAX_PATH);
        ret = GetPath(ToUtf8(exeName));
#elif defined(__APPLE__)
        char exeName[MAX_PATH];
        memset(exeName, 0, MAX_PATH);
        unsigned size = MAX_PATH;
        _NSGetExecutablePath(exeName, &size);
        ret = GetPath(string(exeName));
#elif defined(__linux__)
        char exeName[MAX_PATH];
        memset(exeName, 0, MAX_PATH);
        pid_t pid = getpid();
        String link = "/proc/" + String(pid) + "/exe";
        readlink(link.CString(), exeName, MAX_PATH);
        ret = GetPath(string(exeName));
#endif

        // Sanitate /./ construct away
        ret = ReplaceAll(ret, "/./", "/");
        return ret;
    }

    void SplitPath(const std::string& fullPath, std::string& pathName, std::string& fileName, std::string& extension, bool lowercaseExtension)
    {
        String fullPathCopy = Path::Normalize(fullPath);

        String::size_type extPos = fullPathCopy.find_last_of('.');
        String::size_type pathPos = fullPathCopy.find_last_of('/');

        if (extPos != String::npos && (pathPos == String::npos || extPos > pathPos))
        {
            extension = fullPathCopy.substr(extPos);
            if (lowercaseExtension)
            {
                extension = ToLower(extension);
            }

            fullPathCopy = fullPathCopy.substr(0, extPos);
        }
        else
        {
            extension.clear();
        }

        pathPos = fullPathCopy.find_last_of('/');
        if (pathPos != String::npos)
        {
            fileName = fullPathCopy.substr(pathPos + 1);
            pathName = fullPathCopy.substr(0, pathPos + 1);
        }
        else
        {
            fileName = fullPathCopy;
            pathName.clear();
        }
    }

    String GetPath(const String& fullPath)
    {
        String path, file, extension;
        SplitPath(fullPath, path, file, extension);
        return path;
    }

    String GetFileName(const String& fullPath)
    {
        String path, file, extension;
        SplitPath(fullPath, path, file, extension);
        return file;
    }

    String GetExtension(const String& fullPath, bool lowercaseExtension)
    {
        String path, file, extension;
        SplitPath(fullPath, path, file, extension, lowercaseExtension);
        return extension;
    }

    String GetFileNameAndExtension(const String& fileName, bool lowercaseExtension)
    {
        String path, file, extension;
        SplitPath(fileName, path, file, extension, lowercaseExtension);
        return file + extension;
    }

    String ReplaceExtension(const String& fullPath, const String& newExtension)
    {
        String path, file, extension;
        SplitPath(fullPath, path, file, extension);
        return path + file + newExtension;
    }

    String ParentPath(const String& path)
    {
        size_t pos = RemoveTrailingSlash(path).find_last_of('/');
        if (pos != String::npos)
        {
            return path.substr(0, pos + 1);
        }

        return kEmptyString;
    }
}
