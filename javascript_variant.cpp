
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

void set_variant(string variant_name,void* variant_data,support_javascript_variant_type variant_type) {
    if (is_exist_variant(variant_name))
        if (STRING==global_javascript_variant_table[variant_name].vt)
            if (NULL!=global_javascript_variant_table[variant_name].ulVal)
                HeapFree(heap_handle,0,(void*)global_javascript_variant_table[variant_name].ulVal);
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

static long get_next_calculation_flag(string& express) {
    if (INVALID_VALUE!=express.find("("))
        return express.find("(");
    else if (INVALID_VALUE!=express.find("+"))
        return express.find("+");
    else if (INVALID_VALUE!=express.find("-"))
        return express.find("-");
    else if (INVALID_VALUE!=express.find("*"))
        return express.find("*");
    else if (INVALID_VALUE!=express.find("/"))
        return express.find("/");
    return INVALID_VALUE;
}

static express_type get_express_type(string& express) {
    if ('\''==express[0] && '\''==express[express.length()-1]) {  //  WARNING! it can not correct check express ..
        return EXPRESSION_STRING;
    } else if (('('==express[0] && ')'==express[express.length()-1]) ||
               INVALID_VALUE!=get_next_calculation_flag(express)){
        return EXPRESSION_EXPRESS;
    } else if (atoi(express.c_str()) || ('0'==express[0] && 1==express.length())) {
        return EXPRESSION_NUMBER_DECIMAL;
    } else if (('0'==express[0] && 'x'==express[1]) || ('x'==express[0])) {
        string number(express.substr(2));
        if (atoi(number.c_str()))
            return EXPRESSION_NUMBER_HEX;
    } else if (is_exist_variant(express)) {
        return EXPRESSION_VARIANT;
    }
    return EXPRESSION_UNKNOW;
}

static bool execute_calculation_express(string& express) {
    long next_calculation_flag=get_next_calculation_flag(express);
    if ('+'==express[next_calculation_flag]) {
        string left_express(express.substr(0,next_calculation_flag));
        trim(left_express);
        express_type left_express_type=get_express_type(left_express);
        unsigned long left_express_calcu_value=0;
        support_javascript_variant_type left_express_calcu_value_type=NONE;
        if (EXPRESSION_UNKNOW==left_express_type)  //  12kk4+321 or +321
            return false;
        if (EXPRESSION_EXPRESS==left_express_type || EXPRESSION_VARIANT==left_express_type) {  //  (123+321)+1
            if (!execute_calculation_express(left_express))
                return false;
            get_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)&left_express_calcu_value,&left_express_calcu_value_type);
            if (NUMBER==left_express_calcu_value_type) {
                left_express_type=EXPRESSION_NUMBER_DECIMAL;
            } else {
                left_express_type=EXPRESSION_STRING;
                left_express=(const char*)left_express_calcu_value;
            }
        } else if (EXPRESSION_STRING==left_express_type) { //  'AAA'+'A'
            left_express=left_express.substr(1,left_express.length()-2);
        }

        string right_express(express.substr(next_calculation_flag+1));
        trim(right_express);
        express_type right_express_type=get_express_type(right_express);
        unsigned long right_express_calcu_value=0;
        support_javascript_variant_type right_express_calcu_value_type=NONE;
        if (EXPRESSION_UNKNOW==right_express_type)  //  321+12kk4 or 321+
            return false;
        if (EXPRESSION_EXPRESS==right_express_type || EXPRESSION_VARIANT==right_express_type) {  //  123+123+123
            if (!execute_calculation_express(right_express))
                return false;
            get_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)&right_express_calcu_value,&right_express_calcu_value_type);
            if (NUMBER==right_express_calcu_value_type) {
                right_express_type=EXPRESSION_NUMBER_DECIMAL;
            } else {
                right_express_type=EXPRESSION_STRING;
                right_express=(const char*)right_express_calcu_value;
            }
        } else if (EXPRESSION_STRING==right_express_type) {  //  'AAA'+'A'
            right_express=right_express.substr(1,right_express.length()-2);
        }
        //  123+123 or (123+321)+123+123 or '123'+'123' or '123'+123
        if (EXPRESSION_STRING==left_express_type && EXPRESSION_STRING!=right_express_type) {
            if (NONE!=right_express_calcu_value_type) {
                string connect_string(left_express);
                connect_string+=right_express;
                set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)connect_string.c_str(),STRING);
            } else {
                char conver_buffer[0x20]={0};
                sprintf(conver_buffer,"%d",right_express_calcu_value);
                string connect_string(left_express);
                connect_string+=conver_buffer;
                set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)connect_string.c_str(),STRING);
            }
        } else if (EXPRESSION_STRING!=left_express_type && EXPRESSION_STRING==right_express_type) {
            if (NONE==left_express_calcu_value_type) {
                string connect_string(left_express);
                connect_string+=right_express;
                set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)connect_string.c_str(),STRING);
            } else {
                char conver_buffer[0x20]={0};
                sprintf(conver_buffer,"%d",left_express_calcu_value);
                string connect_string(conver_buffer);
                connect_string+=right_express;
                set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)connect_string.c_str(),STRING);
            }
        } else if (EXPRESSION_STRING==left_express_type && EXPRESSION_STRING==right_express_type) {
            string connect_string;
            if (NONE!=left_express_calcu_value_type)
                connect_string=(const char*)left_express_calcu_value;
            else
                connect_string=left_express;
            if (NONE!=right_express_calcu_value_type)
                connect_string+=(const char*)right_express_calcu_value;
            else
                connect_string=right_express;
            set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)connect_string.c_str(),STRING);
        } else {
            long left_express_value=0;
            if (NONE!=left_express_calcu_value_type) {
                left_express_value=left_express_calcu_value;
            } else {
                if (EXPRESSION_NUMBER_HEX==left_express_calcu_value_type)
                    left_express_value=hex_string_to_number(left_express);
                else
                    left_express_value=atoi(left_express.c_str());
            }
            long right_express_value=0;
            if (NONE!=right_express_calcu_value_type) {
                right_express_value=right_express_calcu_value;
            } else {
                if (EXPRESSION_NUMBER_HEX==right_express_calcu_value_type)
                    right_express_value=hex_string_to_number(right_express);
                else
                    right_express_value=atoi(right_express.c_str());
            }
            left_express_value+=right_express_value;
            set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)left_express_value,NUMBER);
        }
        return true;
    } else if ('-'==express[next_calculation_flag]) {
        string left_express(express.substr(0,next_calculation_flag));
        trim(left_express);
        express_type left_express_type=get_express_type(left_express);
        unsigned long left_express_calcu_value=0;
        support_javascript_variant_type left_express_calcu_value_type=NONE;
        if (EXPRESSION_UNKNOW==left_express_type)
            return false;
        if (EXPRESSION_EXPRESS==left_express_type || EXPRESSION_VARIANT==left_express_type) {
            if (!execute_calculation_express(left_express))
                return false;
            get_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)&left_express_calcu_value,&left_express_calcu_value_type);
            if (NUMBER==left_express_calcu_value_type)
                left_express_type=EXPRESSION_NUMBER_DECIMAL;
        } else if (EXPRESSION_NUMBER_HEX==left_express_type) {
            left_express_calcu_value=hex_string_to_number(left_express);
        } else if (EXPRESSION_NUMBER_DECIMAL==left_express_type) {
            left_express_calcu_value=atoi(left_express.c_str());
        }

        string right_express(express.substr(next_calculation_flag+1));
        trim(right_express);
        express_type right_express_type=get_express_type(right_express);
        unsigned long right_express_calcu_value=0;
        support_javascript_variant_type right_express_calcu_value_type=NONE;
        if (EXPRESSION_UNKNOW==right_express_type)
            return false;
        if (EXPRESSION_EXPRESS==right_express_type || EXPRESSION_VARIANT==right_express_type) {
            if (!execute_calculation_express(right_express))
                return false;
            get_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)&right_express_calcu_value,&right_express_calcu_value_type);
            if (NUMBER==right_express_calcu_value_type)
                right_express_type=EXPRESSION_NUMBER_DECIMAL;
        } else if (EXPRESSION_NUMBER_HEX==right_express_type) {
            right_express_calcu_value=hex_string_to_number(right_express);
        } else if (EXPRESSION_NUMBER_DECIMAL==right_express_type) {
            right_express_calcu_value=atoi(right_express.c_str());
        }
        left_express_calcu_value-=right_express_calcu_value;
        set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)left_express_calcu_value,NUMBER);
        return true;
    } else if ('*'==express[next_calculation_flag]) {
        string left_express(express.substr(0,next_calculation_flag));
        trim(left_express);
        express_type left_express_type=get_express_type(left_express);
        unsigned long left_express_calcu_value=0;
        support_javascript_variant_type left_express_calcu_value_type=NONE;
        if (EXPRESSION_UNKNOW==left_express_type)
            return false;
        if (EXPRESSION_EXPRESS==left_express_type || EXPRESSION_VARIANT==left_express_type) {
            if (!execute_calculation_express(left_express))
                return false;
            get_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)&left_express_calcu_value,&left_express_calcu_value_type);
            if (NUMBER==left_express_calcu_value_type)
                left_express_type=EXPRESSION_NUMBER_DECIMAL;
        } else if (EXPRESSION_NUMBER_HEX==left_express_type) {
            left_express_calcu_value=hex_string_to_number(left_express);
        } else if (EXPRESSION_NUMBER_DECIMAL==left_express_type) {
            left_express_calcu_value=atoi(left_express.c_str());
        }

        string right_express(express.substr(next_calculation_flag+1));
        trim(right_express);
        express_type right_express_type=get_express_type(right_express);
        unsigned long right_express_calcu_value=0;
        support_javascript_variant_type right_express_calcu_value_type=NONE;
        if (EXPRESSION_UNKNOW==right_express_type)
            return false;
        if (EXPRESSION_EXPRESS==right_express_type || EXPRESSION_VARIANT==right_express_type) {
            if (!execute_calculation_express(right_express))
                return false;
            get_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)&right_express_calcu_value,&right_express_calcu_value_type);
            if (NUMBER==right_express_calcu_value_type)
                right_express_type=EXPRESSION_NUMBER_DECIMAL;
        } else if (EXPRESSION_NUMBER_HEX==right_express_type) {
            right_express_calcu_value=hex_string_to_number(right_express);
        } else if (EXPRESSION_NUMBER_DECIMAL==right_express_type) {
            right_express_calcu_value=atoi(right_express.c_str());
        }
        left_express_calcu_value*=right_express_calcu_value;
        set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)left_express_calcu_value,NUMBER);
        return true;
    } else if ('/'==express[next_calculation_flag]) {
        string left_express(express.substr(0,next_calculation_flag));
        trim(left_express);
        express_type left_express_type=get_express_type(left_express);
        unsigned long left_express_calcu_value=0;
        support_javascript_variant_type left_express_calcu_value_type=NONE;
        if (EXPRESSION_UNKNOW==left_express_type)
            return false;
        if (EXPRESSION_EXPRESS==left_express_type || EXPRESSION_VARIANT==left_express_type) {
            if (!execute_calculation_express(left_express))
                return false;
            get_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)&left_express_calcu_value,&left_express_calcu_value_type);
            if (NUMBER==left_express_calcu_value_type)
                left_express_type=EXPRESSION_NUMBER_DECIMAL;
        } else if (EXPRESSION_NUMBER_HEX==left_express_type) {
            left_express_calcu_value=hex_string_to_number(left_express);
        } else if (EXPRESSION_NUMBER_DECIMAL==left_express_type) {
            left_express_calcu_value=atoi(left_express.c_str());
        }

        string right_express(express.substr(next_calculation_flag+1));
        trim(right_express);
        express_type right_express_type=get_express_type(right_express);
        unsigned long right_express_calcu_value=0;
        support_javascript_variant_type right_express_calcu_value_type=NONE;
        if (EXPRESSION_UNKNOW==right_express_type)
            return false;
        if (EXPRESSION_EXPRESS==right_express_type || EXPRESSION_VARIANT==right_express_type) {
            if (!execute_calculation_express(right_express))
                return false;
            get_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)&right_express_calcu_value,&right_express_calcu_value_type);
            if (NUMBER==right_express_calcu_value_type)
                right_express_type=EXPRESSION_NUMBER_DECIMAL;
        } else if (EXPRESSION_NUMBER_HEX==right_express_type) {
            right_express_calcu_value=hex_string_to_number(right_express);
        } else if (EXPRESSION_NUMBER_DECIMAL==right_express_type) {
            right_express_calcu_value=atoi(right_express.c_str());
        }
        left_express_calcu_value/=right_express_calcu_value;
        set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)left_express_calcu_value,NUMBER);
        return true;
    } else if ('('==express[next_calculation_flag]) {
        string resolve_express(express);
        for (unsigned long left_bracket_index=resolve_express.find('(');
                           left_bracket_index!=INVALID_VALUE;
                           left_bracket_index=resolve_express.find('(')) {
            unsigned long next_right_bracket=get_matching_outside_right_bracket(express,0);
            if (!next_right_bracket)
                return false;
            if (!execute_calculation_express(resolve_express.substr(left_bracket_index+1,next_right_bracket-left_bracket_index-1)))
                return false;
            unsigned long express_calcu_result_value=0;
            support_javascript_variant_type express_calcu_result_value_type=NONE;
            get_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)&express_calcu_result_value,&express_calcu_result_value_type);
            resolve_express=express.substr(0,left_bracket_index);
            if (NUMBER==express_calcu_result_value_type) {
                char conver_buffer[0x20]={0};
                sprintf(conver_buffer,"%d",express_calcu_result_value);
                resolve_express+=conver_buffer;
            } else {
                resolve_express+="\'";
                resolve_express+=(const char*)express_calcu_result_value;
                resolve_express+="\'";
            }
            resolve_express+=express.substr(next_right_bracket+1);
        }
        if (!execute_calculation_express(resolve_express))
            return false;
        return true;
    }
    if (is_exist_variant(express)) {
        copy_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,express);
        return true;
    }
    return false;
}

