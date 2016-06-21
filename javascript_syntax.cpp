
#include "disable_warning.h"

#include "baselib_string.h"
#include "javascript_base.h"
#include "javascript_function.h"
#include "javascript_envirment.h"
#include "javascript_variant.h"

bool eval_for(string& express) {
    trim(express);

    if (check_string("for",express.c_str())) {
        //  for (var var_name=express;term;express) {code_block} or 
        //  for (var var_name=express;term;express) code_line;
        express=express.substr(3);
        trim(express);
        unsigned long left_bracket_index=express.find('(');
        unsigned long right_bracket_index=get_matching_outside_right_bracket(express,0);
        if (INVALID_VALUE!=left_bracket_index && INVALID_VALUE!=right_bracket_index) {
            string term(express.substr(left_bracket_index+1,right_bracket_index-left_bracket_index));
            trim(term);
            unsigned long split_index=term.find(';');
            if (INVALID_VALUE!=split_index) {
                eval(term.substr(0,split_index));
                term=term.substr(split_index+1);
                split_index=term.find(';');
                if (INVALID_VALUE!=split_index) {
                    string for_express(term.substr(split_index+1));
                    for_express=for_express.substr(0,for_express.length()-1);
                    term=term.substr(0,term.find(';'));
                    express=express.substr(right_bracket_index+1);
                    trim(express);
                    unsigned long next_right_brace_index=get_matching_outside_right_brace(express,0);
                    string for_execute_code_block;
                    if ('{'==express[0] && '}'==express[next_right_brace_index]) {
                        for_execute_code_block=express.substr(1,next_right_brace_index-1);
                        express=express.substr(next_right_brace_index+1);
                    } else if (INVALID_VALUE!=express.find(';')) {
                        for_execute_code_block=express.substr(0,express.find(';'));
                        express=express.substr(express.find(';')+1);
                    }
                    trim(for_execute_code_block);
                    trim(express);

                    while (true) {
                        if (!execute_calculation_term(term))
                            return false;
                        unsigned long eval_result=0;
                        support_javascript_variant_type eval_result_type=NONE;
                        get_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)&eval_result,&eval_result_type);
                        if (!eval_result)
                            break;
                        if (!eval(for_execute_code_block))
                            return false;
                        if (!eval(for_express))
                            return false;
                    }
                    return true;
                }
            }
        }
    }
    return false;
}

bool eval_if(string& express) {
    trim(express);

    if (check_string("if",express.c_str())) {              //  if (term2) {if_inside_code_block_2}
        express=express.substr(2);                         //  ->(term2) {if_inside_code_block_2}
    } else if (check_string("else if",express.c_str())) {  //  else if (term2) {if_inside_code_block_2}
        express=express.substr(7);                         //  ->(term2) {if_inside_code_block_2}
    } else if (check_string("else",express.c_str())) {     //  else {if_inside_code_block_3}
        express=express.substr(4);                         //  ->{if_inside_code_block_2}
    }
    trim(express);

    unsigned long left_bracket_index=express.find('('),right_bracket_index=get_matching_outside_right_bracket(express,0);
    string if_term;
    if (INVALID_VALUE!=left_bracket_index && INVALID_VALUE!=right_bracket_index) {  //  check if term syntax ..
        if_term=express.substr(left_bracket_index+1,right_bracket_index-left_bracket_index-1);
        express=express.substr(right_bracket_index+1);
        trim(express);
    } else {
        return false;
    }

    string if_inside_code_block;
    unsigned long matching_right_brace=get_matching_outside_right_brace(express,0);
    if ('{'==express[0] && '}'==express[matching_right_brace]) {
        if_inside_code_block=express.substr(0,matching_right_brace);
        express=express.substr(matching_right_brace+1);
        trim(express);
    } else if (INVALID_VALUE!=express.find(';')) {
        if_inside_code_block=express.substr(0,express.find(';'));
        express=express.substr(express.find(';')+1);
        trim(express);
    } else {
        return false;
    }

    if (!if_term.empty()) {
        if (execute_calculation_term(if_term)) {
            unsigned long term_calcu_result=0;
            support_javascript_variant_type term_calcu_result_type=NONE;
            get_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)&term_calcu_result,&term_calcu_result_type);
            if (term_calcu_result)
                eval(if_inside_code_block);
        } else {
            return false;
        }
    } else {
        eval(if_inside_code_block);
    }

    return true;
}

bool eval_decleare_function(string& express) {
    trim(express);  //  function function_name(function_argment_list) {function_code_block};

    express=express.substr(8);  //  function_name(function_argment_list) {function_code_block};
    trim(express);
    unsigned long left_bracket_index=express.find('('),right_bracket_index=express.find(')');
    if (INVALID_VALUE!=left_bracket_index && INVALID_VALUE!=right_bracket_index) {
        string function_name(express.substr(0,left_bracket_index));
        string function_argment(express.substr(left_bracket_index+1,right_bracket_index-left_bracket_index-1));
        express=express.substr(right_bracket_index+1);  //  {function_code_block};
        trim(express);
        unsigned long matching_right_brace=get_matching_outside_right_brace(express,0);  //  all thinks packet success ,just pick these string and call ..
        if ('{'==express[0] && '}'==express[matching_right_brace]) {
            add_javascript_function("",function_name,function_argment,express.substr(1,matching_right_brace-1));
            express=express.substr(matching_right_brace+1);
            trim(express);
            return true;
        }
    }
    return false;
}
