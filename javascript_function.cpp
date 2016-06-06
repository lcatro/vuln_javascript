
#include <memory.h>
#include <stdio.h>

#include <vector>

#include "baselib_string.h"
#include "javascript_base.h"
#include "javascript_variant.h"

using std::vector;

typedef vector<string> function_argments;

static void console_log(function_argments& input_function_argments) {
    if (input_function_argments.empty())
        return;
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

bool eval_function(string express) {  //  console.log(express); or console.log(express1,express2)
    trim(express);
    unsigned long first_left_bracket_index=express.find('(');
    unsigned long match_right_bracket_index=get_matching_outside_right_bracket(express,0);
    if (INVALID_VALUE==first_left_bracket_index || INVALID_VALUE==match_right_bracket_index)
        return false;

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

    if (check_string("console.log",express.c_str())) {
        console_log(function_argments_list);
    }
    return true;
}

