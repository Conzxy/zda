set(CMAKE_CXX_FLAGS_RELEASE  "-O2 -DNDEBUG")
set (CXX_FLAGS -fvisibility=hidden) 
set (C_FLAGS ${CXX_FLAGS})
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
string(REPLACE ";" " " CMAKE_C_FLAGS "${C_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
