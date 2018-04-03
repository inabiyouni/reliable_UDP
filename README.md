This program simulate a simlpe version of TCP protocol by using UDP connections.

For running the programs follow these steps:
For compiling you can easily type make -f Makefile to compile both files to gether.

Server:

After compiling use this command type:
./complied_name port_number advertised_window(byte)
Example:
./a_s 15010 150000


Client:

hostname -I   
After compiling use this command type:
./complied_name  server_IP port_number file_name packet_loss_rate(%) latency(microSec) advertised_window(byte)
Example:
./a_c 149.160.202.17 15010 text_1MB.txt 1 1 150000



