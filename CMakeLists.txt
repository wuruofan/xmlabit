cmake_minimum_required(VERSION 3.4)

project(xmlabit CXX)

# compile flags
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

# check out pugixml repo by git submodule
set(PUGIXML_REPO "pugixml")

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/${PUGIXML_REPO}/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

# pugixml static libraray
aux_source_directory(pugixml/src pugi_src)
add_library(pugi_library STATIC ${pugi_src})

# xmlabit
aux_source_directory(src xmlabit_src)
include_directories("${PUGIXML_REPO}/src")

add_executable(xmlabit ${xmlabit_src} ${pugi_src})
target_link_libraries(xmlabit ${pugi_library})


# copy test xml file to build/
set(test_dir "test")
set(test_xml_file "books.xml")

if(NOT EXISTS ${CMAKE_BINARY_DIR}/${test_xml_file})
add_custom_target(copy_xml ALL
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/${test_dir}/${test_xml_file} ${CMAKE_BINARY_DIR}/${test_xml_file}
  DEPENDS xmlabit
  COMMENT "Copying ${test_xml_file} ..."
  )
endif()
