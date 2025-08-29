#pragma once

#ifdef KTL_NAMESPACE
using namespace ktl;
#endif

enum class FileNameOptions {
	Normalized = FLT_FILE_NAME_NORMALIZED,
	Opened = FLT_FILE_NAME_OPENED,
	Short = FLT_FILE_NAME_SHORT,

	QueryDefault = FLT_FILE_NAME_QUERY_DEFAULT,
	QueryCacheOnly = FLT_FILE_NAME_QUERY_CACHE_ONLY,
	QueryFileSystemOnly = FLT_FILE_NAME_QUERY_FILESYSTEM_ONLY,

	RequestFromCurrentProvider = FLT_FILE_NAME_REQUEST_FROM_CURRENT_PROVIDER,
	DoNotCache = FLT_FILE_NAME_DO_NOT_CACHE,
	AllowQueryOnReparse = FLT_FILE_NAME_ALLOW_QUERY_ON_REPARSE
};
DEFINE_ENUM_FLAG_OPERATORS(FileNameOptions);

struct FilterFileNameInformation {
	FilterFileNameInformation(_In_ PFLT_CALLBACK_DATA data, _In_opt_ FileNameOptions options = DefaultFilenameOptions);
	FilterFileNameInformation(_In_opt_ PFLT_INSTANCE instance, _In_ PFILE_OBJECT fileObject, _In_opt_ FileNameOptions options = DefaultFilenameOptions);
	~FilterFileNameInformation();

	operator bool() const {
		return m_info != nullptr;
	}

	PFLT_FILE_NAME_INFORMATION Get() const {
		return m_info;
	}

	operator PFLT_FILE_NAME_INFORMATION() const {
		return Get();
	}

	PFLT_FILE_NAME_INFORMATION operator->() {
		return m_info;
	}

	NTSTATUS Parse();

private:
	PFLT_FILE_NAME_INFORMATION m_info;
	static const FileNameOptions DefaultFilenameOptions = FileNameOptions::QueryDefault | FileNameOptions::Normalized;
};

#ifdef KTL_NAMESPACE
}
#endif

