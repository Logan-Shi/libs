# Install script for directory: /home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/unsupported/Eigen

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/unsupported/Eigen" TYPE FILE FILES
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/unsupported/Eigen/AdolcForward"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/unsupported/Eigen/AlignedVector3"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/unsupported/Eigen/ArpackSupport"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/unsupported/Eigen/AutoDiff"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/unsupported/Eigen/BVH"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/unsupported/Eigen/FFT"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/unsupported/Eigen/IterativeSolvers"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/unsupported/Eigen/KroneckerProduct"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/unsupported/Eigen/LevenbergMarquardt"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/unsupported/Eigen/MatrixFunctions"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/unsupported/Eigen/MoreVectorization"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/unsupported/Eigen/MPRealSupport"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/unsupported/Eigen/NonLinearOptimization"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/unsupported/Eigen/NumericalDiff"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/unsupported/Eigen/OpenGLSupport"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/unsupported/Eigen/Polynomials"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/unsupported/Eigen/Skyline"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/unsupported/Eigen/SparseExtra"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/unsupported/Eigen/Splines"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/build_dir/unsupported/Eigen/src/cmake_install.cmake")

endif()

