
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <string>
#include <map>

#include <windows.h>
#include <oaidl.h>

#include "global_setting.h"
#include "javascript_base.h"

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
    NUMBER=0,  //  VT_INT
    STRING,    //  VT_BSTR
    ARRAY,     //  VT_ARRAY
    OBJECT,    //  VT_STORED_OBJECT
    NONE
};

global_javascript_variant_table_ global_javascript_variant_table;

unsigned long get_variant_index(string& variant_name) {
    unsigned long index=0;
    for (global_javascript_variant_table_::iterator variant_table_iterator=global_javascript_variant_table.begin();
         variant_table_iterator!=global_javascript_variant_table.end();
         ++variant_table_iterator,++index)
        if (variant_name==variant_table_iterator->first)
            return index;
    return INVALID_VALUE;
}

bool is_exist_variant(string& variant_name) {
    if (INVALID_VALUE!=get_variant_index(variant_name))
        return true;
    return false;
}

void set_variant(string variant_name,void* variant_data,support_javascript_variant_type variant_type) {
    if (is_exist_variant(variant_name)) {
        if (STRING==global_javascript_variant_table[variant_name].vt) {
            if (NULL!=global_javascript_variant_table[variant_name].ulVal)
                HeapFree(heap_handle,0,(void*)global_javascript_variant_table[variant_name].ulVal);
            global_javascript_variant_table[variant_name].wReserved3=strlen((const char*)variant_data)+1;
            global_javascript_variant_table[variant_name].ulVal=(unsigned long)HeapAlloc(heap_handle,HEAP_ZERO_MEMORY,global_javascript_variant_table[variant_name].wReserved3);
            memcpy((void*)global_javascript_variant_table[variant_name].ulVal,variant_data,global_javascript_variant_table[variant_name].wReserved3);
        }
    }
    if (NONE==variant_type) {
        global_javascript_variant_table[variant_name].vt=NUMBER;
        global_javascript_variant_table[variant_name].wReserved3=sizeof(int);
        global_javascript_variant_table[variant_name].ulVal=0;
    } else if (NUMBER==variant_type) {
        global_javascript_variant_table[variant_name].vt=NUMBER;
        global_javascript_variant_table[variant_name].wReserved3=sizeof(int);
        global_javascript_variant_table[variant_name].ulVal=(int)variant_data;
    } else if (STRING==variant_type) {
        global_javascript_variant_table[variant_name].vt=STRING;
        global_javascript_variant_table[variant_name].wReserved3=strlen((const char*)variant_data)+1;
        global_javascript_variant_table[variant_name].ulVal=(unsigned long)HeapAlloc(heap_handle,HEAP_ZERO_MEMORY,global_javascript_variant_table[variant_name].wReserved3);
        memcpy((void*)global_javascript_variant_table[variant_name].ulVal,variant_data,global_javascript_variant_table[variant_name].wReserved3);
    } else if (ARRAY==variant_type) {
        global_javascript_variant_table[variant_name].vt=ARRAY;
        //global_javascript_variant_table[variant_name].wReserved3=sizeof(tagARRAYDESC::);
        //global_javascript_variant_table[variant_name].ulVal=(int)variant_data;::tagOBJECTDESCRIPTOR;::tagSAFEARRAY::
    } else if (OBJECT==variant_type) {
        global_javascript_variant_table[variant_name].vt=NUMBER;
        global_javascript_variant_table[variant_name].wReserved3=sizeof(int);
        global_javascript_variant_table[variant_name].ulVal=0;
    }
}

bool get_variant(string variant_name,void* output_variant_data,support_javascript_variant_type* output_variant_type) {
    if (is_exist_variant(variant_name)) {
        *(unsigned long*)output_variant_data=global_javascript_variant_table[variant_name].ulVal;
        *output_variant_type=(support_javascript_variant_type)global_javascript_variant_table[variant_name].vt;
        return true;
    }
    *(unsigned long*)output_variant_data=NULL;
    *output_variant_type=NONE;
    return false;
}

bool copy_variant(string source_variant_name,string destination_variant_name) {
    if (!is_exist_variant(destination_variant_name))
        return false;
    unsigned long variant_data=0;
    support_javascript_variant_type variant_type=NONE;
    get_variant(destination_variant_name,(void*)&variant_data,&variant_type);
    set_variant(source_variant_name,(void*)variant_data,variant_type);
    return true;
}

bool set_variant_array(string variant_name,unsigned long array_index,void* input_variant_data,support_javascript_variant_type input_variant_type) {
    if (is_exist_variant(variant_name)) {
        support_javascript_variant_type variant_type=NONE;
        return true;
    }
    return false;
}

