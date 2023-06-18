cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(zda VERSION 1.0)

set (CXX_FLAGS -fvisibility=hidden) 
set (C_FLAGS ${CXX_FLAGS})
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
string(REPLACE ";" " " CMAKE_C_FLAGS "${C_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")

aux_source_directory(src ZDA_SRC)

add_library(zda ${ZDA_SRC})
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
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/kvarint>"
)

include(GNUInstallDirs)

###############################################
# Installation
###############################################
install(TARGETS zda DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY zda DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

add_subdirectory(test)