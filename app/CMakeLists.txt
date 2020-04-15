file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/gameboyemu/*.h")

add_executable(emulator emulator.cpp ${HEADER_LIST})

target_compile_definitions(emulator PRIVATE SFML_STATIC)
target_include_directories(emulator PRIVATE "${SFML_SOURCE_DIR}/include")
target_link_directories(emulator PRIVATE "${SFML_BINARY_DIR}/lib/")

target_link_libraries(emulator emulib sfml-graphics sfml-window sfml-audio fmt::fmt)

target_include_directories(emulator PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_include_directories(emulator PRIVATE "${PROJECT_BINARY_DIR}/include")

source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADER_LIST})