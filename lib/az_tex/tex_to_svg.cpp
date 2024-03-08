#include "az/tex/api.h"
#include "az/js/qjs_wrapper.h"
#include <spdlog/spdlog.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <string>
#include <string_view>

static std::string remove_unexpected_svg_attr(std::string_view sv) {
    boost::property_tree::ptree tree;
    std::istringstream in(sv.data());
    read_xml(in, tree);
    auto svgTree = tree.get_child_optional("svg");
    if (!svgTree) {
        return std::string{sv};
    }
    auto root_attr = svgTree->get_child_optional("<xmlattr>");
    if (!root_attr) {
        return std::string{sv};
    }
    root_attr->erase("role");
    root_attr->erase("focusable");
    root_attr->erase("style");

    root_attr->put("width", "1024");
    root_attr->put("height", "1024");

    std::ostringstream out;
    write_xml(out, tree);
    return out.str();
}

std::string az::tex::convert_tex_to_svg(std::string_view in) {
    std::string raw_svg = az::js::QjsWrapper::get_instance()->mathjax_tex_to_svg(in);
    return remove_unexpected_svg_attr(raw_svg);
}