bool get_variant_array(string variant_name,unsigned long array_index,void* output_variant_data,support_javascript_variant_type* output_variant_type) {
    if (is_exist_variant(variant_name)) {
        support_javascript_variant_type variant_type=NONE;
        return true;
    }
    return false;
}

static void trim(string& input_string) {
    for (string::iterator index =input_string.begin();
                          index!=input_string.end();
                          ++index)
        if (' '==*index)
            input_string.erase(index);
        else
            break;
    for (string::reverse_iterator rindex =input_string.rbegin();
                                  rindex!=input_string.rend();
                                  ++rindex) {
        if (' '==*rindex) { 
            input_string.erase((++rindex).base());
            rindex=input_string.rbegin();
        } else {
            break;
        }
    }
    if (input_string[input_string.length()-1]==' ')
        input_string.erase((++input_string.rbegin()).base());
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

enum express_type {
    EXPRESSION_NUMBER_DECIMAL=0,
    EXPRESSION_NUMBER_HEX,
    EXPRESSION_STRING,
    EXPRESSION_UNKNOW
};

static express_type get_express_type(string& express) {
    if ('\''==express[0] && '\''==express[express.length()-1]) {
        return EXPRESSION_STRING;
    } else if (atoi(express.c_str()) || ('0'==express[0] && 1==express.length())) {
        return EXPRESSION_NUMBER_DECIMAL;
    } else if (('0'==express[0] && 'x'==express[1]) || ('x'==express[0])) {
        string number(express.substr(2));
        if (atoi(number.c_str()))
            return EXPRESSION_NUMBER_HEX;
    }
    return EXPRESSION_UNKNOW;
}

static long get_next_calculation_flag(string& express) {
    if (INVALID_VALUE!=express.find("+"))
        return express.find("+");
    else if (INVALID_VALUE!=express.find("-"))
        return express.find("-");
    else if (INVALID_VALUE!=express.find("*"))
        return express.find("*");
    else if (INVALID_VALUE!=express.find("/"))
        return express.find("/");
    else if (INVALID_VALUE!=express.find("("))
        return express.find("(");
    return INVALID_VALUE;
}

static bool is_calculation_express(string& express) {
    //  WARNING !!! 
    express_type base_calculation_type=EXPRESSION_UNKNOW;
    for (long next_calculation_flag=get_next_calculation_flag(express);
              next_calculation_flag!=INVALID_VALUE;
              next_calculation_flag=get_next_calculation_flag(express)) {
        if ('+'==express[next_calculation_flag])) {
            string left_express(express.substr(next_calculation_flag));
            trim(left_express);
            express=express.substr(next_calculation_flag+1);
            express_type left_express_type=get_express_type(left_express);
            next_calculation_flag=get_next_calculation_flag(express);
            if (EXPRESSION_UNKNOW==left_express_type)
                break;
            if (EXPRESSION_UNKNOW==base_calculation_type)
                base_calculation_type=left_express_type;
            if (INVALID_VALUE==next_calculation_flag) {
                string right_express(express);
                trim(right_express);
                express_type left_express_type=get_express_type(left_express);
            }
            set_variant("_function_return_",atoi(express.c_str()),NUMBER);
        } else if ('-'==express[next_calculation_flag]) {
        } else if ('*'==express[next_calculation_flag]) {
        } else if ('/'==express[next_calculation_flag]) {
        } else if ('('==express[next_calculation_flag]) {
        }
    }
    return false;
}

static bool is_function_call(string& express) {
    return false;
}

bool express_calcu(string express,javascript_variant_struct* output_result) {
    trim(express);
    if (is_calculation_express(express)) {
    } else if (is_function_call(express)) {
    }
    express_type express_type_=get_express_type(express);
    if (EXPRESSION_UNKNOW!=express_type_) {
        if (EXPRESSION_NUMBER_DECIMAL==express_type_)
            set_variant("_calcu_temp_variant_",atoi(express.c_str()),NUMBER);
        else if (EXPRESSION_NUMBER_HEX==express_type_)
            set_variant("_calcu_temp_variant_",hex(express.c_str()),NUMBER);
        else if (EXPRESSION_STRING==express_type_)
            set_variant("_calcu_temp_variant_",express.c_str(),STRING);
        return true;
    }
    return false;
}

bool init_envirment(void);

void main(void) {
    init_envirment();
    set_variant("string1",(void*)"1234",STRING);
    set_variant("string2",(void*)"just test!",STRING);
    copy_variant("string_copy","string2");
    long output_data=0;
    support_javascript_variant_type output_type=NONE;
    get_variant("string1",(void*)&output_data,&output_type);
    printf("string1=%s\n",output_data);
    get_variant("string2",(void*)&output_data,&output_type);
    printf("string2=%s\n",output_data);
    get_variant("string_copy",(void*)&output_data,&output_type);
    printf("string_copy=%s\n",output_data);
}
