
#ifndef _JAVASCRIPT_FUNCTION_H__
#define _JAVASCRIPT_FUNCTION_H__

#ifndef _STRING_
#include <string>

using std::string;
#endif

#define JAVASCRIPT_NATIVE_OBJECT_CONSOLE "console"
#define JAVASCRIPT_NATIVE_OBJECT_DOCUMENT "document"

void init_native_function(void);
bool add_javascript_function(string base_object,string function_name,string function_argments_express,string function_code);
bool eval_function(string express);
bool eval_function_return(string& express);

#endif
