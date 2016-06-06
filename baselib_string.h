
#ifndef _BASELIB_STRING_H__
#define _BASELIB_STRING_H__

#ifndef _STRING_
#include <string>

using std::string;
#endif

void trim(string& input_string);
long hex_string_to_number(string& input_string);
bool check_string(const char* input_string1,const char* input_string2);
unsigned long get_matching_outside_right_bracket(string& express,unsigned long call_index);
unsigned long get_matching_outside_right_brace(string& express,unsigned long call_index);

#endif
