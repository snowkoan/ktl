#include "pch.h"

#include "Process.h"

NTSTATUS CompleteRequest(PIRP Irp, NTSTATUS status, ULONG_PTR info, CCHAR boost) {
	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = info;
	IoCompleteRequest(Irp, boost);
	return status;
}

#define va_start __va_start
#define va_end(x) 

#ifdef DBG
void __cdecl Error(PCSTR format, ...) {
	va_list args;
	va_start(&args, format);
	vDbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, format, args);
	va_end(args);
}

void __cdecl Warning(PCSTR format, ...) {
	va_list args;
	va_start(&args, format);
	vDbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_WARNING_LEVEL, format, args);
	va_end(args);
}
void __cdecl Info(PCSTR format, ...) {
	va_list args;
	va_start(&args, format);
	vDbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, format, args);
	va_end(args);
}
void __cdecl Trace(PCSTR format, ...) {
	va_list args;
	va_start(&args, format);
	vDbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_TRACE_LEVEL, format, args);
	va_end(args);
}
#else
void Error(PCSTR, ...) {}
void Warning(PCSTR, ...) {}
void Info(PCSTR, ...) {}
void Trace(PCSTR, ...) {}
#endif
