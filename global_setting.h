
#ifndef _GLOBAL_SETTING_H__
#define _GLOBAL_SETTING_H__

#pragma warning (disable:4786)

#define HEAP_EXECUTE_PROTECT
#define HEAP_LENGTH                (1024*1000*3000)  //  300 MB
#define HEAP_CREATE_ENALBE_EXECUTE 0x00040000

#define PLATFORM_WINDOWS


#ifdef PLATFORM_WINDOWS

#ifndef _WINDOWS_
#include <windows.h>
#endif

extern HANDLE heap_handle;
#endif

#endif
