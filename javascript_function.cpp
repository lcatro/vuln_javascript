
#include "disable_warning.h"

#include <memory.h>
#include <stdio.h>

#include <map>
#include <vector>

#include "baselib_string.h"
#include "javascript_base.h"
#include "javascript_function.h"
#include "javascript_variant.h"

using std::map;
using std::vector;

typedef vector<string> function_argments;

typedef bool (*native_function_type)(function_argments& input_function_argments);
typedef struct {
    bool is_native_function;
    native_function_type native_function;
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
        else if (ARRAY==argment_data_type)
            printf("unsupport ARRAY\n",argment_data);
        else if (OBJECT==argment_data_type)
            printf("unsupport object\n",argment_data);
        else
            printf("undefine\n");
    }
    return true;
}

static bool document_appendChild(function_argments& input_function_argments) {
    return true;
}

static bool document_createElement(function_argments& input_function_argments) {
    return true;
}

bool add_javascript_function(string base_object,string function_name,string function_code) {
    return false;
}

void init_native_function(void) {
    local_function_table[JAVASCRIPT_NATIVE_OBJECT_CONSOLE]["log"].is_native_function=true;
    local_function_table[JAVASCRIPT_NATIVE_OBJECT_CONSOLE]["log"].native_function=console_log;
    local_function_table[JAVASCRIPT_NATIVE_OBJECT_DOCUMENT]["createElement"].is_native_function=true;
    local_function_table[JAVASCRIPT_NATIVE_OBJECT_DOCUMENT]["createElement"].native_function=console_log;
    local_function_table[JAVASCRIPT_NATIVE_OBJECT_DOCUMENT]["appendChild"].is_native_function=true;
    local_function_table[JAVASCRIPT_NATIVE_OBJECT_DOCUMENT]["appendChild"].native_function=console_log;
}

static bool is_exist_object(string object) {
    for (local_function_table_::iterator local_function_table_index=local_function_table.begin();
                                         local_function_table_index!=local_function_table.end();
                                         ++local_function_table_index)
        if (local_function_table_index->first==object)
            return true;
    return false;
}

static bool is_exist_object_function(string object,string function) {
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

bool eval_javascript_function(string express,function_argments function_argments_list) {
    return false;
}

bool eval_function(string express) {  //  console.log(express); or console.log(express1,express2)
    trim(express);
    unsigned long first_left_bracket_index=express.find('(');
    unsigned long match_right_bracket_index=get_matching_outside_right_bracket(express,0);
    if (INVALID_VALUE==first_left_bracket_index || INVALID_VALUE==match_right_bracket_index)
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
    }
    if (!express_calcu(function_argment))
        return false;
    string function_argment_variant_index("function_argment");
    char conver_buffer[0x20]={0};
    sprintf(conver_buffer,"%d",function_argments_index);
    function_argment_variant_index+=conver_buffer;
    copy_variant(function_argment_variant_index,JAVASCRIPT_VARIANT_KEYNAME_CALCULATION_RESULT);
    function_argments_list.push_back(function_argment_variant_index);

    if (INVALID_VALUE!=function_name.find('.')) {
        string object_name(function_name.substr(0,function_name.find('.')));
        string function_name(function_name.substr(function_name.find('.')+1));
        trim(object_name);
        trim(function_name);
        if (!is_exist_object_function(object_name,function_name))
            return false;
        if (local_function_table[object_name][function_name].is_native_function)
            return local_function_table[object_name][function_name].native_function(function_argments_list);
        return eval_javascript_function(local_function_table[object_name][function_name].javascript_code,function_argments_list);
    }
    return true;
}
