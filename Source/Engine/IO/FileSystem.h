// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/String.h"
#include <vector>

namespace Alimer
{
	using FilePath = std::string;

	enum class ScanDirMode : uint32_t
	{
		None = 0,
		/// Return files.
		Files = 1 << 0,
		/// Return directories.
		Directories = 1 << 1,
		/// Return also hidden files.
		Hidden = 1 << 2,
	};

	namespace Path
	{
		/// Convert a path to normalized (internal) format which uses slashes.
		ALIMER_API FilePath Normalize(const FilePath& path);
		/// Convert a path to the format required by the operating system.
		ALIMER_API FilePath NativePath(const FilePath& path);
		/// Convert a path to the format required by the operating system in wide characters.
		ALIMER_API std::wstring WideNativePath(const FilePath& path);
		/// Return whether a path is absolute.
		ALIMER_API bool IsAbsolute(const FilePath& path);

        /// Add a slash at the end of the path if missing and convert to normalized format (use slashes.)
        ALIMER_API String AddTrailingSlash(const String& pathName);
        /// Remove the slash from the end of a path if exists and convert to normalized format (use slashes.)
        ALIMER_API String RemoveTrailingSlash(const String& pathName);

		ALIMER_API FilePath Join(const FilePath& path1, const FilePath& path2);

		template<typename ... Args>
		FilePath Join(const FilePath& path1, const FilePath& path2, const Args&... args)
		{
			return Join(path1, Join(path2, std::forward<Args>(args)...));
		}
	}

	namespace Directory
	{
		/// Set the current working directory.
		ALIMER_API bool SetCurrent(const FilePath& path);
		/// Return the absolute current working directory.
		ALIMER_API FilePath GetCurrent();
		/// Create a directory.
		ALIMER_API bool Create(const FilePath& path);
		/// Check if a directory exists.
		ALIMER_API bool Exists(const FilePath& path);
	}
	
	namespace File
	{
		/// Check if a file exists.
		ALIMER_API bool Exists(const FilePath& path);

		/// Delete a file. Return true on success.
		ALIMER_API bool Delete(const FilePath& path);

		ALIMER_API std::string ReadAllText(const FilePath& path);
		ALIMER_API Vector<uint8_t> ReadAllBytes(const FilePath& path);
	}

	/// Copy a file. Return true on success.
	ALIMER_API bool CopyFile(const std::string& srcFileName, const std::string& destFileName);
	/// Rename a file. Return true on success.
	ALIMER_API bool RenameFile(const std::string& srcFileName, const std::string& destFileName);
	
	
	/// Return the file's last modified time as seconds since epoch, or 0 if can not be accessed.
	ALIMER_API uint32_t GetLastModifiedTime(const std::string& fileName);
	/// Set the file's last modified time as seconds since epoch. Return true on success.
	ALIMER_API bool SetLastModifiedTime(const std::string& fileName, unsigned newTime);
	
    /// Scan a directory for specified files.
    ALIMER_API void ScanDir(std::vector<std::string>& result, const std::string& pathName, const std::string& filter, ScanDirMode mode = ScanDirMode::Files, bool recursive = false);
    /// Return the executable's directory.
    ALIMER_API std::string GetExecutableDir();
    /// Split a full path to path, filename and extension. The extension will be converted to lowercase by default.
    ALIMER_API void SplitPath(const std::string& fullPath, std::string& pathName, std::string& fileName, std::string& extension, bool lowercaseExtension = true);
    /// Return the path from a full path.
    ALIMER_API String GetPath(const String& fullPath);
    /// Return the filename from a full path.
    ALIMER_API std::string GetFileName(const std::string& fullPath);
    /// Return the extension from a full path, converted to lowercase by default.
    ALIMER_API std::string GetExtension(const std::string& fullPath, bool lowercaseExtension = true);
    /// Return the filename and extension from a full path. The case of the extension is preserved by default, so that the file can be opened in case-sensitive operating systems.
    ALIMER_API std::string GetFileNameAndExtension(const std::string& fullPath, bool lowercaseExtension = false);
    /// Replace the extension of a file name with another.
    ALIMER_API std::string ReplaceExtension(const std::string& fullPath, const std::string& newExtension);
    
    /// Return the parent path, or the path itself if not available.
    ALIMER_API std::string ParentPath(const std::string& pathName);
}

ALIMER_DEFINE_ENUM_BITWISE_OPERATORS(Alimer::ScanDirMode);
