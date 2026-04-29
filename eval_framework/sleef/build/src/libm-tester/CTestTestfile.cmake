# CMake generated Testfile for 
# Source directory: /data/chenxuqiang/eval_framework/sleef/src/libm-tester
# Build directory: /data/chenxuqiang/eval_framework/sleef/build/src/libm-tester
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(tester4advsimd "/data/chenxuqiang/eval_framework/sleef/build/bin/tester4advsimd")
set_tests_properties(tester4advsimd PROPERTIES  COST "1.0" _BACKTRACE_TRIPLES "/data/chenxuqiang/eval_framework/sleef/src/libm-tester/CMakeLists.txt;79;add_test;/data/chenxuqiang/eval_framework/sleef/src/libm-tester/CMakeLists.txt;213;add_test_with_emu;/data/chenxuqiang/eval_framework/sleef/src/libm-tester/CMakeLists.txt;308;test_extension;/data/chenxuqiang/eval_framework/sleef/src/libm-tester/CMakeLists.txt;0;")
add_test(tester3advsimd "/data/chenxuqiang/eval_framework/sleef/build/bin/tester3advsimd" "/data/chenxuqiang/eval_framework/sleef/src/libm-tester/hash_finz.txt")
set_tests_properties(tester3advsimd PROPERTIES  COST "0.5" _BACKTRACE_TRIPLES "/data/chenxuqiang/eval_framework/sleef/src/libm-tester/CMakeLists.txt;297;add_test;/data/chenxuqiang/eval_framework/sleef/src/libm-tester/CMakeLists.txt;308;test_extension;/data/chenxuqiang/eval_framework/sleef/src/libm-tester/CMakeLists.txt;0;")
add_test(tester4purec_scalar "/data/chenxuqiang/eval_framework/sleef/build/bin/tester4purec_scalar")
set_tests_properties(tester4purec_scalar PROPERTIES  COST "3" _BACKTRACE_TRIPLES "/data/chenxuqiang/eval_framework/sleef/src/libm-tester/CMakeLists.txt;79;add_test;/data/chenxuqiang/eval_framework/sleef/src/libm-tester/CMakeLists.txt;211;add_test_with_emu;/data/chenxuqiang/eval_framework/sleef/src/libm-tester/CMakeLists.txt;308;test_extension;/data/chenxuqiang/eval_framework/sleef/src/libm-tester/CMakeLists.txt;0;")
add_test(tester3purec_scalar "/data/chenxuqiang/eval_framework/sleef/build/bin/tester3purec_scalar" "/data/chenxuqiang/eval_framework/sleef/src/libm-tester/hash_finz.txt")
set_tests_properties(tester3purec_scalar PROPERTIES  COST "3" _BACKTRACE_TRIPLES "/data/chenxuqiang/eval_framework/sleef/src/libm-tester/CMakeLists.txt;297;add_test;/data/chenxuqiang/eval_framework/sleef/src/libm-tester/CMakeLists.txt;308;test_extension;/data/chenxuqiang/eval_framework/sleef/src/libm-tester/CMakeLists.txt;0;")
add_test(tester4purecfma_scalar "/data/chenxuqiang/eval_framework/sleef/build/bin/tester4purecfma_scalar")
set_tests_properties(tester4purecfma_scalar PROPERTIES  COST "1.0" _BACKTRACE_TRIPLES "/data/chenxuqiang/eval_framework/sleef/src/libm-tester/CMakeLists.txt;79;add_test;/data/chenxuqiang/eval_framework/sleef/src/libm-tester/CMakeLists.txt;213;add_test_with_emu;/data/chenxuqiang/eval_framework/sleef/src/libm-tester/CMakeLists.txt;308;test_extension;/data/chenxuqiang/eval_framework/sleef/src/libm-tester/CMakeLists.txt;0;")
add_test(tester3purecfma_scalar "/data/chenxuqiang/eval_framework/sleef/build/bin/tester3purecfma_scalar" "/data/chenxuqiang/eval_framework/sleef/src/libm-tester/hash_finz.txt")
set_tests_properties(tester3purecfma_scalar PROPERTIES  COST "0.5" _BACKTRACE_TRIPLES "/data/chenxuqiang/eval_framework/sleef/src/libm-tester/CMakeLists.txt;297;add_test;/data/chenxuqiang/eval_framework/sleef/src/libm-tester/CMakeLists.txt;308;test_extension;/data/chenxuqiang/eval_framework/sleef/src/libm-tester/CMakeLists.txt;0;")
add_test(tester4dspscalar "/data/chenxuqiang/eval_framework/sleef/build/bin/tester4dspscalar")
set_tests_properties(tester4dspscalar PROPERTIES  COST "3.0" _BACKTRACE_TRIPLES "/data/chenxuqiang/eval_framework/sleef/src/libm-tester/CMakeLists.txt;79;add_test;/data/chenxuqiang/eval_framework/sleef/src/libm-tester/CMakeLists.txt;321;add_test_with_emu;/data/chenxuqiang/eval_framework/sleef/src/libm-tester/CMakeLists.txt;0;")
