cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(zda VERSION 1.0)

aux_source_directory(src ZDA_SRC)

add_library(zda ${ZDA_SRC})
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