cmake_minimum_required(VERSION 3.21)
project(ArcConsistency VERSION 0.2.0 LANGUAGES CXX)

include(CTest)
enable_testing()

add_library(ArcConsistency src/arc_consistency.cpp src/var.cpp src/constraint.cpp)
target_compile_features(ArcConsistency PUBLIC cxx_std_17)
target_include_directories(ArcConsistency PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
if(NOT TARGET json)
    add_subdirectory(extern/json)
endif()
add_dependencies(ArcConsistency json)
target_link_libraries(ArcConsistency PUBLIC json)
setup_sanitizers(ArcConsistency)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

set(CPACK_PROJECT_NAME ArcConsistency)
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
