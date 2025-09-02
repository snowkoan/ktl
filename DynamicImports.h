#pragma once

class DynamicImports
{
public:

	// Call this in DriverEntry to initialize. We are guaranteed to be at PASSIVE_LEVEL.
	static DynamicImports* Instance()
	{
		static DynamicImports singleton;
		return &singleton;
	}

	// Win11 22H2
	_IRQL_requires_max_(DISPATCH_LEVEL)
		NTSTATUS
		FLTAPI
		FltGetCopyInformationFromCallbackData(
			_In_ PFLT_CALLBACK_DATA Data,
			_Out_ PCOPY_INFORMATION CopyInformation
		)
	{
		if (_FltGetCopyInformationFromCallbackData == nullptr)
		{
			return STATUS_NOT_SUPPORTED;
		}
		else
		{
			return _FltGetCopyInformationFromCallbackData(Data, CopyInformation);
		}
	}

	// Win11 22H2
	bool IoCheckFileObjectOpenedAsCopySource(
		_In_ PFILE_OBJECT FileObject
	)
	{
		return (nullptr == _IoCheckFileObjectOpenedAsCopySource) ? false : _IoCheckFileObjectOpenedAsCopySource(FileObject);
	}

	// Win11 22H2
	bool IoCheckFileObjectOpenedAsCopyDestination(
		_In_ PFILE_OBJECT FileObject
	)
	{
		return (nullptr == _IoCheckFileObjectOpenedAsCopyDestination) ? false : _IoCheckFileObjectOpenedAsCopyDestination(FileObject);
	}

	// This never got into the headers and the docs say that it may disappear one day
	NTSTATUS
		NTAPI ZwQueryInformationProcess(
			_In_      HANDLE           ProcessHandle,
			_In_      PROCESSINFOCLASS ProcessInformationClass,
			_Out_     PVOID            ProcessInformation,
			_In_      ULONG            ProcessInformationLength,
			_Out_opt_ PULONG           ReturnLength
		)
	{
		if (nullptr ==_ZwQueryInformationProcess)
		{
			return STATUS_NOT_SUPPORTED;
		}
		else
		{
			return _ZwQueryInformationProcess(ProcessHandle, 
				ProcessInformationClass, 
				ProcessInformation, 
				ProcessInformationLength, 
				ReturnLength);
		}
	}

private:

	typedef NTSTATUS(FLTAPI* PFN_FltGetCopyInformationFromCallbackData)(
		_In_ PFLT_CALLBACK_DATA Data,
		_Out_ PCOPY_INFORMATION CopyInformation
		);
	PFN_FltGetCopyInformationFromCallbackData _FltGetCopyInformationFromCallbackData = {};

	typedef NTSTATUS(NTAPI* PFN_ZwQueryInformationProcess) (
		__in HANDLE ProcessHandle,
		__in PROCESSINFOCLASS ProcessInformationClass,
		__out_bcount(ProcessInformationLength) PVOID ProcessInformation,
		__in ULONG ProcessInformationLength,
		__out_opt PULONG ReturnLength
		);
	PFN_ZwQueryInformationProcess _ZwQueryInformationProcess;

	typedef
		BOOLEAN(*PFN_IO_CHECK_FILE_OBJECT_OPENED_AS_COPY_SOURCE)(
			_In_ PFILE_OBJECT FileObject
			);
	PFN_IO_CHECK_FILE_OBJECT_OPENED_AS_COPY_SOURCE _IoCheckFileObjectOpenedAsCopySource;

	typedef
		BOOLEAN(*PFN_IO_CHECK_FILE_OBJECT_OPENED_AS_COPY_DESTINATION)(
			_In_ PFILE_OBJECT FileObject
			);
	PFN_IO_CHECK_FILE_OBJECT_OPENED_AS_COPY_DESTINATION _IoCheckFileObjectOpenedAsCopyDestination;



	void* GetExportedNtFunction(PCUNICODE_STRING functionName)
	{
		return MmGetSystemRoutineAddress(const_cast<PUNICODE_STRING>(functionName));
	}

	void* GetExportedFltFunction(PCSTR functioName)
	{
		return FltGetRoutineAddress(functioName);
	}

	DynamicImports()
	{
		// Initialize members
		_FltGetCopyInformationFromCallbackData = 
			reinterpret_cast<PFN_FltGetCopyInformationFromCallbackData>(GetExportedFltFunction("FltGetCopyInformationFromCallbackData"));

		UNICODE_STRING routineName = RTL_CONSTANT_STRING(L"ZwQueryInformationProcess");
		_ZwQueryInformationProcess = reinterpret_cast<PFN_ZwQueryInformationProcess>(GetExportedNtFunction(&routineName));
		
		routineName = RTL_CONSTANT_STRING(L"IoCheckFileObjectOpenedAsCopySource");;
		_IoCheckFileObjectOpenedAsCopySource = reinterpret_cast<PFN_IO_CHECK_FILE_OBJECT_OPENED_AS_COPY_SOURCE>(GetExportedNtFunction(&routineName));

		routineName = RTL_CONSTANT_STRING(L"IoCheckFileObjectOpenedAsCopyDestination");
		_IoCheckFileObjectOpenedAsCopyDestination = reinterpret_cast<PFN_IO_CHECK_FILE_OBJECT_OPENED_AS_COPY_DESTINATION>(GetExportedNtFunction(&routineName));
	}
};
