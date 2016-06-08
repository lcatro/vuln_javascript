
#ifndef _JAVASCRIPT_ARRAY_H__
#define _JAVASCRIPT_ARRAY_H__

#ifndef _JAVASCRIPT_VARIANT_H__
#include "javascript_variant.h"
#endif

enum array_type {
    BASE_ARRAY_CLASS=0,
    INT_ARRAY_CLASS,
    OBJECT_ARRAY_CLASS
};

class base_array {
 public:
  base_array();
  base_array(unsigned long init_length);
  ~base_array();

  virtual void* get_index(unsigned long index);
  virtual void  set_index(unsigned long index,void* index_data);

  virtual unsigned long length(void);
  virtual array_type type(void);

 protected:
  unsigned long array_length;
  array_type    array_type_;
  void*         array_index;
};

class int_array : public base_array {
 public:
  int_array();
  int_array(unsigned long init_length);
};

class object_array : public base_array {
 public:
  object_array();
  object_array(unsigned long init_length);

  virtual void* get_index(unsigned long index);
  virtual void  set_index(unsigned long index,void* index_data);

 private:
  typedef struct object_index_ {
      unsigned long object_index_number;
      javascript_variant_struct object_data;
      object_index_* next_object_index;
  } object_index;
};

#endif
