
#include "baselib_string.h"
#include "global_setting.h"
#include "javascript_base.h"
#include "javascript_function.h"
#include "javascript_variant.h"

bool eval(string express) {
    trim(express);
    if (check_string("{",express.c_str()) && INVALID_VALUE!=express.find('}')) {  //  inside code block ..
        for (unsigned long code_block_end_index=get_matching_outside_right_brace(express.substr(1),0),
                           code_block_start_index=express.find('{')+1;
                           INVALID_VALUE!=code_block_end_index;
                           code_block_end_index=get_matching_outside_right_brace(express.substr(1),0),
                           code_block_start_index=express.find('{')+1) {
            string code_block(express.substr(code_block_start_index,code_block_end_index));
            while (INVALID_VALUE!=code_block.find(';')) {
                if (!eval(code_block.substr(0,code_block.find(';'))))
                    return false;
                code_block=code_block.substr(code_block.find(';')+1);
            }
            express=express.substr(0,code_block_start_index)+express.substr(code_block_end_index+1);
        }
        return true;
    }
    if (INVALID_VALUE!=express.find(';')) 
        express=express.substr(0,express.find(';'));
    if (INVALID_VALUE!=express.find('=')) {  //  var asd=123 or var4='asd'
        string variant_name;  //  asd
        if (check_string("var",express.c_str()))
            variant_name=express.substr(3,express.find('=')-3);
        else
            variant_name=express.substr(0,express.find('=')-1);
        trim(variant_name);
        string calcu_express(express.substr(express.find('=')+1));
        if (express_calcu(calcu_express)) {
            unsigned long calcu_value=0;
            support_javascript_variant_type calcu_value_type=NONE;
            get_variant(JAVASCRIPT_VARIANT_KEYNAME_CALCULATION_RESULT,(void*)&calcu_value,&calcu_value_type);
            copy_variant(variant_name,JAVASCRIPT_VARIANT_KEYNAME_CALCULATION_RESULT);
        }
        return true;
    } else if (eval_function(express)) {
        return true;
    } else if (check_string("for",express.c_str())) {
    }
    return false;
}

bool init_envirment(void);

void main(void) {
    init_envirment();/*
    eval("var a=1;");
    eval("var b=3");
    eval("var c='AAAAAA';");
    eval("var d=c;");
    eval("e=a;");
    eval("console.log(a);");
    eval("console.log(c);");
    eval("console.log(d);");
    eval("console.log(e);");
    eval("console.log(2-1);");
    eval("console.log(1*2);");
    eval("console.log(4/1);");
    eval("console.log(a+c)");
    eval("console.log(c+c)");*/

    eval("{var a=1;var b=2;console.log(a+b);}");
}
