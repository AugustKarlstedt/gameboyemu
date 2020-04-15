cmake_minimum_required(VERSION 3.14)

### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

project(gameboyemu 
		VERSION 0.01 
		DESCRIPTION "gameboy emulator"
		LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

include(FetchContent)

FetchContent_Declare(
  fmtlib
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        6.1.2
)
FetchContent_MakeAvailable(fmtlib)

set(BUILD_SHARED_LIBS FALSE)
set(SFML_BUILD_NETWORK FALSE)
FetchContent_Declare(
  SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG        2.5.1
)
FetchContent_MakeAvailable(SFML)

configure_file("${PROJECT_SOURCE_DIR}/include/gameboyemu/BuildSettings.h.in" "${PROJECT_BINARY_DIR}/include/gameboyemu/BuildSettings.h")

add_subdirectory(src)
add_subdirectory(app)
