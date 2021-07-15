// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "IO/Stream.h"
#include "Math/MathHelper.h"
#include "Core/StringId.h"

namespace Alimer
{
	void Stream::SetName(const String& newName)
	{
		name = newName;
	}

	bool Stream::ReadBoolean()
	{
		return ReadByte() != 0;
	}

    int8_t Stream::ReadSByte()
    {
        int8_t ret;
        Read(&ret, sizeof ret);
        return ret;
    }

    int16_t Stream::ReadInt16()
    {
        int16_t ret;
        Read(&ret, sizeof ret);
        return ret;
    }

    int32_t Stream::ReadInt32()
    {
        int32_t ret;
        Read(&ret, sizeof ret);
        return ret;
    }

    int64_t Stream::ReadInt64()
    {
        int64_t ret;
        Read(&ret, sizeof ret);
        return ret;
    }

    uint8_t Stream::ReadByte()
    {
        uint8_t ret;
        Read(&ret, sizeof(uint8_t));
        return ret;
    }

    uint16_t Stream::ReadUInt16()
    {
        uint16_t ret;
        Read(&ret, sizeof(uint16_t));
        return ret;
    }

    uint32_t Stream::ReadUInt32()
    {
        uint32_t ret;
        Read(&ret, sizeof(uint32_t));
        return ret;
    }

    uint64_t Stream::ReadUInt64()
    {
        uint64_t ret;
        Read(&ret, sizeof(uint64_t));
        return ret;
    }

    float Stream::ReadSingle()
    {
        float ret;
        Read(&ret, sizeof(float));
        return ret;
    }

    double Stream::ReadDouble()
    {
        double ret;
        Read(&ret, sizeof ret);
        return ret;
    }

    uint32_t Stream::Read7BitEncodedInt()
    {
        uint32_t result = 0;
        uint8_t byteReadJustNow;

        // Read the integer 7 bits at a time. The high bit
        // of the byte when on means to continue reading more bytes.
        //
        // There are two failure cases: we've read more than 5 bytes,
        // or the fifth byte is about to cause integer overflow.
        // This means that we can read the first 4 bytes without
        // worrying about integer overflow.

        const uint32_t MaxBytesWithoutOverflow = 4;
        for (uint32_t shift = 0; shift < MaxBytesWithoutOverflow * 7; shift += 7)
        {
            // ReadByte handles end of stream cases for us.
            byteReadJustNow = ReadByte();
            result |= (byteReadJustNow & 0x7Fu) << shift;

            if (byteReadJustNow <= 0x7Fu)
            {
                return result; // early exit
            }
        }

        // Read the 5th byte. Since we already read 28 bits,
        // the value of this byte must fit within 4 bits (32 - 28),
        // and it must not have the high bit set.

        byteReadJustNow = ReadByte();
        if (byteReadJustNow > 15u)
        {
            //ALIMER_LOGE("Bad 7Bit integer");
        }

        result |= (uint32_t)byteReadJustNow << (MaxBytesWithoutOverflow * 7);
        return result;
    }

    uint64_t Stream::Read7BitEncodedInt64()
    {
        uint64_t result = 0;
        uint8_t byteReadJustNow;

        // Read the integer 7 bits at a time. The high bit
        // of the byte when on means to continue reading more bytes.
        //
        // There are two failure cases: we've read more than 10 bytes,
        // or the tenth byte is about to cause integer overflow.
        // This means that we can read the first 9 bytes without
        // worrying about integer overflow.

        const uint32_t MaxBytesWithoutOverflow = 9;
        for (uint32_t shift = 0; shift < MaxBytesWithoutOverflow * 7; shift += 7)
        {
            // ReadByte handles end of stream cases for us.
            byteReadJustNow = ReadByte();
            result |= (byteReadJustNow & 0x7Ful) << shift;

            if (byteReadJustNow <= 0x7Fu)
            {
                return result; // early exit
            }
        }

        // Read the 10th byte. Since we already read 63 bits,
        // the value of this byte must fit within 1 bit (64 - 63),
        // and it must not have the high bit set.

        byteReadJustNow = ReadByte();
        if (byteReadJustNow > 1u)
        {
            //ALIMER_LOGE("Bad 7Bit integer");
        }

        result |= (uint64_t)byteReadJustNow << (MaxBytesWithoutOverflow * 7);
        return result;
    }

