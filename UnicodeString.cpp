#include "pch.h"

#include "UnicodeString.h"

void UnicodeString::Move(UnicodeString& src)
{
    Reset();

    m_str = src.m_str;
    m_AllocatedBufferSizeBytes = src.m_AllocatedBufferSizeBytes;

    RtlInitEmptyUnicodeString(&src.m_str, nullptr, 0);
    src.m_AllocatedBufferSizeBytes = 0;
}

bool UnicodeString::Reserve(const USHORT charCount)
{
    // We need to enforce SLACK here, in case this is later NULL terminated.
    // If someone has requested MAXUSHORT chars, this means we'll end up re-allocating.
    if (m_str.MaximumLength < ((SLACK + charCount) * sizeof(wchar_t)))
    {
        UnicodeString newStr;
        if (!newStr.AllocateChars(charCount))
        {
            return false;
        }

        newStr.Copy( Get() );
        Move(newStr);
    }

    return true;
}

bool UnicodeString::AllocateChars(const USHORT charCount, const POOL_TYPE pool)
{
    Reset();

    const ULONG byteCount = (SLACK + charCount) * sizeof(wchar_t);
    PVOID p = ExAllocatePoolZero(pool,
        byteCount,
        UNICODE_STRING_POOLTAG);

    if (nullptr == p)
    {
        return false;
    }

    // Make sure MaximumLength is <= MAXUSHORT (since we added some slack)
    RtlInitEmptyUnicodeString(&m_str,
        reinterpret_cast<PWCHAR>(p),
        static_cast<USHORT>(min(byteCount, MAXUSHORT)));

    m_AllocatedBufferSizeBytes = byteCount;
    return true;
}

bool UnicodeString::Copy(PCUNICODE_STRING pStr)
{
    if (!pStr)
    {
        return false;
    }

    // Do we have enough space to copy the string?
    if (pStr->Length > m_str.MaximumLength)
    {
        // No, re-allocate.
        if (!AllocateChars(pStr->Length / sizeof(wchar_t)))
        {
            return false;
        }
    }

    RtlCopyUnicodeString(&m_str, pStr);
    return true;
}

NTSTATUS UnicodeString::Append(PCUNICODE_STRING pStr)
{
    if (!pStr)
    {
        return STATUS_INVALID_PARAMETER;
    }

    ULONG totalLen = m_str.Length + pStr->Length;
    if (totalLen > MAXUSHORT)
    {
        // Too big for UNICODE_STRING
        return STATUS_BUFFER_TOO_SMALL;
    }

    // Do we have enough space to append the string?
    if (totalLen > m_str.MaximumLength)
    {
    	// No, re-allocate, copy, then append.
        UnicodeString tmp;
        if (!tmp.Copy(this->Get()))
        {
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        if (!AllocateChars(static_cast<USHORT>(totalLen) / sizeof(wchar_t)))
        {
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        // Copy our original string back in
        RtlCopyUnicodeString(&m_str, tmp);
    }

    return RtlAppendUnicodeStringToString(&m_str, pStr);
}