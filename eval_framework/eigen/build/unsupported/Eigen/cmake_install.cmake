# Install script for directory: /data/chenxuqiang/eval_framework/eigen/unsupported/Eigen

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
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/unsupported/Eigen" TYPE FILE FILES
    "/data/chenxuqiang/eval_framework/eigen/unsupported/Eigen/AdolcForward"
    "/data/chenxuqiang/eval_framework/eigen/unsupported/Eigen/AlignedVector3"
    "/data/chenxuqiang/eval_framework/eigen/unsupported/Eigen/ArpackSupport"
    "/data/chenxuqiang/eval_framework/eigen/unsupported/Eigen/AutoDiff"
    "/data/chenxuqiang/eval_framework/eigen/unsupported/Eigen/BVH"
    "/data/chenxuqiang/eval_framework/eigen/unsupported/Eigen/EulerAngles"
    "/data/chenxuqiang/eval_framework/eigen/unsupported/Eigen/FFT"
    "/data/chenxuqiang/eval_framework/eigen/unsupported/Eigen/IterativeSolvers"
    "/data/chenxuqiang/eval_framework/eigen/unsupported/Eigen/KroneckerProduct"
    "/data/chenxuqiang/eval_framework/eigen/unsupported/Eigen/LevenbergMarquardt"
    "/data/chenxuqiang/eval_framework/eigen/unsupported/Eigen/MatrixFunctions"
    "/data/chenxuqiang/eval_framework/eigen/unsupported/Eigen/MPRealSupport"
    "/data/chenxuqiang/eval_framework/eigen/unsupported/Eigen/NNLS"
    "/data/chenxuqiang/eval_framework/eigen/unsupported/Eigen/NonLinearOptimization"
    "/data/chenxuqiang/eval_framework/eigen/unsupported/Eigen/NumericalDiff"
    "/data/chenxuqiang/eval_framework/eigen/unsupported/Eigen/OpenGLSupport"
    "/data/chenxuqiang/eval_framework/eigen/unsupported/Eigen/Polynomials"
    "/data/chenxuqiang/eval_framework/eigen/unsupported/Eigen/SparseExtra"
    "/data/chenxuqiang/eval_framework/eigen/unsupported/Eigen/SpecialFunctions"
    "/data/chenxuqiang/eval_framework/eigen/unsupported/Eigen/Splines"
    "/data/chenxuqiang/eval_framework/eigen/unsupported/Eigen/Tensor"
    "/data/chenxuqiang/eval_framework/eigen/unsupported/Eigen/TensorSymmetry"
    "/data/chenxuqiang/eval_framework/eigen/unsupported/Eigen/ThreadPool"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/unsupported/Eigen" TYPE DIRECTORY FILES "/data/chenxuqiang/eval_framework/eigen/unsupported/Eigen/src" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/data/chenxuqiang/eval_framework/eigen/build/unsupported/Eigen/CXX11/cmake_install.cmake")

endif()

