// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "IO/Stream.h"

namespace Alimer
{
	/// File open mode.
	enum class FileMode
	{
        // Opens an existing file for reading.
        OpenRead,
        // Opens an existing file for reading and writing.
        Open,
        // Creates a new file or overwrites an existing file for writing.
        CreateWrite,
        // Creates a new file or overwrites an existing file for reading and writing.
        Create,
	};

	/// File stream class
	class ALIMER_API FileStream : public Stream
	{
	public:
		/// Constructor.
		FileStream() = default;
		/// Constructor and open a file.
		FileStream(const std::string& fileName, FileMode fileMode = FileMode::OpenRead);
		/// Destructor. Close the file if open.
		~FileStream() override;

		/// Read bytes from the file. Return number of bytes actually read.
		size_t Read(void* buffer, size_t length) override;
		/// Set position in bytes from the beginning of the file.
        size_t Seek(size_t position) override;
		/// Write bytes to the file. Return number of bytes actually written.
		size_t Write(const void* buffer, size_t length) override;
		/// Return whether read operations are allowed.
		bool CanRead() const override;
		/// Return whether write operations are allowed.
		bool CanWrite() const override;
        uint64_t Position() const override;
        uint64_t Length() const override;

		/// Open a file. Return true on success.
		bool Open(const std::string& fileName, FileMode fileMode = FileMode::OpenRead);
		/// Close the file.
		void Close();
		/// Flush any buffered output to the file.
		void Flush();

		/// Return the open mode.
		FileMode GetMode() const { return mode; }
		/// Return whether is open.
		bool IsOpen() const;
		/// Return the file handle.
		void* GetHandle() const { return handle; }

		using Stream::Read;
		using Stream::Write;

	private:
		/// Open mode.
		FileMode mode = FileMode::OpenRead;
		/// File handle.
		void* handle = nullptr;
	};

}
