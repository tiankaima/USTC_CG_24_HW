project(view)
file(GLOB source
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${INCLUDE_DIR}/view/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/shapes/*.cpp"
        "${INCLUDE_DIR}/view/shapes/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/warp/*.cpp"
        "${INCLUDE_DIR}/view/warp/*.h"
)
find_package (Eigen3 3.3 REQUIRED NO_MODULE)
add_library(${PROJECT_NAME} ${source})
set_target_properties(${PROJECT_NAME} PROPERTIES
        DEBUG_POSTFIX "_d"
        RUNTIME_OUTPUT_DIRECTORY "${BINARY_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY "${LIBRARY_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC ${THIRD_PARTY_DIR}/annoy/src)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw glad imgui Eigen3::Eigen)
target_include_directories(${PROJECT_NAME}
        PUBLIC ${INCLUDE_DIR}
        PUBLIC ${THIRD_PARTY_DIR}
)