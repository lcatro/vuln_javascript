
#ifndef _BASELIB_STRING_H__
#define _BASELIB_STRING_H__

#ifndef _STRING_
#include <string>

using std::string;
#endif

void trim(string& input_string);
long hex_string_to_number(const string& input_string);
long hex_string_to_number(const char* input_string);
bool check_string(const char* input_string1,const char* input_string2);
unsigned long get_matching_outside_right_bracket(string& express,unsigned long call_index);
unsigned long get_matching_outside_right_brace(string& express,unsigned long call_index);
void filter_useless_char(string& express);
void conver_coding(char* input_string);

bool init_heap(void);
void* alloc_memory(unsigned long alloc_length);
void free_memory(void* alloc_buffer);

#endif
