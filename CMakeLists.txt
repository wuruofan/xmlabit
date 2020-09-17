cmake_minimum_required(VERSION 3.4)

project(xmlabit CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(CMAKE_C_FLAGS                "-fPIC -Wall")
set(CMAKE_C_FLAGS_DEBUG          "${CMAKE_C_FLAGS} -g -ggdb")
set(CMAKE_C_FLAGS_MINSIZEREL     "${CMAKE_C_FLAGS} Os -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE        "${CMAKE_C_FLAGS} O4 -DNDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS} O2 -g")

set(CMAKE_CXX_FLAGS                "-fPIC -Wall")
set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS} -g -ggdb")
set(CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS} -Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS} -O4 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS} -O2 -g")

aux_source_directory(src xmlabit_src)

add_executable(xmlabit ${xmlabit_src})

set(test_dir "test")
set(test_xml_file "books.xml")

if(NOT EXISTS ${CMAKE_BINARY_DIR}/${test_xml_file})
add_custom_target(copy_xml ALL
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/${test_dir}/${test_xml_file} ${CMAKE_BINARY_DIR}/${test_xml_file}
  DEPENDS xmlabit
  COMMENT "Copying ${test_xml_file} ..."
  )
endif()
