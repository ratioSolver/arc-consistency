cmake_minimum_required(VERSION 3.21)
project(DeltaCSP VERSION 0.2.0 LANGUAGES CXX)

include(CTest)
enable_testing()

add_library(DeltaCSP src/var.cpp)
target_compile_features(DeltaCSP PUBLIC cxx_std_17)
target_include_directories(DeltaCSP PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
if(NOT TARGET json)
    add_subdirectory(extern/json)
endif()
add_dependencies(DeltaCSP json)
target_link_libraries(DeltaCSP PUBLIC json)
setup_sanitizers(DeltaCSP)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

set(CPACK_PROJECT_NAME DeltaCSP)
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