static bool execute_function_call(string& express) {
    return false;
}

static bool execute_calculation_term(string& express) {
    if (INVALID_VALUE!=express.find("==")) {
        if (express_calcu(express.substr(0,express.find("==")))) {
            copy_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,JAVASCRIPT_VARIANT_KEYNAME_CALCULATION_RESULT);
            if (express_calcu(express.substr(express.find("==")+2))) {
                unsigned long left_term_data=0;
                support_javascript_variant_type left_term_data_type=NONE;
                unsigned long right_term_data=0;
                support_javascript_variant_type right_term_data_type=NONE;
                get_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)&left_term_data,&left_term_data_type);
                get_variant(JAVASCRIPT_VARIANT_KEYNAME_CALCULATION_RESULT,(void*)&right_term_data,&right_term_data_type);
                if (left_term_data_type!=right_term_data_type) {
                    left_term_data=0;
                    left_term_data_type=NUMBER;
                    set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)left_term_data,left_term_data_type);
                } else if (NUMBER==left_term_data_type) {
                    if (left_term_data==right_term_data)
                        left_term_data=1;
                    else
                        left_term_data=0;
                    left_term_data_type=NUMBER;
                    set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)left_term_data,left_term_data_type);
                } else if (STRING==left_term_data_type) {
                    if (!strcmp((const char*)left_term_data,(const char*)right_term_data))
                        left_term_data=1;
                    else
                        left_term_data=0;
                    left_term_data_type=NUMBER;
                    set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)left_term_data,left_term_data_type);
                }
                return true;
            }
        }
    } else if (INVALID_VALUE!=express.find("!=")) {
        express.replace(express.find("!="),2,"==");
        if (express_calcu(express)) {
            unsigned long calcu_term_value=0;
            support_javascript_variant_type calcu_term_value_type=NONE;
            get_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)&calcu_term_value,&calcu_term_value_type);
            calcu_term_value=(calcu_term_value)?0:1;
            set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)calcu_term_value,calcu_term_value_type);
            return true;
        }
    } else if (INVALID_VALUE!=express.find("<")) {
        if (express_calcu(express.substr(0,express.find('<')))) {
            copy_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,JAVASCRIPT_VARIANT_KEYNAME_CALCULATION_RESULT);
            if (express_calcu(express.substr(express.find('<')+1))) {
                unsigned long left_term_data=0;
                support_javascript_variant_type left_term_data_type=NONE;
                unsigned long right_term_data=0;
                support_javascript_variant_type right_term_data_type=NONE;
                get_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)&left_term_data,&left_term_data_type);
                get_variant(JAVASCRIPT_VARIANT_KEYNAME_CALCULATION_RESULT,(void*)&right_term_data,&right_term_data_type);
                if (left_term_data_type==right_term_data_type && NUMBER==left_term_data_type) {
                    if (left_term_data<right_term_data)
                        left_term_data=1;
                    else
                        left_term_data=0;
                    left_term_data_type=NUMBER;
                    set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)left_term_data,left_term_data_type);
                    return true;
                }
            }
        }
    } else if (INVALID_VALUE!=express.find(">=")) {
        express.replace(express.find(">="),2,"<");
        if (express_calcu(express)) {
            unsigned long calcu_term_value=0;
            support_javascript_variant_type calcu_term_value_type=NONE;
            get_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)&calcu_term_value,&calcu_term_value_type);
            calcu_term_value=(calcu_term_value)?0:1;
            set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)calcu_term_value,calcu_term_value_type);
            return true;
        }
    } else if (INVALID_VALUE!=express.find(">")) {
        if (express_calcu(express.substr(0,express.find('>')))) {
            copy_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,JAVASCRIPT_VARIANT_KEYNAME_CALCULATION_RESULT);
            if (express_calcu(express.substr(express.find('>')+1))) {
                unsigned long left_term_data=0;
                support_javascript_variant_type left_term_data_type=NONE;
                unsigned long right_term_data=0;
                support_javascript_variant_type right_term_data_type=NONE;
                get_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)&left_term_data,&left_term_data_type);
                get_variant(JAVASCRIPT_VARIANT_KEYNAME_CALCULATION_RESULT,(void*)&right_term_data,&right_term_data_type);
                if (left_term_data_type==right_term_data_type && NUMBER==left_term_data_type) {
                    if (left_term_data>right_term_data)
                        left_term_data=1;
                    else
                        left_term_data=0;
                    left_term_data_type=NUMBER;
                    set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)left_term_data,left_term_data_type);
                    return true;
                }
            }
        }
    } else if (INVALID_VALUE!=express.find("<=")) {
        express.replace(express.find("<="),2,">");
        if (express_calcu(express)) {
            unsigned long calcu_term_value=0;
            support_javascript_variant_type calcu_term_value_type=NONE;
            get_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)&calcu_term_value,&calcu_term_value_type);
            calcu_term_value=(calcu_term_value)?0:1;
            set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)calcu_term_value,calcu_term_value_type);
            return true;
        }
    }
    return false;
}

