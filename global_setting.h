
#ifndef _GLOBAL_SETTING_H__
#define _GLOBAL_SETTING_H__

#pragma warning (disable:4786)

//#define HEAP_EXECUTE_PROTECT

#define PLATFORM_WINDOWS

#ifdef PLATFORM_WINDOWS

#ifndef _WINDOWS_
#include <windows.h>
#endif

#endif


//#define HEAP_ALLOC  //  use HeapAlloc not VritualAlloc
#ifdef HEAP_ALLOC

#define HEAP_SIZE (200*1024*1024)
#define HEAP_CREATE_ENABLE_EXECUTE 0x00040000

#endif

#endif
