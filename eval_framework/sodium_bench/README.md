Benchamrk for simple sodium authentication, encryption, decryption.

To test on other machines simple clone project:
```$ git clone https://github.com/hbdgr/sodium_bench```

build (google benchmark is dependence. https://github.com/google/benchmark [should be installed on machine]):
```$ cmake  -DCMAKE_BUILD_TYPE=Release -DBENCHMARK_ENABLE_LTO=true && make```

and run script:
```$ ./result_analysis.sh --run-bench --analyze  --gen-plot-data```

to only console benchmaks (without anlysis and gnuplot charts)
```$ ./sodium_bench```

All data from benchmark should be placed in "data_files"  directory.
Gnuplot charts will be placed in "output" directory.


Project is easy to extend to benchmark other functionalities by writing next google_benchmark style functions in "src" or
by choosing the appropriate predefined functions, only uncomment them in src/main.cpp.

Result_analysis.sh script works only half-generic, it takes all benchmark functions started by "BM"
```
$ ./result_analysis.sh --help
usage: sodium_bench options
Program run benchmarks and analyze it

OPTIONS:
    -r --run-bench           run sodium_bench binary with google benchmarks and save results to results.txt
    -a --analyze             analyze  benchmark results from sodium_bench
    -g --gen-plot-data       generate data to plot in ./data_files dir
    -h --help                show this help
```

Gnuplot scripts are not generic and should be written (or copied) for all new functions.

Example results, tested on (2 X 2500 MHz CPU s)[AMD A4-4300M]:

![alt tag](https://raw.githubusercontent.com/hbdgr/sodium_bench/master/results/AMD_A4-4300M/3d_weld_encryption_buf_size.png)
![alt tag](https://raw.githubusercontent.com/hbdgr/sodium_bench/master/results/AMD_A4-4300M/3d_weld_encryption_buf_num.png)
![alt tag](https://raw.githubusercontent.com/hbdgr/sodium_bench/master/examples/Auth_Encypt.png)
![alt tag](https://raw.githubusercontent.com/hbdgr/sodium_bench/master/examples/Auth_Encypt_Decrypt.png)
