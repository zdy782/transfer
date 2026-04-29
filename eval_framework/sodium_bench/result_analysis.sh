#!/usr/bin/env bash

readonly WELD_ENCRYPT_3D=1

readonly FILENAME="results.txt"
readonly PROGNAME="sodium_bench"
readonly GNUPLOT_DATA_DIR="data_files"
readonly OUTPUT="output"

function run_benchmark() {
	local program_name=$1
	local results_filename=$2
	./${program_name} | tee "${results_filename}"
}

# function arguments:
# $1 time in nanoseconds
function to_seconds() {
	local nanoseconds=$1
	local nanosecond_multiplier=1000000000

	#seconds
	local seconds=`echo "scale=10; ${nanoseconds}/${nanosecond_multiplier}" | bc -l`
	printf "${seconds}"
}

# function arguments:
# $1 size in bytes, kilobytes or megabytes
function to_bytes() {
	local human_bytes=$1
	local bytes_operation=`echo "$human_bytes" | sed -e 's/k/*1024/g' | sed -e 's/M/*1024*1024/g'`

	#bytes
	local bytes=`echo "${bytes_operation}" | bc -l`
	printf "${bytes}"
}

# function arguments:
# $1 size in bytes, kilobytes or megabytes
# $2 time in nanoseconds
function calc_Mbps() {
	local bytes=`to_bytes "$1"`
	local seconds=`to_seconds "$2"`
	local Megabit_multiplier=`echo "1024*1024/8" | bc -l`

	local Mbps=`echo "${bytes} ${seconds} ${Megabit_multiplier}" | awk  '{ print ($1/$3)/$2 }' `
	printf "${Mbps}"
}

function gen_data_Mbps() {
	printf "Start reading / Analyze\n"

	while read line; do
		# results that we analyze should have name with "BM"
		if [[ $line != *"BM"* ]]; then
			continue
		fi

		local size=`echo $line | awk -F'/' '{ print $2 }' | awk '{ print $1 }'`
		local time=`echo $line | awk '{ print $2 }'`

		local Mbps=`calc_Mbps "${size}" "${time}"`

		printf "${line} | "${Mbps}" Mbps\n"

	done < "${FILENAME}"
}
function is_dir() {
	local dir="$1"
	[[ -d "$dir" ]]
}

function clean_gnuplot_data() {
	is_dir ${GNUPLOT_DATA_DIR} \
		&& echo "deleting gnuplot data directory: ${GNUPLOT_DATA_DIR}" \
		&& rm -rf ${GNUPLOT_DATA_DIR}

	is_dir ${OUTPUT} \
		&& echo "deleting gnuplot data directory: ${OUTPUT}" \
		&& rm -rf ${OUTPUT}

	mkdir ${GNUPLOT_DATA_DIR}
	mkdir ${OUTPUT}
}

function gen_charts() {

	#declare -a gnuplot_files=`find -type f -name "*.gnuplot"`
	find -type f -name "*.gnuplot" \
		| while read file; do
			echo "ploting: ${file}"
			gnuplot ${file}
		done
}

function gen_gnuplot_files() {
	if [[ $WELD_ENCRYPT_3D == 1 ]]
	then
		# generate 3d charts manualy
		gnuplot 3d_weld_encryption_buf_num.gnuplot
		gnuplot 3d_weld_encryption_buf_size.gnuplot

		# and exit because auto gen charts have no sense for this case
		exit 0
	fi
	#python implementation
	./generate_gnuplot_files.py
}

function gen_data_Mbps_gunpfiles() {
	printf "Start generating files in ${GNUPLOT_DATA_DIR}\n"

	while read line; do
		# results that we analyze should have name with "BM"
		if [[ $line != *"BM"* ]]; then
			continue
		fi

		local fun_name=`echo $line | awk -F'/' '{ print $1 }'`
		local threads=`echo $line | awk -F"threads:" '{ print $2  }' | awk '{ print $1 }'`
		if [[ -z $threads ]]
		then
			threads=1
		fi

		local size=`echo $line | awk -F'/' '{ print $2 }' | awk '{ print $1 }'`
		local second_arg=`echo $line | awk -F'/' '{ print $3 }' | awk '{ print $1 }'`

		local time=`echo $line | awk '{ print $2 }'`
		local cpu_time=`echo $line | awk '{ print $4 }'`

		local Mbps=`calc_Mbps "${size}" "${time}"`

		printf "Generating data for: fun_name [${fun_name}], threads[${threads}],"
			printf "size [${size}], second_arg [${second_arg}],"
			printf "time [${time}], cpu_time [${cpu_time}]\n"

		if [[ -n second_arg ]]
		then
			# division to get spec buffer size in weld_encryption
			average_size=$(echo "scale=4; `to_bytes ${size}` / `to_bytes ${second_arg}`" | bc)
			printf "`to_bytes ${size}` `to_bytes ${second_arg}`   ${Mbps}\n" >> "${GNUPLOT_DATA_DIR}/${fun_name}-${threads}-buf_num-Mbps.data"
			printf "`to_bytes ${size}` $average_size  ${Mbps}\n" >> "${GNUPLOT_DATA_DIR}/${fun_name}-${threads}-buf_size-Mbps.data"

		else
			printf "`to_bytes ${size}` ${time}\n" >> "${GNUPLOT_DATA_DIR}/${fun_name}-${threads}-latency.data"
			printf "`to_bytes ${size}` ${Mbps}\n" >> "${GNUPLOT_DATA_DIR}/${fun_name}-${threads}-Mbps.data"
			printf "`to_bytes ${size}` ${cpu_time}\n" >> "${GNUPLOT_DATA_DIR}/${fun_name}-${threads}-cpu_time.data"
		fi

	done < "${FILENAME}"
}

usage() {
cat <<- EOF
	usage: $PROGNAME options

	Program run benchmarks and analyze it

	OPTIONS:
	    -r --run-bench           run ${PROGNAME} binary with google benchmarks and save results to ${FILENAME}
	    -a --analyze             analyze  benchmark results from ${PROGNAME}
	    -g --gen-plot-data       generate data to ./${GNUPLOT_DATA_DIR} and plot charts using gnuplot to ./${OUTPUT}

	    -h --help                show this help
EOF
}

cmdline() {
	# got this idea from here:
	# http://kirk.webfinish.com/2009/10/bash-shell-script-to-use-getopts-with-gnu-style-long-positional-parameters/
	local arg=$@
	for arg; do
		local delim=""
		case "$arg" in
			#translate --gnu-long-options to -g (short options)
			--run-bench)            args="${args}-r ";;
			--analyze)              args="${args}-a ";;
			--gen-plot-data)        args="${args}-g ";;
			--help)                 args="${args}-h ";;
			#pass through anything else
			*) [[ "${arg:0:1}" == "-" ]] || delim="\""
				args="${args}${delim}${arg}${delim} ";;
		esac
	done

	#Reset the positional parameters to the short options
	eval set -- $args

	while getopts "hrag" OPTION; do
		case $OPTION in
		r)
			set -o errexit
			set -o nounset
			run_benchmark "${PROGNAME}" "${FILENAME}"
			;;
		a)
			set -o errexit
			set -o nounset
			gen_data_Mbps
			;;
		g)
			set -o errexit
			set -o nounset
			clean_gnuplot_data
			gen_data_Mbps_gunpfiles
			gen_gnuplot_files
			gen_charts
			;;
		h)
			usage
			exit 0
			;;
		esac
	done

	return 0
}

main() {
	cmdline $@
}
main $@