bool express_calcu(string express) {
    trim(express);
    if (execute_calculation_express(express)) {
        copy_variant(JAVASCRIPT_VARIANT_KEYNAME_CALCULATION_RESULT,JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT);
        return true;
    } else if (execute_function_call(express)) {
        copy_variant(JAVASCRIPT_VARIANT_KEYNAME_CALCULATION_RESULT,JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT);
        return true;
    } else if (execute_calculation_term(express)) {
        copy_variant(JAVASCRIPT_VARIANT_KEYNAME_CALCULATION_RESULT,JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT);
        return true;
    }
    express_type express_type_=get_express_type(express);
    if (EXPRESSION_UNKNOW!=express_type_) {
        if (EXPRESSION_NUMBER_DECIMAL==express_type_)
            set_variant(JAVASCRIPT_VARIANT_KEYNAME_CALCULATION_RESULT,(void*)atoi(express.c_str()),NUMBER);
        else if (EXPRESSION_NUMBER_HEX==express_type_)
            set_variant(JAVASCRIPT_VARIANT_KEYNAME_CALCULATION_RESULT,(void*)hex_string_to_number(express),NUMBER);
        else if (EXPRESSION_STRING==express_type_)
            set_variant(JAVASCRIPT_VARIANT_KEYNAME_CALCULATION_RESULT,(void*)(express.substr(1,express.length()-2)).c_str(),STRING);
        return true;
    }
    return false;
}

/*
void main(void) {
    init_envirment();
    /*
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
    printf("string_copy=%s\n",output_data);*/
    /*
    string calcu("1+(2+(1+1))");
    if (execute_calculation_express(calcu)) {
        long return_data=0;
        support_javascript_variant_type return_type=NONE;
        get_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)&return_data,&return_type);
        printf("Calcu=%d",return_data);
    } else
        printf("ERR");
        */
    /*
    string a("0x100");
    printf("%d",hex_string_to_number(a));*/
    /*
    string express("(()()");
    printf("%d\n",get_matching_outside_right_bracket(express,0));
    express="()()";
    printf("%d\n",get_matching_outside_right_bracket(express,0));
    express="(()())";
    printf("%d\n",get_matching_outside_right_bracket(express,0));
    express=")";
    printf("%d\n",get_matching_outside_right_bracket(express,0));
}
*/
