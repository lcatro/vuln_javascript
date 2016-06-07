
#ifndef _JAVASCRIPT_FUNCTION_H__
#define _JAVASCRIPT_FUNCTION_H__

#ifndef _STRING_
#include <string>

using std::string;
#endif

#define JAVASCRIPT_NATIVE_OBJECT_CONSOLE "console"

void init_native_function(void);
bool add_function(string base_object,string function);
bool eval_function(string express);

#endif
