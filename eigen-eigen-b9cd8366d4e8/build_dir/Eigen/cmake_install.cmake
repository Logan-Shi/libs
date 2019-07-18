# Install script for directory: /home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/Eigen" TYPE FILE FILES
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/PardisoSupport"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/Jacobi"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/Array"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/Cholesky"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/UmfPackSupport"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/Dense"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/StdList"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/Eigen"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/StdVector"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/StdDeque"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/Sparse"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/MetisSupport"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/SparseCholesky"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/SparseCore"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/SPQRSupport"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/IterativeLinearSolvers"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/SparseLU"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/QR"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/SuperLUSupport"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/Householder"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/QtAlignedMalloc"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/PaStiXSupport"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/SVD"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/CholmodSupport"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/SparseQR"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/Eigenvalues"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/Geometry"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/LeastSquares"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/Core"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/LU"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/OrderingMethods"
    "/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/Eigen/Eigen2Support"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/loganshi/Documents/eigen-eigen-b9cd8366d4e8/build_dir/Eigen/src/cmake_install.cmake")

endif()