	String Stream::ReadString(int length)
	{
		if (length >= 0)
		{
            String str;
			str.resize(length);
			Read(str.data(), length);
			str[length] = '\0';
			return str;
		}
		else
		{
            String str;
			char next;
			while (Read(&next, 1) && next != '\0')
			{
				str += next;
			}

			return str;
		}
	}

	uint32_t Stream::ReadVLE()
	{
		uint32_t ret;
		uint8_t byte;

		byte = ReadByte();
		ret = byte & 0x7f;
		if (byte < 0x80)
			return ret;

		byte = ReadByte();
		ret |= ((uint32_t)(byte & 0x7f)) << 7;
		if (byte < 0x80)
			return ret;

		byte = ReadByte();
		ret |= ((uint32_t)(byte & 0x7f)) << 14;
		if (byte < 0x80)
			return ret;

		byte = ReadByte();
		ret |= ((uint32_t)byte) << 21;
		return ret;
	}

    String Stream::ReadLine()
	{
        String result;

        char next;
        while (Read(&next, 1) && next != '\n' && next != '\0')
        {
            result += next;
        }

        return result;
	}

    String Stream::ReadFileID()
	{
        String result;
		result.resize(4);
		Read(result.data(), 4);
		return result;
	}

	Vector<uint8_t> Stream::ReadBytes(size_t count)
	{
		if (!count)
			count = Length();

        Vector<uint8_t> result(count);
		Read(result.data(), result.size());
		return result;
	}

    Vector<uint8_t> Stream::ReadBuffer()
	{
        Vector<uint8_t> result(ReadVLE());
		if (result.size())
		{
			Read(result.data(), result.size());
		}

		return result;
	}

	template<> bool Stream::Read<bool>()
	{
		return ReadBoolean();
	}

	template<> String Stream::Read<String>()
	{
		return ReadString();
	}

	template<> StringId32 Stream::Read<StringId32>()
	{
		return StringId32(ReadUInt32());
	}

	void Stream::WriteFileID(const String& value)
	{
		Write(value.c_str(), Min(value.length(), (size_t)4));
		for (size_t i = value.length(); i < 4; ++i)
		{
			Write(' ');
		}
	}

	void Stream::WriteBuffer(const Vector<uint8_t>& value)
	{
		size_t numBytes = value.size();

		WriteVLE(numBytes);
		if (numBytes)
		{
			Write(&value[0], numBytes);
		}
	}

	void Stream::WriteVLE(size_t value)
	{
		uint8_t data[4];

		if (value < 0x80)
			Write((uint8_t)value);
		else if (value < 0x4000)
		{
			data[0] = (uint8_t)value | 0x80;
			data[1] = (uint8_t)(value >> 7);
			Write(data, 2);
		}
		else if (value < 0x200000)
		{
			data[0] = (uint8_t)value | 0x80;
			data[1] = (uint8_t)((value >> 7) | 0x80);
			data[2] = (uint8_t)(value >> 14);
			Write(data, 3);
		}
		else
		{
			data[0] = (uint8_t)value | 0x80;
			data[1] = (uint8_t)((value >> 7) | 0x80);
			data[2] = (uint8_t)((value >> 14) | 0x80);
			data[3] = (uint8_t)(value >> 21);
			Write(data, 4);
		}
	}

	void Stream::WriteLine(const String& value)
	{
		Write(value.c_str(), value.length());
		Write('\r');
		Write('\n');
	}

	template<> void Stream::Write<bool>(const bool& value)
	{
		Write<unsigned char>(value ? 1 : 0);
	}

	template<> void Stream::Write<std::string>(const std::string& value)
	{
		// Write content and null terminator
		Write(value.c_str(), value.length() + 1);
	}

	template<> void Stream::Write<StringId32>(const StringId32& value)
	{
		Write(value.Value());
	}
}
