
#include "disable_warning.h"

#include <memory.h>
#include <stdio.h>
#include <string.h>

#include <map>
#include <new>

#include "baselib_string.h"
#include "javascript_array.h"
#include "javascript_base.h"
#include "javascript_element.h"
#include "javascript_envirment.h"
#include "javascript_function.h"
#include "javascript_variant.h"

using std::map;

typedef vector<string> function_argments;

typedef bool (*native_function_type)(function_argments& input_function_argments);
typedef struct {
    bool is_native_function;
    native_function_type native_function;
    function_argments javascript_argment_name;
    string javascript_code;
} function_entry;
typedef map<string,function_entry> object_function_table_;
typedef map<string,object_function_table_> local_function_table_;
local_function_table_ local_function_table;

//  native function ..
static bool console_log(function_argments& input_function_argments) {
    if (!input_function_argments.empty()) {
        unsigned long argment_data=0;
        support_javascript_variant_type argment_data_type=NONE;
        get_variant(input_function_argments[0],(void*)&argment_data,&argment_data_type);
        if (NUMBER==argment_data_type)
            printf("%d\n",argment_data);
        else if (STRING==argment_data_type)
            printf("%s\n",argment_data);
        else if (INT_ARRAY==argment_data_type)
            printf("int_array_addr:%X\n",argment_data);
        else if (OBJECT_ARRAY==argment_data_type)
            printf("object_array_addr:%X\n",argment_data);
        else if (OBJECT==argment_data_type)
            printf("object_addr:%X\n",argment_data);
        else
            printf("undefine\n");
    }
    return true;
}

static bool document_appendChild(function_argments& input_function_argments) {
    return true;
}

static bool document_createElement(function_argments& input_function_argments) {
    if (input_function_argments.empty())
        return false;
    unsigned long argment_element=0;
    support_javascript_variant_type argment_element_type=NONE;
    get_variant(input_function_argments[0],(void*)&argment_element,&argment_element_type);

    if (!strcmp("img",(const char*)argment_element)) {
        img_element* img_element_object=new (alloc_memory(sizeof(img_element))) img_element;  //  new to consume heap ..
        set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)img_element_object,OBJECT);
    } else if (!strcmp("div",(const char*)argment_element)) {
        div_element* div_element_object=new (alloc_memory(sizeof(div_element))) div_element;
        set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)div_element_object,OBJECT);
    } else {
        return false;
    }
    return true;
}

static bool new_int_array(function_argments& input_function_argments) {
    if (input_function_argments.empty()) {  //  new IntArray();
        set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)new int_array(),INT_ARRAY);
    } else if (1==input_function_argments.size()) {  //  new IntArray(1);
        unsigned long argment_array_length=0;
        support_javascript_variant_type argment_array_length_type=NONE;
        get_variant(input_function_argments[0],(void*)&argment_array_length,&argment_array_length_type);
        if (NUMBER!=argment_array_length_type)
            return false;
        set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)new int_array(argment_array_length),INT_ARRAY);
    } else {  //  new IntArray(1,2,3,4,...);
        int_array* int_array_class=new int_array(input_function_argments.size());
        unsigned long init_array_index=0;
        for (function_argments::iterator function_argments_index=input_function_argments.begin();
                                         function_argments_index!=input_function_argments.end();
                                         ++function_argments_index) {
            unsigned long argment_array_data=0;
            support_javascript_variant_type argment_array_data_type=NONE;
            get_variant(*function_argments_index,(void*)&argment_array_data,&argment_array_data_type);
            if (NUMBER!=argment_array_data_type)
                return false;
            int_array_class->set_index(init_array_index++,(void*)argment_array_data);
        }
        set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)int_array_class,INT_ARRAY);
    }
    return true;
}

static bool new_obj_array(function_argments& input_function_argments) {
    if (input_function_argments.empty()) {  //  new ObjArray();
        set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)new object_array(),OBJECT_ARRAY);
    } else if (1==input_function_argments.size()) {  //  new ObjArray(1);
        unsigned long argment_array_length=0;
        support_javascript_variant_type argment_array_length_type=NONE;
        get_variant(input_function_argments[0],(void*)&argment_array_length,&argment_array_length_type);
        if (NUMBER!=argment_array_length_type)
            return false;
        set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)new object_array(argment_array_length),OBJECT_ARRAY);
    }
    return true;
}

