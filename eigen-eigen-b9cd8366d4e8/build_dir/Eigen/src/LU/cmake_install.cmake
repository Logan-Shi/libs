# Install script for directory: /home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/LU

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Devel")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/Eigen/src/LU" TYPE FILE FILES
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/LU/PartialPivLU_MKL.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/LU/FullPivLU.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/LU/Determinant.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/LU/PartialPivLU.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/LU/Inverse.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/build_dir/Eigen/src/LU/arch/cmake_install.cmake")

endif()

