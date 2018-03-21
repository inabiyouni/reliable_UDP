#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <regex>
#include <netdb.h>
#include <ctime>
//#include <Windows.h>
#include <chrono>
#include <thread>
#include <ctime>
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
    char header[22];
    sprintf (header, "%10d%10d%1d%1d", sendg_header.squnc_number, sendg_header.ack_number,
             sendg_header.ack_flag, sendg_header.fin_flag);
    stream << header;
}
void extract_Header(char* buffer, header_struct& recvg_header, int& buffer_size){
    int numbers_size = 10;
    int flag_size = 1;
    int headerSize = 2 * numbers_size + 2 * flag_size;
    char temp[numbers_size];
    memmove (temp, buffer , numbers_size);
    recvg_header.squnc_number = atoi(temp);
    memmove (temp, buffer + numbers_size , numbers_size);
    recvg_header.ack_number = atoi(temp);
    memmove (temp, buffer + 2* numbers_size , flag_size);
    recvg_header.ack_flag = (temp[0] == '1') ? true:false;
    memmove (temp, buffer + 2* numbers_size + flag_size , flag_size);
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
    int clientSocket, rcvd_Data, server_port;
    int bufsize = atoi(args[6]);
    char buffer[bufsize];
    const char *server_host, *connection_type, *connection_order, *filename;// = malloc(bufsize);
    struct sockaddr_in srvr_Addr, clnt_Addr;
    struct hostent *hst_Name;
    socklen_t clnt_Len = sizeof(clnt_Addr);
    socklen_t srvr_Len = sizeof(srvr_Addr);

    server_host = args[1];//"149.160.210.223";//192.168.2.223";//149.160.200.10";//
    server_port = atoi(args[2]);//15010;//
    connection_type = "non-persistent";//args[3];//
    filename = args[3];//"text.txt";//HelloWorld.html";//
/*
    server_host = "192.168.2.223";//149.160.210.223";//192.168.2.223";//149.160.200.10";//
    server_port = 15010;//
    connection_type = "non-persistent";//
    filename = "text_1MB.txt";//text_1MB.txt";//HelloWorld.html";//
*/
    if (strcmp(connection_type,"persistent") == 0) connection_order = "keep-alive";
    else if (strcmp(connection_type, "non-persistent") == 0) connection_order = "close";
    else{
        perror("Entered connection type is no valid!");
        exit(1);
    }

    string zeroTo255 = "(\\d{1,2}|(0|1)\\d{2}|2[0-4]\\d|25[0-5])";
    string compPattern = zeroTo255 + "\\." + zeroTo255 + "\\." + zeroTo255 + "\\." + zeroTo255;
    regex pattern(compPattern);//"(\\d{1,3})\.(\\d{1,3})");

    //time_t begin;
    //time(&begin);
    //clock_t begin = clock();
    clock_t startTime = clock(); //Start timer
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
    sprintf(message, "GET /%s HTTP/1.1\n", filename);
    stream << message;
    sprintf(message, "Host: %s\n", server_host);
    stream << message;
    sprintf(message, "Connection:%s\n", connection_order);
    stream << message;
    add_Date(&stream);

    sending_Text = stream.str();
    //printf(sending_Text.c_str());
    sendto(clientSocket, sending_Text.c_str(), sending_Text.length(),0, (struct sockaddr *)&srvr_Addr, srvr_Len);

    bzero(buffer,bufsize);
    rcvd_Data = recvfrom(clientSocket, buffer, bufsize, 0, (struct sockaddr *)&srvr_Addr, &srvr_Len);
    struct header_struct recvg_header;
    extract_Header(buffer, recvg_header, bufsize);

    regex okay_Pttrn("HTTP/.+ 200 OK");
    regex notFnd_Pttrn("HTTP/.+ 404 Not Found");
    regex pat_connct_len("Content-length:.+\n");
    smatch m;
    if (rcvd_Data < 0) perror("ERROR reading from socket");
    //printf("Here is the server message:\n %s\n", buffer);
    string str(buffer);
    string connection_len;
    if (regex_search(str, m, okay_Pttrn)) {
        regex dblReturn_Pttrn("\\n\\n");
        if (regex_search(str, m, dblReturn_Pttrn))
            memmove (buffer, buffer + m.position(), strlen (buffer));// = buffer(m.position(),str.length());
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
             std::this_thread::sleep_for(std::chrono::microseconds(atoi(args[5])));
             //Sleep(stod(args[5]));
             if (rcvd_Data > 0  && rand_drop >= atoi(args[4])) {
                 stringstream stream;
                 //printf("\nreceived header: %.*s\n", header_size, buffer);
                 extract_Header(buffer, recvg_header, bufsize);
                 if (LastByteAcked == recvg_header.squnc_number){
                     LastByteAcked = recvg_header.squnc_number + rcvd_Data - header_size;
                 }
                 else{
                    //printf("LastByeAcked did not get updated\n");
                 }
                 //printf("Acke: %d, header sequ: %d, header ack: %d\n",
                 //       LastByteAcked, recvg_header.squnc_number, recvg_header.ack_number);
                 struct header_struct sendg_header(recvg_header.squnc_number, LastByteAcked, true, false);
                 add_header(sendg_header, stream);
                 sending_Text = stream.str();
                 sendto(clientSocket, sending_Text.c_str(), sending_Text.length(),0, (struct sockaddr *)&srvr_Addr, srvr_Len);

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
        //printf("The file was received and elapsed time(msec)is: %2.3f", difftime(end, begin));//double(end - begin) / CLOCKS_PER_SEC);
	std::cout << "number of received bytes: " << LastByteAcked << std::endl;	
	std::cout << "The file was received and elapsed time(msec)is: " << (clock() - startTime) << std::endl;
    }
    else if (regex_search(str, m, notFnd_Pttrn)) {
        printf("Server could not find the requested file!\n");
    }
    close(clientSocket);
    return 0;
}
