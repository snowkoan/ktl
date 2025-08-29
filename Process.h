#pragma once

#include "UnicodeString.h"
#include "DynamicImports.h"
#include "KernelHandle.h"

#define PROCESS_POOLTAG 'spTK'

#ifdef KTL_NAMESPACE
namespace ktl {
#endif

    class Process {
    public:
        Process(PEPROCESS process) 
        {
            // In theory, this could go away...
            _process = process;
        }

        ~Process()
        {
            if (_ProcessName)
            {
                ExFreePoolWithTag(_ProcessName, PROCESS_POOLTAG);
                _ProcessName = nullptr;
            }
        }

        NTSTATUS GetImageFileNameFullPath(_Out_ UnicodeString& imageName)
        {
            auto status = InitProcessName();
            if (NT_SUCCESS(status))
            {
                ASSERT(_ProcessName != nullptr);

                if (!imageName.Copy(_ProcessName))
                {
                    status = STATUS_INSUFFICIENT_RESOURCES;
                }
            }

            return status;
        }

        NTSTATUS GetImageFileNameOnly(UnicodeString& FileName)
        {
            auto status = InitProcessName();
            if (NT_SUCCESS(status))
            {
                ASSERT(_ProcessName != nullptr);

                // Find the last backslash
                
                UNICODE_STRING FinalComponent = {};
                if (NT_SUCCESS(status = FltParseFileName(_ProcessName, nullptr, nullptr, &FinalComponent)))
                {
                    if (!FileName.Copy(&FinalComponent))
                    {
                        status = STATUS_INSUFFICIENT_RESOURCES;
                    }
                }
            }

            return STATUS_SUCCESS;
        }

        static PCUNICODE_STRING GetUnknownProcessName()
        {
            static const UNICODE_STRING UnknownProcess = RTL_CONSTANT_STRING(L"Unknown");
            return &UnknownProcess;
        }     

    private:

        NTSTATUS InitProcessName()
        {
            if (_ProcessName != nullptr)
            {
                return STATUS_SUCCESS;
            }

            auto status = STATUS_UNSUCCESSFUL;

            do
            {
                if (!_hProcess.IsValid())
                {
                    // Get a handle to the current process
                    status = ObOpenObjectByPointer(
                        _process,
                        OBJ_KERNEL_HANDLE, // Or 0 for no special attributes
                        NULL,
                        0x400, // PROCESS_QUERY_INFORMATION
                        *PsProcessType,
                        KernelMode,
                        &_hProcess
                    );

                    if (!NT_SUCCESS(status)) {
                        break;
                    }
                }

                ULONG length = {};
                status = DynamicImports::Instance()->ZwQueryInformationProcess(
                    _hProcess,
                    ProcessImageFileName,
                    nullptr,
                    0,
                    &length);

                if (STATUS_INFO_LENGTH_MISMATCH != status) {
                    break;
                }

                _ProcessName = reinterpret_cast<PUNICODE_STRING>(ExAllocatePool2(POOL_FLAG_NON_PAGED, length, PROCESS_POOLTAG));

                if (nullptr == _ProcessName)
                {
                    status = STATUS_INSUFFICIENT_RESOURCES;
                    break;
                }

                status = DynamicImports::Instance()->ZwQueryInformationProcess(
                    _hProcess,
                    ProcessImageFileName,
                    _ProcessName,
                    length,
                    &length);

                if (!NT_SUCCESS(status))
                {
                    ExFreePoolWithTag(_ProcessName, PROCESS_POOLTAG);
                    break;
                }

                status = STATUS_SUCCESS;

            } while (false);

            return status;
        }

        PEPROCESS _process = {};
        PUNICODE_STRING _ProcessName = {};
        KernelHandle _hProcess;
    };

#ifdef KTL_NAMESPACE
}
#endif

