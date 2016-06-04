
#include <memory.h>
#include <stdio.h>
#include <string.h>

#include <string>
#include <map>

#include <windows.h>
#include <oaidl.h>

#include "global_setting.h"

using std::string;
using std::map;

typedef tagVARIANT javascript_variant_struct;
typedef map<string,javascript_variant_struct> global_javascript_variant_table_;

/* VARIANT STRUCTURE   <--  oaidl.h
 *
 *  VARTYPE vt;        <--  https://msdn.microsoft.com/en-us/library/windows/desktop/ms221170(v=vs.85).aspx
 *  WORD wReserved1;
 *  WORD wReserved2;
 *  WORD wReserved3;
 *  union {
 *    LONG           VT_I4
 *    BYTE           VT_UI1
 *    SHORT          VT_I2
 *    FLOAT          VT_R4
 *    DOUBLE         VT_R8
 *    VARIANT_BOOL   VT_BOOL
 *    SCODE          VT_ERROR
 *    CY             VT_CY
 *    DATE           VT_DATE
 *    BSTR           VT_BSTR
 *    IUnknown *     VT_UNKNOWN
 *    IDispatch *    VT_DISPATCH
 *    SAFEARRAY *    VT_ARRAY
 *    BYTE *         VT_BYREF|VT_UI1
 *    SHORT *        VT_BYREF|VT_I2
 *    LONG *         VT_BYREF|VT_I4
 *    FLOAT *        VT_BYREF|VT_R4
 *    DOUBLE *       VT_BYREF|VT_R8
 *    VARIANT_BOOL * VT_BYREF|VT_BOOL
 *    SCODE *        VT_BYREF|VT_ERROR
 *    CY *           VT_BYREF|VT_CY
 *    DATE *         VT_BYREF|VT_DATE
 *    BSTR *         VT_BYREF|VT_BSTR
 *    IUnknown **    VT_BYREF|VT_UNKNOWN
 *    IDispatch **   VT_BYREF|VT_DISPATCH
 *    SAFEARRAY **   VT_BYREF|VT_ARRAY
 *    VARIANT *      VT_BYREF|VT_VARIANT
 *    PVOID          VT_BYREF (Generic ByRef)
 *    CHAR           VT_I1
 *    USHORT         VT_UI2
 *    ULONG          VT_UI4
 *    INT            VT_INT
 *    UINT           VT_UINT
 *    DECIMAL *      VT_BYREF|VT_DECIMAL
 *    CHAR *         VT_BYREF|VT_I1
 *    USHORT *       VT_BYREF|VT_UI2
 *    ULONG *        VT_BYREF|VT_UI4
 *    INT *          VT_BYREF|VT_INT
 *    UINT *         VT_BYREF|VT_UINT
 *  }
 */

enum support_javascript_variant_type {
    NUMBER=0;  //  VT_INT
    STRING;    //  VT_BSTR
    ARRAY;     //  VT_ARRAY
    OBJECT     //  VT_STORED_OBJECT
};

global_javascript_variant_table_ global_javascript_variant_table;

unsigned long get_variant_index(string& variant_name) {
    unsigned long index=0;
    for (global_javascript_variant_table_::iterator variant_table_iterator=global_javascript_variant_table.begin();
         variant_table_iterator!=global_javascript_variant_table.end();
         ++variant_table_iterator,++index)
        if (variant_name==variant_table_iterator->first)
            return index;
    return -1;
}

bool is_exist_variant(string& variant_name) {
    if (-1!=get_variant_index(variant_name))
        return true;
    return false;
}

void set_variant(string variant_name,void* variant_data,support_javascript_variant_type variant_type) {
    if (is_exist_variant(variant_name)) {
        if (support_javascript_variant_type.STRING==global_javascript_variant_table[variant_name].vt) {
            if (NULL!=global_javascript_variant_table[variant_name].bstrVal)
                HeapFree(heap_handle,0,global_javascript_variant_table[variant_name].bstrVal);
            global_javascript_variant_table[variant_name].wReserved3=strlen(variant_data);
            global_javascript_variant_table[variant_name].bstrVal=HeapAlloc(heap_handle,HEAP_ZERO_MEMORY,global_javascript_variant_table[variant_name].wReserved3);
            memcpy(global_javascript_variant_table[variant_name].bstrVal,variant_data,global_javascript_variant_table[variant_name].wReserved3);
        }
    }
    if (!strcmp("null",(const char*)variant_data)) {
        global_javascript_variant_table[variant_name].vt=support_javascript_variant_type.NUMBER;
        global_javascript_variant_table[variant_name].wReserved3=sizeof(int);
        global_javascript_variant_table[variant_name].intVal=0;
    } else if (support_javascript_variant_type.NUMBER==variant_type) {
        global_javascript_variant_table[variant_name].vt=support_javascript_variant_type.NUMBER;
        global_javascript_variant_table[variant_name].wReserved3=sizeof(int);
        global_javascript_variant_table[variant_name].intVal=(int)variant_data;
    } else if (support_javascript_variant_type.STRING==variant_type) {
        global_javascript_variant_table[variant_name].vt=support_javascript_variant_type.STRING;
        global_javascript_variant_table[variant_name].wReserved3=strlen(variant_data);
        global_javascript_variant_table[variant_name].bstrVal=HeapAlloc(heap_handle,HEAP_ZERO_MEMORY,global_javascript_variant_table[variant_name].wReserved3);
        memcpy(global_javascript_variant_table[variant_name].bstrVal,variant_data,global_javascript_variant_table[variant_name].wReserved3);
    } else if (support_javascript_variant_type.ARRAY==variant_type) {
        global_javascript_variant_table[variant_name].vt=support_javascript_variant_type.ARRAY;
        //global_javascript_variant_table[variant_name].wReserved3=sizeof(tagARRAYDESC::);
        //global_javascript_variant_table[variant_name].intVal=(int)variant_data;::tagOBJECTDESCRIPTOR;::tagSAFEARRAY::
    } else if (support_javascript_variant_type.OBJECT==variant_type) {
        global_javascript_variant_table[variant_name].vt=support_javascript_variant_type.NUMBER;
        global_javascript_variant_table[variant_name].wReserved3=sizeof(int);
        global_javascript_variant_table[variant_name].pvarVal=0;
    }
}

/*
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

void express_calcu(string express) {
}

void main(void) {
    printf("%d",sizeof(a));
}
