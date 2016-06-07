
#ifndef _JAVASCRIPT_ELEMENT_H__
#define _JAVASCRIPT_ELEMENT_H__

#ifndef _STRING_
#include <string>

using std::string;
#endif

#ifndef _VECTOR_
#include <vector>

using std::vector;
#endif

class base_element {
 public:
  base_element();
  ~base_element();

  virtual void remove(void);
  virtual void getAttribute(string attribute);
  virtual void setAttribute(string attribute,void* data);

 private:
  typedef vector<string> attribute_string_list;
  typedef struct {
      attribute_string_list attribute_name_list;
      unsigned long         attribute_data_list[1];
  } attribute_table_;

  attribute_table_ attribute_table;
};

class img_element : public base_element {
};

class div_element : public base_element {
};

#endif
