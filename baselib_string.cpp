
#include "disable_warning.h"

#include "global_setting.h"

#include <stdlib.h>
#include <string.h>

#include <string>

using std::string;

HANDLE heap_handle=NULL;

void trim(string& input_string) {
    for (string::iterator index =input_string.begin();
                          index!=input_string.end();
                          ) {
        if (' '==*index) {
            input_string.erase(index);
            index=input_string.begin();
        } else {
            break;
        }
    }
    for (string::reverse_iterator rindex =input_string.rbegin();
                                  rindex!=input_string.rend();
                                  ) {
        if (' '==*rindex) { 
            input_string.erase((++rindex).base());
            rindex=input_string.rbegin();
        } else {
            break;
        }
    }
    if (input_string[input_string.length()-1]==' ')
        input_string.erase((++input_string.rbegin()).base());
}

long hex_string_to_number(const string& input_string) {
    char* end_point=NULL;
    return strtol(input_string.c_str(),&end_point,16);
}

long hex_string_to_number(const char* input_string) {
    char* end_point=NULL;
    return strtol(input_string,&end_point,16);
}

bool check_string(const char* input_string1,const char* input_string2) {
    unsigned long input_string1_length=strlen(input_string1);
    for (unsigned long input_string1_index=0;
                       input_string1_index<input_string1_length;
                       ++input_string1_index)
        if (*(char*)(input_string1+input_string1_index)!=
            *(char*)(input_string2+input_string1_index))
            return false;
    return true;
}

unsigned long get_matching_outside_right_bracket(string& express,unsigned long call_index=0) {
    for (unsigned long index=0;index<express.length();++index) {
        if ('('==express[index]) {
            unsigned long right_bracket_index=get_matching_outside_right_bracket(express.substr(index+1),call_index+1)+index+1;
            if (!call_index)
                return right_bracket_index;
            index=right_bracket_index;
        } else if (')'==express[index]) {
            return index;
        }
    }
    return 0;
}

unsigned long get_matching_outside_right_brace(string& express,unsigned long call_index=0) {
    for (unsigned long index=0;index<express.length();++index) {
        if ('{'==express[index]) {
            unsigned long right_bracket_index=get_matching_outside_right_brace(express.substr(index+1),call_index+1)+index+1;
            if (!call_index)
                return right_bracket_index;
            index=right_bracket_index;
        } else if ('}'==express[index]) {
            return index;
        }
    }
    return 0;
}

void filter_useless_char(string& express) {
    for (string::iterator express_index=express.begin();
                          express_index!=express.end();
                          ++express_index) {
        char express_char=*express_index;
        if ('\t'==express_char || '\n'==express_char || '\r'==express_char)
            *express_index=' ';
    }
}

bool init_heap(void) {
#ifdef HEAP_ALLOC
#ifdef HEAP_EXECUTE_PROTECT
    heap_handle=HeapCreate(NULL,HEAP_SIZE,0);
#else
    heap_handle=HeapCreate(HEAP_CREATE_ENABLE_EXECUTE,HEAP_SIZE,0);
#endif
    if (NULL!=heap_handle)
        return true;
    return false;
#else
    return true;
#endif
}

void* alloc_memory(unsigned long alloc_length) {
    alloc_length=(alloc_length<4)?4:alloc_length;
    void* alloc_address=NULL;
    
#ifdef HEAP_ALLOC
    alloc_address=HeapAlloc(heap_handle,HEAP_ZERO_MEMORY,alloc_length);
#else
#ifdef HEAP_EXECUTE_PROTECT
    alloc_address=VirtualAlloc(NULL,alloc_length,NULL,PAGE_READWRITE);
#else
    alloc_address=VirtualAlloc(NULL,alloc_length,NULL,PAGE_EXECUTE_READWRITE);
#endif
#endif
    if (NULL==alloc_address)
        alloc_address=malloc(alloc_length);
    if (NULL!=alloc_address)
        memset(alloc_address,0,alloc_length);
    return alloc_address;
}

void free_memory(void* alloc_buffer) {
#ifdef HEAP_ALLOC
    HeapFree(heap_handle,NULL,alloc_buffer);
#else
    VirtualFree(alloc_buffer,NULL,NULL);
#endif
}

void conver_coding(char* input_string) {
    unsigned long input_string_length=strlen(input_string);
    for (unsigned long input_string_index=0;
                       input_string_index<input_string_length;
                       ++input_string_index) {
        if ('%'==input_string[input_string_index] &&
            'u'==input_string[input_string_index+1]) {
            if (input_string_index+6<input_string_length) {
                char bit1_string[3]={input_string[input_string_index+2],input_string[input_string_index+3],0};
                char bit2_string[3]={input_string[input_string_index+4],input_string[input_string_index+5],0};
                char bit1=(char)hex_string_to_number(bit1_string);
                char bit2=(char)hex_string_to_number(bit2_string);
                input_string[input_string_index]=bit2;
                input_string[input_string_index+1]=bit1;
                memcpy(&input_string[input_string_index+2],&input_string[input_string_index+6],input_string_length-input_string_index-4+1);  //  +1 for \0
            } else {
                return;
            }
        }
    }
}
