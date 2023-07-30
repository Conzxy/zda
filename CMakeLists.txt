  cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(zda VERSION 1.0)

include(cmake/cache_var.cmake)
include(cmake/compile_flag.cmake)
include(cmake/install.cmake)

aux_source_directory(src ZDA_SRC)
file(GLOB_RECURSE ZDA_INCLUDE zda/*.h zda/*.hpp)

message(STATUS "Headers = ${ZDA_INCLUDE}")
message(STATUS "Sources = ${ZDA_SRC}")
message(STATUS "BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
add_library(zda ${ZDA_SRC} ${ZDA_INCLUDE})
if (BUILD_SHARED_LIBS)
  target_compile_definitions(zda 
    PRIVATE ZDA_BUILD_SHARED
  )
else ()
  target_compile_definitions(zda
    PUBLIC ZDA_STATIC
  )
endif ()

target_include_directories(zda
  PUBLIC 
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zda>"
)

###############################################
# Installation
###############################################
message(STATUS "Generating (${PROJECT_NAME}) install files...")
conzxy_install(PROJECT zda NAMESPACE zda TARGETS zda)

install(TARGETS zda DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY zda DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if (ZDA_INCLUDE_TEST)
  add_subdirectory(test)
endif ()

if (ZDA_INCLUDE_BENCHMARK)
  add_subdirectory(benchmark)
endif ()
