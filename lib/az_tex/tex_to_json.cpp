#include "az/tex/api.h"
#include "az/js/qjs_wrapper.h"

std::string az::tex::convert_tex_to_json(std::string_view in) {
    auto qjs = az::js::QjsWrapper::get_instance();
    return qjs->mathjax_tex_to_json(in);
}
