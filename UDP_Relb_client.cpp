#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <regex>
#include <netdb.h>
//#include <ctime>
//#include <Windows.h>
#include <chrono>
#include <thread>
//#include <ctime>
#include <sys/time.h>
#include <cstdlib>

using namespace std;

struct header_struct{
    int32_t squnc_number;
    int32_t ack_number;
    bool fin_flag;
    bool ack_flag;
    int32_t a = 0;
    bool b= true;
    header_struct() : squnc_number(a), ack_number(a), ack_flag(b), fin_flag(b) {}
    header_struct(int32_t a, int32_t b, bool c, bool d) : squnc_number(a), ack_number(b), ack_flag(c), fin_flag(d) {}
};
void add_header(header_struct &sendg_header, stringstream& stream){
    char header[23];
    sprintf (header, "%10d%10d%1d%1d", sendg_header.squnc_number, sendg_header.ack_number,
             sendg_header.ack_flag, sendg_header.fin_flag);
    stream << header;
}
void extract_Header(char* buffer, header_struct& recvg_header, int& buffer_size, bool& acptHeader){
    int numbers_size = 10;
    int flag_size = 1;
    int headerSize = 2 * numbers_size + 2 * flag_size;
    char temp[numbers_size];
    memmove (temp, buffer , numbers_size);
    try {
        recvg_header.squnc_number = stoi(temp);
    }
    catch(...) {
        acptHeader = false;
    }
    memmove (temp, buffer + numbers_size , numbers_size);
    try {
        recvg_header.ack_number = stoi(temp);
    }
    catch(...) {
        acptHeader = false;
    }
    memmove (temp, buffer + 2* numbers_size , flag_size);
    try {
        int test = stoi(temp);
    }
    catch(...) {
        acptHeader = false;
    }
    recvg_header.ack_flag = (temp[0] == '1') ? true:false;
    memmove (temp, buffer + 2* numbers_size + flag_size , flag_size);
    try {
        int test = stoi(temp);
    }
    catch(...) {
        acptHeader = false;
    }
    recvg_header.fin_flag = (temp[0] == '1') ? true:false;
    memmove (buffer, buffer + headerSize , buffer_size);// = buffer(m.position(),str.length());
}
void add_Date(stringstream *stream){
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[25];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, 25, "%a, %d %h %Y %H:%M:%S", timeinfo);
    char crrnt_Time[35];
    sprintf(crrnt_Time, "Date: %s GMT\n\n", buffer);
    *stream << crrnt_Time;
}

