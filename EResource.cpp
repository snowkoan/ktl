#include "pch.h"
#include "EResource.h"

#ifdef KTL_NAMESPACE
using namespace ktl;
#endif


void EResource::Init() {
	ExInitializeResourceLite(&m_Res);
}

void EResource::Delete() {
	ExDeleteResourceLite(&m_Res);
}

void EResource::Lock() {
	ExEnterCriticalRegionAndAcquireResourceExclusive(&m_Res);
}

void EResource::Unlock() {
	ExReleaseResourceAndLeaveCriticalRegion(&m_Res);
}

void EResource::LockShared() {
	ExEnterCriticalRegionAndAcquireResourceShared(&m_Res);
}

void EResource::UnlockShared() {
	Unlock();
}
