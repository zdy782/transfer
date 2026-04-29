#!/usr/bin/env python3

import os

GNUPLOT_DATADIR="data_files"
OUTPUT_DIR="output"

def get_unique_functions(list_of_files):
    output = set()
    for file in list_of_files:
        function_name=file.partition("-")[0]
        output.add(function_name)
    return output

def get_unique_param(list_of_files, function):
    output = set()
    for file in list_of_files:
        if function + "-" in file:
            param_name = file.replace(".data", "").rsplit("-",maxsplit=1)[-1]
            output.add(param_name)
    return output

def get_available_threads(list_of_files, fun_name, parameter):
    output = set()
    for file in list_of_files:
        if fun_name in file:
            threads = file.replace(".data", "").split("-",maxsplit=2)[1]
            print(fun_name + " " + " threads:" + threads)
            if threads == "":
                return [1] # one thread case
            output.add(int(threads))
    return sorted(output)

def write_gnuplot_header(gnuplot_file, fun_name, plot_num):
    gnuplot_file.write("\n")
    gnuplot_file.write("set terminal pngcairo size 1200,2400 enhanced font 'Verdana,10'\n")
    gnuplot_file.write("set output '" + OUTPUT_DIR + "/" + fun_name + ".png'\n")
    gnuplot_file.write("set multiplot layout " + str(plot_num) + ", 1 title '" + fun_name + "' font ',16'\n")
    gnuplot_file.write("set grid x y")

def write_gnuplot_chart_title(gnuplot_file, title):
    gnuplot_file.write("\n")
    gnuplot_file.write("set title '" + title + "'\n")
    gnuplot_file.write("set title  font ',12' norotate\n")

def write_gnuplot_x_axis(gnuplot_file, title, logscale=0):
    gnuplot_file.write("\n")
    gnuplot_file.write("set xlabel '" + title + "'\n")
    if logscale:
        gnuplot_file.write("set logscale x " + str(logscale) + "\n")

def write_gnuplot_y_axis(gnuplot_file, title, logscale=0):
    gnuplot_file.write("\n")
    gnuplot_file.write("set ylabel '" + title + "'\n")
    if logscale:
        gnuplot_file.write("set logscale y " + str(logscale) + "\n")

def write_gnuplot_xdata_axis(gnuplot_file):
    write_gnuplot_x_axis(gnuplot_file, "data size", 2)

    gnuplot_file.write("\n")
    # specify tics according to data size
    gnuplot_file.write("set xtics ('1B' 1,'2B' 2,'4B' 4,'8B' 8,'16B' 16,'32B' 32,'64B' 64,'128B' 128,'256B' 256,'512B' 512,"
                        + "'1kB' 1024,'2kB' 2048,'4kB' 4096,'8kB' 8192,'16kB' 16384,'32kB' 32768,'64kB' 65536,'128kB' 131072,"
                        + "'256kB' 262144,'512kB' 524288,'1MB' 1048576,'2MB' 2097152,'4MB' 4194304,'8MB' 8388608,"
                        + "'16MB' 16777216,'32MB' 33554432 )\n")

def write_gnuplot_plot_data(gnuplot_file, fun_name, threads, param):
    gnuplot_file.write("\n")
    gnuplot_file.write("plot ")
    for thread in threads:
        gnuplot_file.write("'" + GNUPLOT_DATADIR + "/" + fun_name + "-" + str(thread) + "-" + param + ".data' with lines title 'threads-chunks: " + str(thread) +  "' axes x1y1,\\\n")


if __name__ == "__main__":
    file_list = os.listdir(GNUPLOT_DATADIR)
    functions = get_unique_functions(file_list)

    for fun in functions:
        fun_param = get_unique_param(file_list, fun)
        fun_pretty_name = fun.replace("BM_","").replace("_","-").capitalize()

        gnuplot_file_name = (fun_pretty_name.lower() + ".gnuplot")
        print("Generating: " + gnuplot_file_name)
        gnuplot_file = open(gnuplot_file_name, "w")

        write_gnuplot_header(gnuplot_file, fun_pretty_name, len(fun_param))
        write_gnuplot_xdata_axis(gnuplot_file)

        for param in sorted(fun_param):
            threads_array = get_available_threads(file_list, fun, param)

            if param == "Mbps":
                print("Generating Mbps chart for " + fun)
                write_gnuplot_chart_title(gnuplot_file, "Speed in Mbps")
                write_gnuplot_y_axis(gnuplot_file, "speed rate [Mbps]")
                write_gnuplot_plot_data(gnuplot_file, fun, threads_array, param)
            elif param == "latency":
                print("Generating latency chart for " + fun)
                write_gnuplot_chart_title(gnuplot_file, "Latency in nano seconds")
                write_gnuplot_y_axis(gnuplot_file, "latency [ns]", 10)
                write_gnuplot_plot_data(gnuplot_file, fun, threads_array, param)
            elif param == "cpu_time":
                print("Generating cpu_time chart for " + fun)
                write_gnuplot_chart_title(gnuplot_file, "CPU time in nano seconds")
                write_gnuplot_y_axis(gnuplot_file, "cpu time [ns]", 10)
                write_gnuplot_plot_data(gnuplot_file, fun, threads_array, param)
            else:
                print("Parameter not recognized fun[" + fun  + "], param[" + param + "]" )

        gnuplot_file.close()
