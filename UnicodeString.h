#pragma once

#ifdef KTL_NAMESPACE
namespace ktl {
#endif

#define UNICODE_STRING_POOLTAG 'suTK'

//
// A simple UNICODE_STRING wrapper.
// 
// Not thread-safe, though this could be added later.
//
class UnicodeString
{
public:

    // Default constructor only. Use Copy to initialize -- it has a return code.
    UnicodeString(): m_AllocatedBufferSizeBytes(0)
    {
        RtlInitEmptyUnicodeString(&m_str, nullptr, 0);
    }

    ~UnicodeString()
    {
        Reset();
    }

    void Reset()
    {
        if (m_str.Buffer)
        {
            ExFreePool(m_str.Buffer);
        }
        RtlInitEmptyUnicodeString(&m_str, nullptr, 0);
        m_AllocatedBufferSizeBytes = 0;
    }

    // Explicit copy so that we can get a return code
    bool Copy(PCUNICODE_STRING pStr);
    NTSTATUS Append(PCUNICODE_STRING pStr);

    __inline PCUNICODE_STRING Get() const 
    {
        return &m_str;
    }
    __inline operator PCUNICODE_STRING() const
    {
        return Get();
    }

    __inline USHORT GetCharCount() const
    {
        return (m_str.Length / 2);
    }

    // Caller is responsible for passing a valid index
    __inline wchar_t GetAt(const USHORT index) const
    {
        ASSERT(index < m_AllocatedBufferSizeBytes /sizeof(wchar_t));
        return m_str.Buffer[index];
    }
    __inline wchar_t operator[](const USHORT index) const
    {
        return GetAt(index);
    }

    __inline bool ConvertDOSPathToNtPath()
    {
        // convert \\?\ to \??\ 
        if (GetCharCount() >= 4 &&
            GetAt(0) == L'\\' &&
            GetAt(1) == L'\\' &&
            GetAt(2) == L'?' &&
            GetAt(3) == L'\\')
        {
            m_str.Buffer[1] = L'?';
            return true;
        }

        return false;
    }

    __inline bool EndsWith(const wchar_t c) const
    {
        if (m_str.Length)
        {
            return c == m_str.Buffer[(m_str.Length / sizeof(wchar_t)) - 1];
        }
        return false;
    }

    __inline void DecrementLength()
    {
        if (m_str.Length >= sizeof(wchar_t))
        {
            m_str.Length -= sizeof(wchar_t);
        }
    }

    void* operator new(size_t s)
    {
        return ExAllocatePoolZero(PagedPool, s, UNICODE_STRING_POOLTAG);
    }
    void operator delete(void* p)
    {
        if (p)
        {
            ExFreePoolWithTag(p, UNICODE_STRING_POOLTAG);
        }
    }
    LPCWSTR c_str()
    {
        if (m_str.MaximumLength == 0)
        {
            return nullptr;
        }
        else
        {
            // There should always be some SLACK.
            ASSERT(m_str.Length < m_AllocatedBufferSizeBytes);
            if (m_str.Length < m_AllocatedBufferSizeBytes)
            {
                m_str.Buffer[m_str.Length / sizeof(wchar_t)] = L'\0';
            }
            else
            {
                return L"";
            }
            return m_str.Buffer;
        }
    }

    // Not needed at present
    UnicodeString(UnicodeString const&) = delete;
    UnicodeString& operator=(UnicodeString const&) = delete;

    bool Reserve(const USHORT charCount);

    void Move( UnicodeString& src );

private:

    //
    // Allocates enough space for charCount characters, plus room for
    // trailing NULL, in case it is required (even if charCount == MAXUSHORT)
    // 
    // All memory is zeroed out.
    //
    bool AllocateChars(const USHORT charCount, const POOL_TYPE pool = PagedPool);

    UNICODE_STRING m_str;
    ULONG          m_AllocatedBufferSizeBytes;

    // Leave some slack, for null terminator and backslash
    static constexpr ULONG SLACK = 2;
};

#ifdef KTL_NAMESPACE
}
#endif

