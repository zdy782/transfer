# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/data/chenxuqiang/eval_framework/sleef/submodules/tlfloat"
  "/data/chenxuqiang/eval_framework/sleef/build/ext_tlfloat-prefix/src/ext_tlfloat-build"
  "/data/chenxuqiang/eval_framework/sleef/build/ext_tlfloat-prefix"
  "/data/chenxuqiang/eval_framework/sleef/build/ext_tlfloat-prefix/tmp"
  "/data/chenxuqiang/eval_framework/sleef/build/ext_tlfloat-prefix/src/ext_tlfloat-stamp"
  "/data/chenxuqiang/eval_framework/sleef/build/ext_tlfloat-prefix/src"
  "/data/chenxuqiang/eval_framework/sleef/build/ext_tlfloat-prefix/src/ext_tlfloat-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/data/chenxuqiang/eval_framework/sleef/build/ext_tlfloat-prefix/src/ext_tlfloat-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/data/chenxuqiang/eval_framework/sleef/build/ext_tlfloat-prefix/src/ext_tlfloat-stamp${cfgdir}") # cfgdir has leading slash
endif()
