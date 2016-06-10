
#ifndef _JAVASCRIPT_ENVITMENT_H__
#define _JAVASCRIPT_ENVITMENT_H__

#ifndef _STRING_
#include <string>

using std::string;
#endif

bool init_javascript_envirment(void);
bool eval(string express);
bool express_calcu(string express);
bool execute_calculation_term(string& express);

#endif