static bool string_object_substr(string& object,function_argments& input_function_argments) {
    unsigned long return_string=0;
    if (!input_function_argments.empty()) {
        if (2<=input_function_argments.size()) {  //  substr(offset)
            unsigned long string_buffer=0;
            unsigned long string_buffer_length=0;
            support_javascript_variant_type string_buffer_type=NONE;
            get_variant(object,(void*)&string_buffer,&string_buffer_type);
            string_buffer_length=strlen((const char*)string_buffer);

            unsigned long argment_offset=0;
            support_javascript_variant_type argment_offset_type=NONE;
            get_variant(input_function_argments[0],(void*)&argment_offset,&argment_offset_type);

            unsigned long argment_length=0;
            if (2==input_function_argments.size()) {  //  substr(offset,length)
                support_javascript_variant_type argment_length_type=NONE;
                get_variant(input_function_argments[1],(void*)&argment_length,&argment_length_type);
            } else {
                argment_length=string_buffer_length-argment_offset;
            }
            char* temp_string_buffer=(char*)alloc_memory(argment_length+1);
            if (NULL!=temp_string_buffer) {
                memcpy(temp_string_buffer,(const char*)string_buffer,argment_length);
                set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)temp_string_buffer,STRING);
                free_memory(temp_string_buffer);
                return true;
            }
        }
    }
    set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)JAVASCRIPT_UNDEFINE,STRING);
    return false;
}

static bool string_object_length(string& object) {
    unsigned long string_buffer=0;
    support_javascript_variant_type string_buffer_type=NONE;
    get_variant(object,(void*)&string_buffer,&string_buffer_type);
    set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)strlen((const char*)string_buffer),NUMBER);
    return true;
}

static bool array_object_length(string& object) {
    base_array* array_class=NULL;
    support_javascript_variant_type array_class_type=NONE;
    get_variant(object,(void*)&array_class,&array_class_type);
    set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)array_class->length(),NUMBER);
    return true;
}

void init_native_function(void) {
    local_function_table[JAVASCRIPT_NATIVE_OBJECT_CONSOLE]["log"].is_native_function=true;
    local_function_table[JAVASCRIPT_NATIVE_OBJECT_CONSOLE]["log"].native_function=console_log;
    local_function_table[JAVASCRIPT_NATIVE_OBJECT_DOCUMENT]["createElement"].is_native_function=true;
    local_function_table[JAVASCRIPT_NATIVE_OBJECT_DOCUMENT]["createElement"].native_function=document_createElement;
    local_function_table[JAVASCRIPT_NATIVE_OBJECT_DOCUMENT]["appendChild"].is_native_function=true;
    local_function_table[JAVASCRIPT_NATIVE_OBJECT_DOCUMENT]["appendChild"].native_function=document_appendChild;
}

static bool is_exist_native_object(string object) {
    for (local_function_table_::iterator local_function_table_index=local_function_table.begin();
                                         local_function_table_index!=local_function_table.end();
                                         ++local_function_table_index)
        if (local_function_table_index->first==object)
            return true;
    return false;
}

static bool is_exist_native_object_function(string object,string function) {
    for (local_function_table_::iterator local_function_table_index=local_function_table.begin();
                                         local_function_table_index!=local_function_table.end();
                                         ++local_function_table_index) {
        if (local_function_table_index->first==object) {
            for (object_function_table_::iterator object_function_table_index=local_function_table_index->second.begin();
                                                  object_function_table_index!=local_function_table_index->second.end();
                                                  ++object_function_table_index) {
                if (object_function_table_index->first==function) {
                    return true;
                }
            }
        }
    }
    return false;
}

static bool call_javascript_object_native_function(string base_object,string function_name,function_argments function_argments_list) {
    if (is_exist_variant(base_object)) {
        support_javascript_variant_type variant_type=get_variant_type(base_object);
        if (STRING==variant_type) {
            if ("substr"==function_name)
                return string_object_substr(base_object,function_argments_list);
            else if ("length"==function_name)
                return string_object_length(base_object);
        } else if (OBJECT==variant_type) {
            base_element* element_object=NULL;
            get_variant(base_object,(void*)&element_object,&variant_type);
            if ("remove"==function_name) {
                element_object->remove();
                return true;
            } else if ("setAttribute"==function_name) {
                if (2==function_argments_list.size()) {
                    unsigned long attribute_name=0;
                    support_javascript_variant_type attribute_name_type=NONE;
                    get_variant(function_argments_list[0],(void*)&attribute_name,&attribute_name_type);
                    unsigned long attribute_data=0;
                    support_javascript_variant_type attribute_data_type=NONE;
                    get_variant(function_argments_list[1],(void*)&attribute_data,&attribute_data_type);
                    element_object->setAttribute((const char*)attribute_name,(void*)attribute_data);
                    return true;
                }
            } else if ("getAttribute"==function_name) {
                if (1==function_argments_list.size()) {
                    unsigned long attribute_name=0;
                    support_javascript_variant_type attribute_name_type=NONE;
                    get_variant(function_argments_list[0],(void*)&attribute_name,&attribute_name_type);
                    element_object->getAttribute((const char*)attribute_name);  //  WARNING! 有个地方占用了free 之后的东西,记得分析..
                    return true;
                }
            }
        } else if (INT_ARRAY==variant_type) {
            if ("length"==function_name)
                return array_object_length(base_object);
        } else if (OBJECT_ARRAY==variant_type) {
            if ("length"==function_name)
                return array_object_length(base_object);
        }
    }
    return false;
}

