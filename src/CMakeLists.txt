file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/gameboyemu/*.h")

add_library(emulib ${SOURCE_LIST} ${HEADER_LIST})

target_link_libraries(emulib fmt::fmt)

target_include_directories(emulib PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_include_directories(emulib PRIVATE "${PROJECT_BINARY_DIR}/include")

source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADER_LIST})