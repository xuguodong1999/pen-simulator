function(xgd_build_third_party)
    set(XGD_THIRD_PARTY_LIBS
            abseil ade avalontoolkit
            arrow
            boost
            cairo coordgen cpufeatures
            eigen
            flatbuffers
            freesasa freetype
            gtest
            inchi
            lbfgs libpng libxml2
            maeparser
            openbabel opencv
            pixman protobuf
            quickjs
            rapidjson rdkit ringdecomposerlib
            skia
            spdlog svgpp
            taskflow tinyxml2
            thrift
            yaehmop
            zlib zstd)
    foreach (XGD_THIRD_PARTY_LIB ${XGD_THIRD_PARTY_LIBS})
        include(${CMAKE_SOURCE_DIR}/cmake/build-${XGD_THIRD_PARTY_LIB}.cmake)
    endforeach ()
endfunction()

xgd_build_third_party()
