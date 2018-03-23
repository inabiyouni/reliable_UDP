all: server client
server: UDP_Relb_server.cpp
	g++ -std=c++11 UDP_Relb_server.cpp -o UDP_Relb_server -lpthread
client: UDP_Relb_client.cpp
	g++ -std=c++11 UDP_Relb_client.cpp -o UDP_Relb_client
clean:
	rm -rf UDP_Relb_server UDP_Relb_client
