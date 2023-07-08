#ifndef __ZDA_STIRNG_VIEW_HPP___
#define __ZDA_STIRNG_VIEW_HPP___

#include "zda/string_view.h"

namespace zda {

class StringView {
 public:
  using size_type = size_t;

  StringView(char const *data, size_t len)
  {
    zda_string_view_init(&view_, data, len);
  }
   
  
 private:
  zda_string_view_t view_;
};

}

#endif /* guard */