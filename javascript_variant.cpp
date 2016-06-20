
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
#include "javascript_array.h"
#include "javascript_base.h"
#include "javascript_variant.h"

using std::map;

typedef map<string,javascript_variant_struct> global_javascript_variant_table_;

global_javascript_variant_table_ global_javascript_variant_table;

unsigned long get_variant_index(string& variant_name) {
    unsigned long index=0;
    try {
        for (global_javascript_variant_table_::iterator variant_table_iterator=global_javascript_variant_table.begin();
                                                        variant_table_iterator!=global_javascript_variant_table.end();
                                                        ++variant_table_iterator,++index)
            if (variant_name==variant_table_iterator->first)
                return index;
    } catch (...) {
        //  i dont know why iterate global_javascript_variant_table will make error of access exception ..
    }
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
                free_memory((void*)global_javascript_variant_table[variant_name].ulVal);
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
        conver_coding((char*)variant_data);
        global_javascript_variant_table[variant_name].wReserved3=sizeof(int);

        unsigned long alloc_length=sizeof(javascript_variant_string)+strlen((const char*)variant_data)+1;
        void* alloc_address=alloc_memory(alloc_length);
        ((javascript_variant_string*)alloc_address)->string_length=strlen((const char*)variant_data);
        ((javascript_variant_string*)alloc_address)->string_data=(char*)((unsigned long)alloc_address+sizeof(javascript_variant_string));
        memcpy((void*)((unsigned long)alloc_address+sizeof(javascript_variant_string)),variant_data,alloc_length-sizeof(javascript_variant_string));

        global_javascript_variant_table[variant_name].ulVal=(unsigned long)alloc_address;
    } else if (INT_ARRAY==variant_type) {
        global_javascript_variant_table[variant_name].vt=INT_ARRAY;
        global_javascript_variant_table[variant_name].wReserved3=sizeof(int);
        global_javascript_variant_table[variant_name].ulVal=(unsigned long)variant_data;
    } else if (OBJECT_ARRAY==variant_type) {
        global_javascript_variant_table[variant_name].vt=OBJECT_ARRAY;
        global_javascript_variant_table[variant_name].wReserved3=sizeof(int);
        global_javascript_variant_table[variant_name].ulVal=(unsigned long)variant_data;
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
            if (STRING==*output_variant_type)
                *(unsigned long*)output_variant_data=(unsigned long)((javascript_variant_string*)global_javascript_variant_table[variant_name].ulVal)->string_data;
            else
                *(unsigned long*)output_variant_data=global_javascript_variant_table[variant_name].ulVal;
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
        if (INT_ARRAY==get_variant_type(variant_name)) {
            if (NUMBER==input_variant_type) {
                int_array* array_class=NULL;
                support_javascript_variant_type variant_type=NONE;
                get_variant(variant_name,&array_class,&variant_type);
                array_class->set_index(array_index,input_variant_data);
                return true;
            }
        } else {
            object_array* array_class=NULL;
            support_javascript_variant_type variant_type=NONE;
            get_variant(variant_name,&array_class,&variant_type);
            javascript_variant_struct object_index_variant={0};
            object_index_variant.vt=input_variant_type;
            if (STRING==input_variant_type)
                object_index_variant.wReserved3=strlen((const char*)input_variant_data);
            else
                object_index_variant.wReserved3=sizeof(int);
            object_index_variant.ulVal=(unsigned long)input_variant_data;
            array_class->set_index(array_index,&object_index_variant);
            return true;
        }
    }
    return false;
}

bool get_variant_array(string variant_name,unsigned long array_index,void* output_variant_data,support_javascript_variant_type* output_variant_type) {
    if (is_exist_variant(variant_name)) {
        base_array* array_class=NULL;
        support_javascript_variant_type array_type=NONE;
        get_variant(variant_name,&array_class,&array_type);
        if (INT_ARRAY==array_type) {
            *(unsigned long*)output_variant_data=(unsigned long)((int_array*)array_class)->get_index(array_index);
            *output_variant_type=NONE;
        } else {
            javascript_variant_struct* array_index_object=(javascript_variant_struct*)((object_array*)array_class)->get_index(array_index);
            *(unsigned long*)output_variant_data=array_index_object->ulVal;
            *output_variant_type=(support_javascript_variant_type)array_index_object->vt;
        }
        return true;
    }
    return false;
}
