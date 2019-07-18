# Install script for directory: /home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/Eigen/src/SparseCore" TYPE FILE FILES
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/ConservativeSparseSparseProduct.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/SparseDot.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/SparseUtil.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/SparseCwiseBinaryOp.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/CompressedStorage.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/SparseSparseProductWithPruning.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/TriangularSolver.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/SparseMatrix.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/SparseRedux.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/SparseSelfAdjointView.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/SparseFuzzy.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/SparseCwiseUnaryOp.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/SparseView.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/SparseTranspose.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/SparseDiagonalProduct.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/SparseMatrixBase.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/SparseDenseProduct.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/MappedSparseMatrix.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/AmbiVector.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/SparseBlock.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/SparseProduct.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/SparseTriangularView.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/SparsePermutation.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/SparseColEtree.h"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/src/SparseCore/SparseVector.h"
    )
endif()

