#include "pch.h"
#include "FilterFileNameInformation.h"

#ifdef KTL_NAMESPACE
using namespace ktl;
#endif

FilterFileNameInformation::FilterFileNameInformation(PFLT_CALLBACK_DATA data, FileNameOptions options) 
{
	auto status = FltGetFileNameInformation(data, static_cast<FLT_FILE_NAME_OPTIONS>(options), &m_info);
	if (!NT_SUCCESS(status))
	{
		m_info = nullptr;
	}
}

FilterFileNameInformation::FilterFileNameInformation(PFLT_INSTANCE instance, PFILE_OBJECT fileObject, FileNameOptions options)
{
	auto status = STATUS_UNSUCCESSFUL;

	do
	{
		// This is a bit hokey at the moment. We could try a cache lookup first and then do these checks.
		if (IoGetTopLevelIrp() != nullptr)
		{
			break;
		}
		if (KeAreAllApcsDisabled())
		{
			break;
		}

		status = FltGetFileNameInformationUnsafe(fileObject, instance, static_cast<FLT_FILE_NAME_OPTIONS>(options), &m_info);
	} while (false);


	if (!NT_SUCCESS(status))
	{
		m_info = nullptr;
	}
}

FilterFileNameInformation::~FilterFileNameInformation() 
{
	if (m_info)
	{
		FltReleaseFileNameInformation(m_info);
	}
}

NTSTATUS FilterFileNameInformation::Parse() 
{
	return FltParseFileNameInformation(m_info);
}