int main(int arg_Num, char *args[]) {//){//
    fflush(stdout);
    int clientSocket, rcvd_Data, server_port;
    int bufsize = atoi(args[6]);
    //int bufsize = 150000;
    char buffer[bufsize];
    const char *server_host, *filename;// = malloc(bufsize);
    struct sockaddr_in srvr_Addr, clnt_Addr;
    struct hostent *hst_Name;
    socklen_t clnt_Len = sizeof(clnt_Addr);
    socklen_t srvr_Len = sizeof(srvr_Addr);

    server_host = args[1];//"149.160.210.223";//192.168.2.223";//149.160.200.10";//
    server_port = atoi(args[2]);//15010;//
    filename = args[3];//"text.txt";//HelloWorld.html";//
/*
    server_host = "192.168.2.223";//149.160.210.223";//192.168.2.223";//149.160.200.10";//
    server_port = 15010;//
    //connection_type = "non-persistent";//
    filename = "text_1MB.txt";//text_1MB.txt";//HelloWorld.html";//
*/
    /*
    if (strcmp(connection_type,"persistent") == 0) connection_order = "keep-alive";
    else if (strcmp(connection_type, "non-persistent") == 0) connection_order = "close";
    else{
        perror("Entered connection type is no valid!");
        exit(1);
    }

    string zeroTo255 = "(\\d{1,2}|(0|1)\\d{2}|2[0-4]\\d|25[0-5])";
    string compPattern = zeroTo255 + "\\." + zeroTo255 + "\\." + zeroTo255 + "\\." + zeroTo255;
    regex pattern(compPattern);//"(\\d{1,3})\.(\\d{1,3})");
*/
    struct timespec start;//,temp;
    clock_gettime(CLOCK_REALTIME, &start);
    long nsecs = start.tv_nsec;
    long secs = start.tv_sec;
    clock_t startTime = clock();
    //struct timespec st, end;
    //clock_gettime(CLOCK_MONOTONIC_RAW, &st);
    if ((clientSocket = socket(AF_INET, SOCK_DGRAM, 0)) > 0){
        printf("The socket was created\n");
    }

    srvr_Addr.sin_family = AF_INET;
    srvr_Addr.sin_addr.s_addr =inet_addr(server_host);
    srvr_Addr.sin_port = htons(server_port);

    printf("The Server is connected...\n");
    stringstream stream;
    string sending_Text;
    int header_size = 22;

    struct header_struct sendg_header(0, 0, false, false);
    add_header(sendg_header, stream);

    char message[60];
    sprintf(message, "Filename:%s\n", filename);
    stream << message;
    /*sprintf(message, "Host: %s\n", server_host);
    stream << message;
    sprintf(message, "Connection:%s\n", connection_order);
    stream << message;
    add_Date(&stream);
*/
    sending_Text = stream.str();
    //printf(sending_Text.c_str());
    sendto(clientSocket, sending_Text.c_str(), sending_Text.length(),0, (struct sockaddr *)&srvr_Addr, srvr_Len);

    bzero(buffer,bufsize);
    rcvd_Data = recvfrom(clientSocket, buffer, bufsize, 0, (struct sockaddr *)&srvr_Addr, &srvr_Len);
    struct header_struct recvg_header;
    bool is_header = true;
    extract_Header(buffer, recvg_header, bufsize, is_header);

    //regex okay_Pttrn("200 OK");
    //regex notFnd_Pttrn("404 Not Found");
    //regex pat_connct_len("Content-length:.+\n");
    //smatch m;
    if (rcvd_Data < 0) perror("ERROR reading from socket");
    //printf("Here is the server message:\n %s\n", buffer);
    string str(buffer);
    string connection_len;
    if (str.substr(0, 6) == "200 OK") {
        //regex dblReturn_Pttrn("\\n\\n");
        //if (regex_search(str, m, dblReturn_Pttrn))
        //    memmove (buffer, buffer + m.position(), strlen (buffer));// = buffer(m.position(),str.length());
        /*
        if (regex_search(str, m, pat_connct_len)) {
            connection_len = m.str();
            connection_len = connection_len.substr(16);
            connection_len.erase(remove(connection_len.begin(), connection_len.end(), '\n'), connection_len.end());
        } else connection_len = "1000";
        //printf("Here is the received file:\n");
        */
        int LastByteAcked = 0;
        int rand_drop = 0;
         do{
             rand_drop = rand() % 100 + 1;
             rcvd_Data = recvfrom(clientSocket, buffer , bufsize, 0, (struct sockaddr *)&srvr_Addr, &srvr_Len);
             std::this_thread::sleep_for(std::chrono::microseconds(atoi(args[5])));//500));//
             //Sleep(stod(args[5]));
             if (rcvd_Data > 0  && rand_drop >= atoi(args[4])) {//60){//
                 stringstream stream;
                 //printf("\nreceived header: %.*s\n", header_size, buffer);
                 bool is_header = true;
                 extract_Header(buffer, recvg_header, bufsize, is_header);
                 if (is_header && LastByteAcked == recvg_header.squnc_number){
                     LastByteAcked = recvg_header.squnc_number + rcvd_Data - header_size;
                 }
                 else if (!is_header) printf("bad header from the server!\n");
                 else{
                    //printf("LastByeAcked did not get updated LastByteAcked: %d recvg_header.squnc_number: %d\n",LastByteAcked, recvg_header.squnc_number);
                 }
                 //printf("Acke: %d, header sequ: %d, header ack: %d\n",
                 //       LastByteAcked, recvg_header.squnc_number, recvg_header.ack_number);
                 struct header_struct sendg_header(recvg_header.squnc_number, LastByteAcked, true, false);
                 add_header(sendg_header, stream);
                 sending_Text = stream.str();
                 sendto(clientSocket, sending_Text.c_str(), sending_Text.length(),0, (struct sockaddr *)&srvr_Addr, srvr_Len);
                 //printf("sent ack: %s\n", stream.str().c_str());
                 if (recvg_header.fin_flag == true) {
                     sendto(clientSocket, sending_Text.c_str(), sending_Text.length(), 0,
                            (struct sockaddr *) &srvr_Addr, srvr_Len);
                     //printf("sent ack: %s\n", stream.str().c_str());
                 }

             }
             else {
                 printf("\nDropped a package \n");
                 printf("Number of received packets: %d\n",
                        LastByteAcked / 1410);
             }//perror("Could not read any data");}

             //LastByteAcked += rcvd_Data - header_size;//strlen(buffer);
             //printf("\nNumber received bytes so far : %i\n", LastByteAcked);
             //printf(buffer);
             //
        }while (recvg_header.fin_flag == false || LastByteAcked != recvg_header.ack_number);
        //clock_t end = clock();
        //time_t end;
        //time(&end);
        //printf("number of received bytes: %d\n", LastByteAcked);
        //printf("The file was received and elapsed time(msec)is: %10.3f\n", double(clock() - startTime) * 1000 / CLOCKS_PER_SEC);
    	std::cout << "number of received bytes: " << LastByteAcked << std::endl;
        struct timespec end;//,temp;
        clock_gettime(CLOCK_REALTIME, &end);
        uint64_t delta_us = (end.tv_sec - secs) * 1000000 + (end.tv_nsec - nsecs) / 1000;
    	std::cout << "The file was received and elapsed time(msec)is: " << delta_us / 1000<<endl;//int((stop.tv_usec - start.tv_usec) /1000) <<endl;//(clock() - startTime) / 1000 << std::endl;//
    }
    else if (str.substr(0, 13) == "404 Not Found") {
        printf("Server could not find the requested file!\n");
    }
    close(clientSocket);
    return 0;
}
