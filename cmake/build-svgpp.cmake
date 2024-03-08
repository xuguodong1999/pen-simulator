# svgpp
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/svgpp-src/svgpp)
add_library(svgpp INTERFACE)
target_include_directories(svgpp INTERFACE ${ROOT_DIR}/include)
xgd_link_libraries(svgpp INTERFACE
        xml2
        boost_gil
        boost_math
        boost_mpl
        boost_spirit
        boost_parameter
        boost_assert
        boost_range
        boost_format
        boost_tti
        boost_utility
        boost_optional
        boost_multi_array)
