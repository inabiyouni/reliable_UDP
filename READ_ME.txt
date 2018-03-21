For running the programs follow these steps:

For UDP:

Server:
g++ -pthread -std=c++11 UDP_Relb_server.cpp -o a_s

After compiling use this command type:
./complied_name port_number advertised_window(byte)
Example:
./a_s 15010 150000


Client:
 g++ -std=c++11 main.cpp -o a_c
hostname -I   
After compiling use this command type:
./complied_name  server_IP port_number file_name packet_loss_rate(%) latency(microSec) advertised_window(byte)
Example:
./a_c 149.160.202.17 15010 text_1MB.txt 1 1 150000
You can always use non-persistent connection type since UDP is connectionless!
NOTE: IF data is lost in the network, Client will not terminate the loop and will not give any number as output. 
So you may need to run it couple of times to make sure no data is lost.

