// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#pragma once

#include "Core/Types.h"

namespace Alimer
{
	class StringId32;
	struct ObjectRef;

	/// Abstract stream for reading and writing.
	class ALIMER_API Stream
	{
	public:
		/// Default-construct with zero size.
		Stream() = default;
		/// Destructor.
		virtual ~Stream() = default;

		/// Read bytes from the stream. Return number of bytes actually read.
		virtual size_t Read(void* buffer, size_t length) = 0;
		/// Seeks the position of the stream
		virtual size_t Seek(size_t position) = 0;
		/// Write bytes to the stream. Return number of bytes actually written.
		virtual size_t Write(const void* data, size_t size) = 0;
		/// Return whether read operations are allowed.
		virtual bool CanRead() const = 0;
		/// Return whether write operations are allowed.
		virtual bool CanWrite() const = 0;

		/// Change the stream name.
		void SetName(const String& newName);
		/// Read a boolean.
		bool ReadBoolean();
        /// Read an 8-bit integer.
        int8_t ReadSByte();
        /// Read a 16-bit integer.
        int16_t ReadInt16();
        /// Read a 32-bit integer.
        int32_t ReadInt32();
        /// Read a 32-bit integer.
        int64_t ReadInt64();
        /// Read an 8-bit unsigned integer.
        uint8_t ReadByte();
        /// Read a 16-bit unsigned integer.
        uint16_t ReadUInt16();
        /// Read a 32-bit unsigned integer.
        uint32_t ReadUInt32();
        /// Read a 64-bit unsigned integer.
        uint64_t ReadUInt64();
        /// Read a float.
        float ReadSingle();
        /// Read a double.
        double ReadDouble();
        ///  Reads in a 32-bit integer in compressed format.
        uint32_t Read7BitEncodedInt();
        ///  Reads a number 7 bits at a time.
        uint64_t Read7BitEncodedInt64();
       
		// reads a string of a given length, or until a null terminator if -1
		String ReadString(int length = -1);
		/// Read a variable-length encoded unsigned integer, which can use 29 bits maximum.
		uint32_t ReadVLE();
		/// Read a text line.
        String ReadLine();
		/// Read a 4-character file ID.
        String ReadFileID();
		/// Read a vector bytes
		Vector<uint8_t> ReadBytes(size_t count = 0);
		/// Read a byte buffer, with size prepended as a VLE value.
        Vector<uint8_t> ReadBuffer();
		/// Write a four-letter file ID. If the string is not long enough, spaces will be appended.
		void WriteFileID(const String& value);
		/// Write a byte buffer, with size encoded as VLE.
		void WriteBuffer(const Vector<uint8_t>& buffer);
		/// Write a variable-length encoded unsigned integer, which can use 29 bits maximum.
		void WriteVLE(size_t value);
		/// Write a text line. Char codes 13 & 10 will be automatically appended.
		void WriteLine(const String& value);

		/// Write a value, template version.
		template <class T> void Write(const T& value) { Write(&value, sizeof value); }

		/// Read a value, template version.
		template <class T> T Read()
		{
			T ret;
			Read(&ret, sizeof ret);
			return ret;
		}

		/// Return the stream name.
		const String& GetName() const { return name; }
		/// Returns the position of the stream
        virtual uint64_t Position() const = 0;
		/// Return the length of the stream.
        virtual uint64_t Length() const = 0;

	protected:
		/// Stream name.
        String name;
	};

	template<> ALIMER_API bool Stream::Read();
	template<> ALIMER_API String Stream::Read();
	template<> ALIMER_API StringId32 Stream::Read();
	//template<> ALIMER_API ResourceRef Stream::Read();
	//template<> ALIMER_API ResourceRefList Stream::Read();
	//template<> ALIMER_API ObjectRef Stream::Read();
	//template<> ALIMER_API JSONValue Stream::Read();
	template<> ALIMER_API void Stream::Write(const bool& value);
	template<> ALIMER_API void Stream::Write(const String& value);
	template<> ALIMER_API void Stream::Write(const StringId32& value);
	//template<> ALIMER_API void Stream::Write(const ResourceRef& value);
	//template<> ALIMER_API void Stream::Write(const ResourceRefList& value);
	//template<> ALIMER_API void Stream::Write(const ObjectRef& value);
	//template<> ALIMER_API void Stream::Write(const JSONValue& value);
}
