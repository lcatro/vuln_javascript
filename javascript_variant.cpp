
#include "disable_warning.h"

#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <map>

#include <windows.h>
#include <oaidl.h>

#include "baselib_string.h"
#include "global_setting.h"
#include "javascript_base.h"
#include "javascript_variant.h"

using std::map;

typedef map<string,javascript_variant_struct> global_javascript_variant_table_;

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

support_javascript_variant_type get_variant_type(string& variant_name) {
    if (!is_exist_variant(variant_name))
        return NONE;
    return (support_javascript_variant_type)global_javascript_variant_table[variant_name].vt;
}

void set_variant(string variant_name,void* variant_data,support_javascript_variant_type variant_type) {
    if (is_exist_variant(variant_name))
        if (STRING==global_javascript_variant_table[variant_name].vt)
            if (NULL!=global_javascript_variant_table[variant_name].ulVal)
                free_memory((void*)global_javascript_variant_table[variant_name].ulVal);//HeapFree(heap_handle,0,(void*)global_javascript_variant_table[variant_name].ulVal);
    if (NONE==variant_type) {
        global_javascript_variant_table[variant_name].vt=NONE;
        global_javascript_variant_table[variant_name].wReserved3=0;
        global_javascript_variant_table[variant_name].ulVal=0;
    } else if (NUMBER==variant_type) {
        global_javascript_variant_table[variant_name].vt=NUMBER;
        global_javascript_variant_table[variant_name].wReserved3=sizeof(int);
        global_javascript_variant_table[variant_name].ulVal=(int)variant_data;
    } else if (STRING==variant_type) {
        global_javascript_variant_table[variant_name].vt=STRING;
        global_javascript_variant_table[variant_name].wReserved3=strlen((const char*)variant_data)+1;
        global_javascript_variant_table[variant_name].ulVal=(unsigned long)alloc_memory(global_javascript_variant_table[variant_name].wReserved3);//HeapAlloc(heap_handle,HEAP_ZERO_MEMORY,global_javascript_variant_table[variant_name].wReserved3);
        memcpy((void*)global_javascript_variant_table[variant_name].ulVal,variant_data,global_javascript_variant_table[variant_name].wReserved3);
    } else if (ARRAY==variant_type) {
        global_javascript_variant_table[variant_name].vt=ARRAY;
        //global_javascript_variant_table[variant_name].wReserved3=sizeof(tagARRAYDESC::);
        //global_javascript_variant_table[variant_name].ulVal=(int)variant_data;::tagOBJECTDESCRIPTOR;::tagSAFEARRAY::
    } else if (OBJECT==variant_type) {
        global_javascript_variant_table[variant_name].vt=OBJECT;
        global_javascript_variant_table[variant_name].wReserved3=sizeof(int);
        global_javascript_variant_table[variant_name].ulVal=(unsigned long)variant_data;
    }
}

bool get_variant(string variant_name,void* output_variant_data,support_javascript_variant_type* output_variant_type) {
    if (is_exist_variant(variant_name)) {
        if (NONE!=global_javascript_variant_table[variant_name].vt) {
            *(unsigned long*)output_variant_data=global_javascript_variant_table[variant_name].ulVal;
            *output_variant_type=(support_javascript_variant_type)global_javascript_variant_table[variant_name].vt;
            return true;
        }
    }
    *(unsigned long*)output_variant_data=NULL;
    *output_variant_type=NONE;
    return false;
}

void copy_variant(string destination_variant_name,string source_variant_name) {
    unsigned long variant_data=0;
    support_javascript_variant_type variant_type=NONE;
    get_variant(source_variant_name,(void*)&variant_data,&variant_type);
    set_variant(destination_variant_name,(void*)variant_data,variant_type);
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