bool add_javascript_function(string base_object,string function_name,string function_argments_express,string function_code) {
    function_argments function_argments_list;
    if (!function_argments_express.empty()) {
        string function_argments_name;
        while (INVALID_VALUE!=function_argments_express.find(',')) {
            function_argments_name=function_argments_express.substr(0,function_argments_express.find(','));
            trim(function_argments_name);
            function_argments_list.push_back(function_argments_name);
            function_argments_express=function_argments_express.substr(function_argments_express.find(',')+1);
        }
        trim(function_argments_express);
        function_argments_list.push_back(function_argments_express);
    }
    local_function_table[base_object][function_name].is_native_function=false;
    local_function_table[base_object][function_name].native_function=NULL;
    local_function_table[base_object][function_name].javascript_argment_name=function_argments_list;
    local_function_table[base_object][function_name].javascript_code=function_code;
    return true;
}

static bool eval_javascript_function(string& object_name,string& function_name,function_argments function_argments_list) {
    if (!local_function_table[object_name][function_name].is_native_function) {
        unsigned long copy_function_argments_index=0;
        unsigned long copy_function_argments_count=function_argments_list.size();

        for (function_argments::iterator javascript_function_argments_name_index=local_function_table[object_name][function_name].javascript_argment_name.begin();
                                         javascript_function_argments_name_index!=local_function_table[object_name][function_name].javascript_argment_name.end();
                                         ++javascript_function_argments_name_index) {
            if (copy_function_argments_index<copy_function_argments_count) {
                copy_variant(*javascript_function_argments_name_index,function_argments_list[copy_function_argments_index++]);
            } else {
                set_variant(*javascript_function_argments_name_index,0,NONE);
            }
        }
        return eval(local_function_table[object_name][function_name].javascript_code);
    }
    return false;
}

bool eval_function(string express) {  //  console.log(express); or console.log(express1,express2)
    trim(express);
    unsigned long first_left_bracket_index=express.find('(');
    unsigned long match_right_bracket_index=get_matching_outside_right_bracket(express,0);
    unsigned long equal_index=express.find('=');
    if (INVALID_VALUE==first_left_bracket_index || INVALID_VALUE==match_right_bracket_index || INVALID_VALUE!=equal_index)
        return false;

    string function_name(express.substr(0,first_left_bracket_index));
    string function_argment(express.substr(first_left_bracket_index+1,match_right_bracket_index-first_left_bracket_index-1));
    function_argments function_argments_list;
    unsigned long function_argments_index=0;
    for (unsigned long split_index=function_argment.find(',');
                       split_index!=INVALID_VALUE;
                       split_index=function_argment.find(','),++function_argments_index) {
        if (!express_calcu(function_argment.substr(0,split_index)))
            return false;
        string function_argment_variant_index("function_argment");
        char conver_buffer[0x20]={0};
        sprintf(conver_buffer,"%d",function_argments_index);
        function_argment_variant_index+=conver_buffer;
        copy_variant(function_argment_variant_index,JAVASCRIPT_VARIANT_KEYNAME_CALCULATION_RESULT);
        function_argments_list.push_back(function_argment_variant_index);
        function_argment=function_argment.substr(function_argment.find(',')+1);
    }
    if (!function_argment.empty())
        if (!express_calcu(function_argment))
            return false;
    string function_argment_variant_index("function_argment");
    char conver_buffer[0x20]={0};
    sprintf(conver_buffer,"%d",function_argments_index);
    function_argment_variant_index+=conver_buffer;
    copy_variant(function_argment_variant_index,JAVASCRIPT_VARIANT_KEYNAME_CALCULATION_RESULT);
    function_argments_list.push_back(function_argment_variant_index);

    if (check_string("new",function_name.c_str())) {
        function_name=function_name.substr(4);
        trim(function_name);
        if ("IntArray"==function_name)
            return new_int_array(function_argments_list);
        else if ("ObjArray"==function_name)
            return new_obj_array(function_argments_list);
    } else {
        string object_name;
        if (INVALID_VALUE!=function_name.find('.')) {
            object_name=function_name.substr(0,function_name.find('.'));
            function_name=function_name.substr(function_name.find('.')+1);
        }
        trim(object_name);
        trim(function_name);
        if (is_exist_variant(object_name))
            return call_javascript_object_native_function(object_name,function_name,function_argments_list);
        if (is_exist_native_object_function(object_name,function_name)) {
            if (local_function_table[object_name][function_name].is_native_function) {
                return local_function_table[object_name][function_name].native_function(function_argments_list);
            } else {
                return eval_javascript_function(object_name,function_name,function_argments_list);
            }
        }
    }
    return true;
}

bool eval_function_return(string& express) {
    unsigned long split_index=express.find(' ');
    unsigned long end_index=express.find(';');
    if (INVALID_VALUE!=split_index && INVALID_VALUE!=end_index) {
        express_calcu(express.substr(split_index+1,end_index-split_index-1));
        copy_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,JAVASCRIPT_VARIANT_KEYNAME_CALCULATION_RESULT);
        return true;
    }
    return false;
}
