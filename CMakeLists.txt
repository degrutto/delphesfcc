cmake_minimum_required(VERSION 2.8)
project(delphesfcc CXX)

# Set up C++11
set(CPP11FLAGS "-std=c++11")
if (${APPLE})
    set(CPP11FLAGS "-std=c++11\ -stdlib=libc++")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -DDROP_CGAL ${CPP11FLAGS} -Wall -Wextra -Wpedantic -Wno-unused-variable -Wno-unused-parameter")

# Make sure we find the Find*.cmake functions distributed with this package


set(DelphesExternals_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/../Delphes-3.2.0/external)




set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_PREFIX_PATH $ENV{ALBERS} $ENV{DELPHES_DIR} $ENV{FCCEDM})
find_package(alberscore REQUIRED)
find_package(fccedm REQUIRED)
find_package(ROOT REQUIRED)
find_package(delphes REQUIRED)



#set(DelphesExternals_INCLUDE_DIR ${DELPHES_DIR}/external)


# set up include-directories
include_directories(
  "${ROOT_INCLUDE_DIR}"
  "${ALBERSCORE_INCLUDE_DIRS}"
  "${FCCEDM_INCLUDE_DIRS}"
  "${DELPHES_INCLUDE_DIRS}"
  "${DelphesExternals_INCLUDE_DIR}"
  )  

# Add sub-directories
add_subdirectory(example)


