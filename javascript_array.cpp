
#include "baselib_string.h"
#include "javascript_array.h"

base_array::base_array() {
    this->array_length=1;
    this->array_type_=BASE_ARRAY_CLASS;
    this->array_index=alloc_memory(4);
    *(unsigned long*)this->array_index=0;
}

base_array::base_array(unsigned long init_length) {
    this->array_length=init_length;
    this->array_type_=BASE_ARRAY_CLASS;
    this->array_index=alloc_memory(4*init_length);
    memset(&this->array_index,0,4*init_length);
}

base_array::~base_array() {
    free_memory(this->array_index);
}

void* base_array::get_index(unsigned long index) {
    return (void*)((unsigned long*)(this->array_index)+index);
}

void base_array::set_index(unsigned long index,void* index_data) {
    *((unsigned long*)(this->array_index)+index)=(unsigned long)index_data;
}

unsigned long base_array::length(void) {
    return this->array_length;
}

array_type base_array::type(void) {
    return this->array_type_;
}

int_array::int_array() {
    this->array_length=1;
    this->array_type_=INT_ARRAY_CLASS;
    this->array_index=alloc_memory(4);
    *(unsigned long*)this->array_index=0;
}

int_array::int_array(unsigned long init_length) {
    this->array_length=init_length;
    this->array_type_=INT_ARRAY_CLASS;
    this->array_index=alloc_memory(4*init_length);
    memset(&this->array_index,0,4*init_length);
}

object_array::object_array() {
    this->array_length=1;
    this->array_type_=OBJECT_ARRAY_CLASS;
    this->array_index=alloc_memory(sizeof(object_index));
    memset(&this->array_index,0,sizeof(object_index));
}

object_array::object_array(unsigned long init_length) {
    this->array_length=1;
    this->array_type_=OBJECT_ARRAY_CLASS;
    this->array_index=alloc_memory(sizeof(object_index)*init_length);
    memset(&this->array_index,0,sizeof(object_index)*init_length);
}

void* object_array::get_index(unsigned long index) {
    for (object_index* object_index_point=(object_index*)this->array_index;
                       object_index_point!=NULL;
                       object_index_point=object_index_point->next_object_index)
        if (object_index_point->object_index_number==index)
            return &object_index_point->object_data;
    return NULL;
}

void object_array::set_index(unsigned long index,void* index_data) {
    object_index* last_object_index_point=NULL;
    for (object_index* object_index_point=(object_index*)this->array_index;
                       object_index_point!=NULL;
                       object_index_point=object_index_point->next_object_index) {
        last_object_index_point=object_index_point;
        if (object_index_point->object_index_number==index) {
            memcpy(&object_index_point->object_data,index_data,sizeof(javascript_variant_struct));
            return;
        }
    }
    if (NULL!=last_object_index_point) {
        last_object_index_point->next_object_index=(object_index*)alloc_memory(sizeof(object_index));
        memset(last_object_index_point->next_object_index,0,sizeof(object_index));
    }
}
