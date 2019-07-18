# Install script for directory: /home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/Core/products

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/Eigen/src/Core/products" TYPE FILE FILES
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/Core/products/TriangularMatrixMatrix.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/Core/products/GeneralBlockPanelKernel.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/Core/products/SelfadjointMatrixMatrix_MKL.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/Core/products/SelfadjointRank2Update.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/Core/products/TriangularSolverVector.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/Core/products/SelfadjointMatrixVector_MKL.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/Core/products/CoeffBasedProduct.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/Core/products/GeneralMatrixMatrixTriangular_MKL.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/Core/products/GeneralMatrixVector.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/Core/products/GeneralMatrixMatrix_MKL.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/Core/products/GeneralMatrixMatrix.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/Core/products/GeneralMatrixMatrixTriangular.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/Core/products/TriangularSolverMatrix.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/Core/products/Parallelizer.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/Core/products/TriangularMatrixVector.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/Core/products/TriangularMatrixVector_MKL.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/Core/products/GeneralMatrixVector_MKL.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/Core/products/TriangularMatrixMatrix_MKL.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/Core/products/TriangularSolverMatrix_MKL.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/Core/products/SelfadjointProduct.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/Core/products/SelfadjointMatrixMatrix.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/Core/products/SelfadjointMatrixVector.h"
    )
endif()

