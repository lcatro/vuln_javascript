
#include <stdio.h>

#include <windows.h>

#include "global_setting.h"

/*
    UaF and memory out of bound ..

    Support Document Function :
    document.createElement();     -> HeapAlloc
    document.body.appendChild();  -> None

    Support Element :
    img
    div

    Support Element Function :
    element.remove();             -> HeapFree
    element.innerHTML;            -> Use Element
    element.setAttribute()        -> Use Element

    Support JavaScript :
    var var_name=expression;      -> var var_name=eval(expression)
    var_name.substr();

    Support Expression :
    + - * /
    call();
    new Array();                  -> HeapAlloc

    Support Var Function :
    var_name.substr(l,b);
    var_name[1];
    var_name.attribute=?????;
*/

HANDLE heap_handle=NULL;

bool init_envirment(void) {
#ifdef HEAP_EXECUTE_PROTECT
    heap_handle=HeapCreate(HEAP_CREATE_ENALBE_EXECUTE,HEAP_LENGTH,0);
#else
    heap_handle=HeapCreate(0,HEAP_LENGTH,0);
#endif
    if (NULL!=heap_handle)
        return true;
    heap_handle=GetProcessHeap();
    return false;
}
