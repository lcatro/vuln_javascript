
#include "baselib_string.h"
#include "javascript_base.h"
#include "javascript_envirment.h"
#include "javascript_variant.h"

bool eval_for(string express) {
    trim(express);
    if (check_string("for",express.c_str())) {
        //  for (var var_name=express;term;express) {code_block} or 
        //  for (var var_name=express;term;express) code_line;
        express=express.substr(3);
        trim(express);
        unsigned long left_bracket_index=express.find('(');
        unsigned long right_bracket_index=express.find(')');
        if (INVALID_VALUE!=left_bracket_index && INVALID_VALUE!=right_bracket_index) {
            string term(express.substr(left_bracket_index+1,right_bracket_index-left_bracket_index-1));
            trim(term);
            unsigned long split_index=term.find(';');
            if (INVALID_VALUE!=split_index) {
                eval(term.substr(0,split_index));
                term=term.substr(split_index+1);
                split_index=term.find(';');
                if (INVALID_VALUE!=split_index) {
                    string for_express(term.substr(term.find(';')+1));
                    term=term.substr(0,term.find(';'));
                    express=express.substr(right_bracket_index+1);
                    trim(express);
                    if ('{'==express[0] && '}'==express[get_matching_outside_right_brace(express,0)])  //  WARNING! ..matchin ERROR ..
                        express=express.substr(1,express.length()-1);
                    else if (INVALID_VALUE!=express.find(';'))
                        express=express.substr(0,express.find(';'));
                    trim(express);

                    while (true) {
                        if (!express_calcu(term))
                            return false;
                        unsigned long eval_result=0;
                        support_javascript_variant_type eval_result_type=NONE;
                        get_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)&eval_result,&eval_result_type);
                        if (!eval_result)
                            break;
                        if (!eval(express))
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

bool eval_if(string express) {
    trim(express);
    if (check_string("if",express.c_str())) {
        //  if (term) {code_block} or if (term) {code_block_1} else {code_block_2} or if (term1) {code_block_1} else if (term2) {code_block_2} else {code_block_3}
        //  if (term) code_line;
        express=express.substr(2);
        trim(express);
        unsigned long left_bracket_index=express.find('('),right_bracket_index=express.find(')');
        if (INVALID_VALUE!=left_bracket_index && INVALID_VALUE!=right_bracket_index) {  //  check if syntax ..
            for (;  INVALID_VALUE!=left_bracket_index && INVALID_VALUE!=right_bracket_index;
                    left_bracket_index=express.find('('),right_bracket_index=express.find(')')) {
                string term(express.substr(left_bracket_index+1,right_bracket_index-left_bracket_index-1));
                trim(term);

                express=express.substr(right_bracket_index+1);
                trim(express);
                string if_inside_code_block;
                unsigned long matching_right_brace=get_matching_outside_right_brace(express,0);
                if ('{'==express[0] && '}'==express[matching_right_brace]) {  //  WARNING! ..matchin ERROR ..
                    if_inside_code_block=express.substr(0,matching_right_brace);
                    express=express.substr(matching_right_brace+1);
                    trim(express);
                } else if (INVALID_VALUE!=express.find(';')) {
                    if_inside_code_block=express.substr(0,express.find(';'));
                    express=express.substr(express.find(';')+1);
                    trim(express);
                }
                if (!express_calcu(term))
                    return false;
                unsigned long term_calcu_result=0;
                support_javascript_variant_type term_calcu_result_type=NONE;
                get_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)&term_calcu_result,&term_calcu_result_type);
                if (term_calcu_result) {
                    if (!eval(if_inside_code_block))
                        return false;
                } else {  //  if (term) {if_inside_code_block} else [if (term2) {if_inside_code_block_2}]
                    if (check_string("else if",express.c_str())) {  //  else if (term2) {if_inside_code_block_2}
                        express=express.substr(5);                  //  ->if (term2) {if_inside_code_block_2}
                    } else if (check_string("else",express.c_str())) {  //  else {if_inside_code_block_3}
                        if (!eval(if_inside_code_block))
                            return false;
                        return true;
                    }
                }
            }
            return true;  //  there are not term could execute if_inside_code_block ..
        }
    }
    return false;
}
