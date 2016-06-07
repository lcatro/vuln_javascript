
#include "baselib_string.h"
#include "global_setting.h"
#include "javascript_base.h"
#include "javascript_function.h"
#include "javascript_syntax.h"
#include "javascript_variant.h"

bool eval(string express) {
    trim(express);
    if (check_string("{",express.c_str()) && INVALID_VALUE!=express.find('}')) {  //  inside code block ..
        for (unsigned long code_block_end_index=get_matching_outside_right_brace(express.substr(1),0),
                           code_block_start_index=express.find('{');
                           INVALID_VALUE!=code_block_start_index && INVALID_VALUE!=code_block_end_index;
                           code_block_end_index=get_matching_outside_right_brace(express.substr(1),0),
                           code_block_start_index=express.find('{')) {
            string code_block(express.substr(code_block_start_index+1,code_block_end_index-code_block_start_index));
            while (INVALID_VALUE!=code_block.find(';')) {
                if (!eval(code_block.substr(0,code_block.find(';'))))
                    return false;
                code_block=code_block.substr(code_block.find(';')+1);
            }
            if (!code_block_start_index)
                break;
            express=express.substr(0,code_block_start_index-1)+express.substr(code_block_end_index+1);
        }
        return true;
    }
    if (check_string("for",express.c_str()))  //  base JavaScript syntax ..
        return eval_for(express);
    else if (check_string("if",express.c_str()))
        return eval_if(express);

    string next_express;
    if (INVALID_VALUE!=express.find(';')) {  //  put data ..
        next_express=express.substr(express.find(';')+1);
        express=express.substr(0,express.find(';'));
    }
    if (eval_function(express)) {
    } else if (INVALID_VALUE!=express.find('=')) {  //  var asd=123 or var4='asd'
        char calcu_flag=express[express.find('=')-1];
        if ('+'==calcu_flag) {
            if (INVALID_VALUE!=express.find("+=")) {  //  asd+=123 -> asd=asd+123
                string variant_name(express.substr(0,express.find("+=")));
                trim(variant_name);
                if (eval(variant_name+"="+variant_name+"+"+express.substr(express.find("+=")+2)))
                    copy_variant(variant_name,JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT);
                else
                    return false;
            }
        } else if ('-'==calcu_flag) {
            if (INVALID_VALUE!=express.find("-=")) {  //  asd+=123 -> asd=asd+123
                string variant_name(express.substr(0,express.find("-=")));
                trim(variant_name);
                if (eval(variant_name+"="+variant_name+"-"+express.substr(express.find("-=")+2)))
                    copy_variant(variant_name,JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT);
                else
                    return false;
            }
        } else if ('*'==calcu_flag) {
            if (INVALID_VALUE!=express.find("*=")) {  //  asd+=123 -> asd=asd+123
                string variant_name(express.substr(0,express.find("*=")));
                trim(variant_name);
                if (eval(variant_name+"="+variant_name+"*"+express.substr(express.find("*=")+2)))
                    copy_variant(variant_name,JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT);
                else
                    return false;
            }
        } else if ('/'==calcu_flag) {
            if (INVALID_VALUE!=express.find("/=")) {  //  asd+=123 -> asd=asd+123
                string variant_name(express.substr(0,express.find("/=")));
                trim(variant_name);
                if (eval(variant_name+"="+variant_name+"/"+express.substr(express.find("/=")+2)))
                    copy_variant(variant_name,JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT);
                else
                    return false;
            }
        } else {
            string variant_name;  //  asd
            if (check_string("var",express.c_str()))
                variant_name=express.substr(3,express.find('=')-3);
            else
                variant_name=express.substr(0,express.find('='));
            trim(variant_name);
            string calcu_express(express.substr(express.find('=')+1));

            if (express_calcu(calcu_express))
                copy_variant(variant_name,JAVASCRIPT_VARIANT_KEYNAME_CALCULATION_RESULT);
            else
                return false;
        }
    }
    if (!next_express.empty())
        return eval(next_express);
    return true;
}

HANDLE heap_handle=NULL;

static bool init_heap(void) {
#ifdef HEAP_EXECUTE_PROTECT
    heap_handle=HeapCreate(HEAP_CREATE_ENALBE_EXECUTE,HEAP_LENGTH,0);
#else
    heap_handle=HeapCreate(0,HEAP_LENGTH,0);
#endif
    if (NULL!=heap_handle)
        return true;
    heap_handle=GetProcessHeap();
    return false;
}

bool init_javascript_envirment(void) {
    if (!init_heap())
        return false;
    init_native_function();
    return true;
}


void main(void) {
    init_javascript_envirment();/*
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

//    eval("var a=1+(2+3)+4+((1+1+2)+3);"); <- WARNING! ..
/*
    */
    eval("var a=1;console.log(a);");/*
    eval("var b=1;");
    eval("a=b+1;");
    eval("console.log(a);");
    eval("a=a+1;");
    eval("console.log(a);");
    eval("a+=1;");
    eval("console.log(a);");
    eval("for (var c=1;c<10;c+=1) {a+=1;}");
    eval("console.log(a);");*/
    //eval("{var a=1;{var b=2;}console.log(a+b);}");
    /*
    eval("console.log(1==1);");
    eval("console.log(1>=1);");
    eval("console.log(1==2);");
    eval("console.log(1<3);");*/
}
