
#ifndef _GLOBAL_SETTING_H__
#define _GLOBAL_SETTING_H__

#pragma warning (disable:4786)

//#define HEAP_EXECUTE_PROTECT

#define PLATFORM_WINDOWS


#ifdef PLATFORM_WINDOWS

#ifndef _WINDOWS_
#include <windows.h>
#endif

extern HANDLE heap_handle;
#endif

#endif
