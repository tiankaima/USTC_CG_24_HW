if (WIN32)
    add_definitions(-D_WIN32)
    add_definitions(-DAsShared_PolynomialList)

    add_definitions(-DExport_PolynomialList)
endif ()

add_library(PolynomialListLib SHARED ./PolynomialList.cpp)

target_include_directories(PolynomialListLib PUBLIC
        ${PROJECT_SOURCE_DIR}/include)

set_target_properties(PolynomialListLib PROPERTIES ${OUTPUT_PROP})
