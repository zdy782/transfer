set terminal png size 1920,1600 enhanced font 'Verdana,8'
set output 'results/3d_weld_encryption_buf_size.png'
set title "Libsodium authenticated encryption with symmetric keys\n'Encryption: XSalsa20 stream cipher, Authentication: Poly1305 MAC' " font ", 26"

set tics front
set key outside bottom horizontal
set hidden3d

set dgrid3d 50,50 qnorm 2

set isosamples 100
set pm3d at s

set contour base
set cntrparam levels auto 10

set palette defined (0 "royalblue",1 "turquoise",3 "yellow",5 "red")

#unset key
#unset surface
#set surface

set logscale x 2
set logscale y 2

set zlabel offset -6,5

set xlabel 'Total buffers size' font ", 20"
set ylabel 'Average size for one buffor' font ", 20"
set zlabel 'speed rate [Mbps]' rotate by 90 font ", 20"

set xtics ('1B' 1,'2B' 2,'4B' 4,'8B' 8,'16B' 16,'32B' 32,'64B' 64,'128B' 128,'256B' 256,'512B' 512,'1kB' 1024,'2kB' 2048,'4kB' 4096,'8kB' 8192,'16kB' 16384,'32kB' 32768,'64kB' 65536,'128kB' 131072,'256kB' 262144,'512kB' 524288,'1MB' 1048576,'2MB' 2097152,'4MB' 4194304,'8MB' 8388608,'16MB' 16777216,'32MB' 33554432,'64MB' 67108864,'128MB' 134217728,'256MB' 268435456, '512MB' 536870912, '1GB' 1073741824 )

set ytics ('1B' 1,'2B' 2,'4B' 4,'8B' 8,'16B' 16,'32B' 32,'64B' 64,'128B' 128,'256B' 256,'512B' 512,'1kB' 1024,'2kB' 2048,'4kB' 4096,'8kB' 8192,'16kB' 16384,'32kB' 32768,'64kB' 65536,'128kB' 131072,'256kB' 262144,'512kB' 524288,'1MB' 1048576,'2MB' 2097152,'4MB' 4194304,'8MB' 8388608,'16MB' 16777216,'32MB' 33554432,'64MB' 67108864,'128MB' 134217728,'256MB' 268435456, '512MB' 536870912, '1GB' 1073741824 )

splot 'data_files/BM_weld_encrypt-1-buf_size-Mbps.data'

pause -1 "Hit return to continue"
