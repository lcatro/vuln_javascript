
#include "disable_warning.h"

#include <memory.h>

#include "baselib_string.h"
#include "global_setting.h"
#include "javascript_base.h"
#include "javascript_element.h"
#include "javascript_variant.h"

base_element::base_element() {
    this->attribute_table.attribute_name_list.clear();
    memset(&this->attribute_table.attribute_data_list,0,sizeof(this->attribute_table.attribute_data_list));
}

base_element::~base_element() {
}

void base_element::remove(void) {
#ifdef HEAP_ALLOC
    free_memory(this);
#else
    delete this;
#endif
}

void base_element::getAttribute(string attribute) {
    unsigned long attribute_index=0;
    set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)0,NUMBER);
    for (attribute_string_list::iterator attribute_iterator=this->attribute_table.attribute_name_list.begin();
                                         attribute_iterator!=this->attribute_table.attribute_name_list.end();
                                         ++attribute_iterator,++attribute_index)
        if (attribute==*attribute_iterator)
            set_variant(JAVASCRIPT_VARIANT_KEYNAME_FUNCTION_RESULT,(void*)this->attribute_table.attribute_data_list[attribute_index],NUMBER);
}

void base_element::setAttribute(string attribute,void* data) {
    unsigned long attribute_index=0;
    for (attribute_string_list::iterator attribute_iterator=this->attribute_table.attribute_name_list.begin();
                                         attribute_iterator!=this->attribute_table.attribute_name_list.end();
                                         ++attribute_iterator,++attribute_index)
        if (attribute==*attribute_iterator)
            goto SET_VALUE;
    this->attribute_table.attribute_name_list.push_back(attribute);
SET_VALUE:
    this->attribute_table.attribute_data_list[attribute_index]=(unsigned long)data;
}
