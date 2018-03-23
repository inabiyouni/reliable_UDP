#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <regex>
#include <string>
#include <time.h>
#include <pthread.h>
#include <chrono>
using namespace std;

//#define auto_init(variable, value) std::decay<decltype(value)>::type variable = value

struct arg_struct{
    const char *str;
    int* skt;
    struct sockaddr_in* clnt_Addr ;
    socklen_t* clnt_Len;
    string buffer;
    int buffer_size;
    int& LastByteSent;
    int& LastByteAcked;
    int& cwnd;
    int& MSS;
    int& ByteforSampleRTT;
    double& DevRTT;
    double& TimeoutInterval;
    double& sampleRTT;
    double& EstimatedRTT;
    int& ssthresh;
    int& cwndIncrmt;
    int& total_file_size;
    //auto_init(val, 5);
    int& rwnd;
    const char* phase;
    clock_t& begin;
    clock_t& end;
    //int& NextPosSent;
    int a= 0, b = 0, c = 0, d = 0, e = 0, f = 0, i = 0, j = 0, k = 0, l = 0;
    double g = 0.0, h = 0.0, m = 0.0, n = 0.0;
    clock_t t1 = clock();
    clock_t t2 = clock();
    const char* ch = "";
    arg_struct() :buffer(""), buffer_size(0), LastByteSent(a), total_file_size(k), rwnd(l),
                  LastByteAcked(b), cwnd(c), MSS(d), ByteforSampleRTT(f), phase(ch),
                    sampleRTT(g), EstimatedRTT(h), ssthresh(i), cwndIncrmt(j),
                    begin(t1), end(t2), DevRTT(m), TimeoutInterval(n) {}
};
struct header_struct{
    int32_t squnc_number;
    int32_t ack_number;
    bool fin_flag;
    bool ack_flag;
    int32_t a = 0;
    bool b= true;
    header_struct() :squnc_number(a), ack_number(a), ack_flag(b), fin_flag(b) {}
    header_struct(int32_t a, int32_t b, bool c, bool d) : squnc_number(a), ack_number(b), ack_flag(c), fin_flag(d) {}
};
void add_header(header_struct& sendg_header, stringstream& stream){
    char header[22];
    sprintf (header, "%10d%10d%1d%1d", sendg_header.squnc_number, sendg_header.ack_number,
             sendg_header.ack_flag, sendg_header.fin_flag);
    stream << header;
}
void extract_Header(char* buffer, header_struct& recvg_header, int &buffer_size){
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
void add_Date(stringstream* stream){
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[25];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, 25, "%a, %d %h %Y %H:%M:%S", timeinfo);
    char crrnt_Time[35];
    sprintf(crrnt_Time, "Date: %s GMT\n", buffer);
    *stream << crrnt_Time;
}
void make_Message(char* message, int leng, const char *connection_type, stringstream &stream){
    char msg[60];
    sprintf(msg, "HTTP/1.1 %s\n", message);
    stream << msg;
    sprintf(msg, "Connection: %s\n", connection_type);
    stream << msg;
    add_Date(&stream);
    sprintf(msg, "Content-length: %d\n", leng);
    stream << msg;
    //write(connectionSocket, "Server: pydb.py\n", 16);
    //write(connectionSocket, "Transfer-Encoding: chunked\n", 27);
    stream << "Content-Type: text/html;\n\n";
    sprintf(msg, "<html><body>%s</body></html>", message);
    stream << msg;
}
void *send_data(void *pnt_Args) {
    struct arg_struct *args =(struct arg_struct *) pnt_Args;
    int* skt_Connection =  args -> skt;// pnt_Socket;
    const char *connection_type = args -> str;
    struct sockaddr_in* clnt_Addr = args -> clnt_Addr ;
    socklen_t* clnt_Len = args -> clnt_Len;
    string str = args->buffer;
    int buffer_size = args->buffer_size;
    args->begin = clock();
    sendto(*skt_Connection, str.c_str(), buffer_size, 0,
                       (struct sockaddr *) clnt_Addr, *clnt_Len);
    if (args->ByteforSampleRTT == 0) {
        args->sampleRTT = clock();
        args->ByteforSampleRTT = args->LastByteSent;
    }
}
void* receive_data(void *pnt_Args){
    struct arg_struct *args =(struct arg_struct *) pnt_Args;
    int* skt_Connection =  args -> skt;// pnt_Socket;
    const char *connection_type = args -> str;
    struct sockaddr_in* clnt_Addr = args -> clnt_Addr ;
    socklen_t* clnt_Len = args -> clnt_Len;
    int bufsize = 1024;
    char buffer[bufsize];// = malloc(bufsize);
    int rcvd_Data = recvfrom(*skt_Connection, buffer, bufsize, 0, (struct sockaddr *) clnt_Addr, clnt_Len);
    if (rcvd_Data > 0) {
        //printf("\nreceived Ack: %.*s, LastAck: %d, cwnd: %d\n", rcvd_Data, buffer, args->LastByteAcked, args->cwnd);
        struct header_struct recvg_header;
        extract_Header(buffer, recvg_header, bufsize);
        if (args->LastByteAcked == recvg_header.ack_number){
            //Fast recovery = dupAcks
            //args->position = args->LastByteAcked;
            args->LastByteSent = args->LastByteAcked;
            if (strcmp(args->phase, "Fast recovery") != 0) {
                args-> phase = "Fast recovery";
                args->cwndIncrmt = args->MSS * int(args->MSS / args->cwnd);
                args->ssthresh = args->cwnd / 2;
                args->cwnd = args->ssthresh + 3 * args->MSS;
                printf("\nFast recovery \nNumber of transferred packets: %d\n"
                               "Percentage of transferred packets: %d %%\n"
                               "EstimatedRTT(mSec): %2.4f \n",
                       args->LastByteAcked / args->MSS, 100 * args->LastByteAcked / args->total_file_size, args->EstimatedRTT);
            }
        }
        else {
            //Congestion avoidance
            if (strcmp(args->phase, "Congestion avoidance") != 0 && args->cwnd >= args->ssthresh){
                args->phase = "Congestion avoidance";
                args->cwndIncrmt = args->MSS * int(args->MSS / args->cwnd);
                printf("\nCongestion avoidance \nNumber of transferred packets: %d\n"
                       "Percentage of transferred packets: %d %% \n"
                               "EstimatedRTT(mSec): %2.4f \n",
                       args->LastByteAcked / args->MSS, 100 * args->LastByteAcked / args->total_file_size, args-> EstimatedRTT);
            }
            else if (strcmp(args->phase, "Slow start") == 0){

            }
            args->LastByteAcked = recvg_header.ack_number;
            //printf("\nLastByteAcked %d ByteforSampleRTT %d is equal %d\n", args->LastByteAcked, args->ByteforSampleRTT, (args->LastByteAcked == args->ByteforSampleRTT));
            if (args->LastByteAcked >= args->ByteforSampleRTT){
                if (args->LastByteAcked == args->ByteforSampleRTT){
                    double sampleTime = double(clock() - args->sampleRTT) * 1000 / CLOCKS_PER_SEC;
                    if (sampleTime >= args->TimeoutInterval) {
                        args->ssthresh = args->cwnd / 2;
                        args->phase = "Slow start";
                        args->cwnd = args->MSS;
                        args->cwndIncrmt = args->MSS;
                        printf("\nSlow start \nNumber of transferred packets: %d\n"
                       "Percentage of transferred packets: %d %% \n"
                               "EstimatedRTT(mSec): %2.4f \n",
                       args->LastByteAcked / args->MSS, 100 * args->LastByteAcked / args->total_file_size, args-> EstimatedRTT);
                    }
                    else{
                        args->sampleRTT = sampleTime;
                        args->EstimatedRTT = 0.875 * args->EstimatedRTT + 0.125 * args->sampleRTT;
                        args->DevRTT = 0.75 * args->DevRTT + 0.25 * abs(args->sampleRTT - args->EstimatedRTT);
                        args->TimeoutInterval = args->EstimatedRTT + 4 * args->DevRTT;
                    }
                }
                args->ByteforSampleRTT = 0;
                //printf("\n%s\n", "update EstimatedRTT");
            }
        }
        args->cwnd += args->cwndIncrmt;
    }
}
void send_File(void *pnt_Args, const char* pntFilePath, struct header_struct& recvg_header) {
    struct arg_struct *args =(struct arg_struct *) pnt_Args;
    int* skt_Connection =  args -> skt;// pnt_Socket;
    const char *connection_type = args -> str;
    struct sockaddr_in* clnt_Addr = args -> clnt_Addr ;
    socklen_t* clnt_Len = args -> clnt_Len;
    string sending_Text;
    stringstream stream;
    //stringstream& refStream = stream;
    if (strcmp(pntFilePath, "404") == 0) {
        struct header_struct sendg_header(recvg_header.squnc_number, recvg_header.ack_number,
                                        recvg_header.ack_flag, recvg_header.fin_flag);
        add_header(sendg_header, stream);
        make_Message("404 Not Found", 39, connection_type, stream);
        sending_Text = stream.str();
        sendto(*skt_Connection, sending_Text.c_str(), sending_Text.length(), 0, (struct sockaddr *) clnt_Addr, *clnt_Len);
    }
    else{
        //sending header of the HTTP message
        int value_temp = 100;
        int headerSize = 22;
        args->MSS = 1500 - 68 - headerSize;
        args->ssthresh = 64000;
        args->cwnd = args->MSS;
        args->EstimatedRTT = 2;
        args->TimeoutInterval = 2;
        args->phase = "Slow start";
        args->LastByteSent = 0;
        args->LastByteAcked = -1;
        args->cwndIncrmt = args->MSS;
        stringstream stream;
        int squnc_cntr = 0;
        struct header_struct sendg_header(squnc_cntr, squnc_cntr + 1,
                                          false, false);
        add_header(sendg_header, stream);
        std::ifstream in(pntFilePath);
        std::string contents((std::istreambuf_iterator<char>(in)),
                             std::istreambuf_iterator<char>());
        const char* sending_File = contents.c_str();
        //FILE *sending_File = fopen(pntFilePath, "r");
        //if (sending_File == NULL) return;
        //fseek(sending_File, 0L, SEEK_END);
        args->total_file_size = strlen(sending_File);
        make_Message("200 OK", args->total_file_size, connection_type, stream);
        //fseek(sending_File, 0L, SEEK_SET);
        fpos_t position;
        sending_Text = stream.str();
        sendto(*skt_Connection, sending_Text.c_str(), sending_Text.length(), 0, (struct sockaddr *) clnt_Addr, *clnt_Len);
        printf("%s\n", args->phase);
        pthread_t thread_id;
        // sending the file
        while (args->LastByteAcked != args->total_file_size) {
            int read_Size, rcvd_Data;
            char read_Buffer[args->MSS];
            if ((double(clock() - args->begin) * 1000 / CLOCKS_PER_SEC) > 10.0){
                printf("\nDead end cycle avoidance!\n");
                stringstream stream;
                struct header_struct sendg_header(args->LastByteSent, args->LastByteSent + read_Size,
                                                  false, (args->LastByteSent + read_Size != args->total_file_size) ? false : true);
                add_header(sendg_header, stream);
                args->LastByteSent += read_Size;
                //LastByteSent += read_Size;
                squnc_cntr++;
                stream << read_Buffer;
                string str(stream.str());
                string* ptrstr= &str;
                //char ptr_Read_Buffer=
                args->buffer = str;
                args->buffer_size = read_Size + headerSize;
                if( pthread_create( &thread_id , NULL ,  send_data , (void*) args) < 0)
                {
                    perror("could not create send thread");
                }
            }
            if (args->LastByteSent - args->LastByteAcked <= min(args->rwnd, args->cwnd)) {
                //fsetpos (sending_File, &args->position);
                read_Size = min(args->MSS, args->total_file_size - args->LastByteSent);
                memmove(read_Buffer, sending_File + args->LastByteSent, read_Size);

                //read_Size = fread(read_Buffer, sizeof(unsigned char), args->MSS, sending_File);
                //fgetpos (sending_File, &args->LastPsSent);
                //args->position = position;
                if (read_Size > 0) {
                    stringstream stream;
                    struct header_struct sendg_header(args->LastByteSent, args->LastByteSent + read_Size,
                                                      false, (args->LastByteSent + read_Size != args->total_file_size) ? false : true);
                    add_header(sendg_header, stream);
                    args->LastByteSent += read_Size;
                    //LastByteSent += read_Size;
                    squnc_cntr++;
                    stream << read_Buffer;
                    string str(stream.str());
                    string* ptrstr= &str;
                    //char ptr_Read_Buffer=
                    args->buffer = str;
                    args->buffer_size = read_Size + headerSize;
                    if( pthread_create( &thread_id , NULL ,  send_data , (void*) args) < 0)
                    {
                        perror("could not create send thread");
                    }
                    //printf("\nsent data: %.*s\n", read_Size + headerSize, ptr_Read_Buffer);
                    //bzero(buffer, bufsize);
                    //printf("data sent\n");
                    if( pthread_create( &thread_id , NULL ,  receive_data , (void*) args) < 0)
                    {
                        perror("could not create receive thread");
                    }
                }
            }
            //printf("Acke: %d, Sent: %d, cwnd: %d, MSS: %d\n",
            //       args->LastByteAcked,args->LastByteSent,args->cwnd, args->MSS);
        }
        //fclose(sending_File);
        printf("\nTotal number of sent bytes: %d\n", args->LastByteSent);
        printf("The requested file was transmitted!\n");
    }
}

void *crt_new_Connect(void *pnt_Args) {

    struct arg_struct *args = (struct arg_struct *) pnt_Args;
    int *serverSocket = args->skt;// pnt_Socket;
    const char *connection_type = args->str;
    struct sockaddr_in *clnt_Addr = args->clnt_Addr;
    socklen_t *clnt_Len = args->clnt_Len;
    int rcvd_Data;
    int bufsize = 1024;
    char buffer[bufsize];// = malloc(bufsize);

    bzero(buffer, bufsize);
    rcvd_Data = recvfrom(*serverSocket, buffer, bufsize, 0, (struct sockaddr *) clnt_Addr, clnt_Len);

    if (rcvd_Data < 0) perror("ERROR reading from socket");
    struct header_struct recvg_header;
    extract_Header(buffer, recvg_header, bufsize);

    regex pattern("/.+ HTTP");
    smatch m;
    printf("Here is the client message:\n %s\n", buffer);
    string str(buffer);
    if (regex_search(str, m, pattern)) {
        string file_Path = m.str();
        file_Path = file_Path.substr(1, file_Path.size() - 6);
        if (access(file_Path.c_str(), F_OK) != -1) {
            send_File(pnt_Args, file_Path.c_str(), recvg_header);
        } else {
            const char *fndState = "404";
            send_File(pnt_Args, fndState, recvg_header);
        }
    }
}


int main(int argc, char *argv[]) {

/*
    int serverSocket, connectionSocket;
    struct sockaddr_in srvr_Addr, clnt_Addr;
    const char *connection_type, *connection_order;
    socklen_t clnt_Len = sizeof(clnt_Addr);

    if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) > 0){
        printf("The socket was created\n");
    }

    connection_type = "non-persistent";//args[2];//
    srvr_Addr.sin_family = AF_INET;
    srvr_Addr.sin_addr.s_addr = INADDR_ANY;
    //srvr_Addr.sin_port = htons(atoi(args[1]));//"15010"));//
    srvr_Addr.sin_port = htons(atoi(args[1]));//"15010"));//
*/
    const char *connection_type, *connection_order;

    int sockfd, portno;
    struct hostent *hostp; /* client host info */
    char *hostaddrp; /* dotted decimal host addr string */
    char buffer[512];
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    int n;

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        fprintf(stderr,"ERROR opening socket");

    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
	     (const void *)&optval , sizeof(int));

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        fprintf(stderr,"ERROR on binding");

    connection_type = "non-persistent";
    connection_order = "close";

/*    
    if (strcmp(connection_type,"persistent") == 0) connection_order = "keep-alive";
    else if (strcmp(connection_type, "non-persistent") == 0) connection_order = "close";
    else{
        perror("Entered connection type is no valid!");
        exit(1);
    }

    if (bind(serverSocket, (struct sockaddr *) &srvr_Addr, sizeof(srvr_Addr)) == 0){
        printf("Binding Socket\n");
    }

    if (serverSocket < 0 ) {
        perror("server cannot connect to client");
        exit(1);
    }
    printf("\nNew Client connection...\n");

    struct arg_struct thrd_Args;
    thrd_Args.str = connection_order;
    thrd_Args.skt = &serverSocket;
    thrd_Args.clnt_Addr = &clnt_Addr;
    thrd_Args.clnt_Len = & clnt_Len;
    thrd_Args.rwnd = atoi(args[2]);
    crt_new_Connect((void*) &thrd_Args);
*/
    //struct arg_struct thrd_Args;
    //thrd_Args.str = connection_order;
    //thrd_Args.skt = &sockfd;
    //thrd_Args.clnt_Addr = &cli_addr;
    //thrd_Args.clnt_Len = & clilen;
    //thrd_Args.rwnd = atoi(argv[2]);
    //crt_new_Connect((void*) &thrd_Args);
        
    
    bzero(buffer,512);
    n = recvfrom(sockfd,buffer,512, 0, (struct sockaddr *) &cli_addr, &clilen);
    if (n < 0) fprintf(stderr,"ERROR reading from socket");
    hostp = gethostbyaddr((const char *)&cli_addr.sin_addr.s_addr, sizeof(cli_addr.sin_addr.s_addr), AF_INET);
    if (hostp == NULL) fprintf(stderr,"ERROR on gethostbyaddr");
    hostaddrp = inet_ntoa(cli_addr.sin_addr);
    if (hostaddrp == NULL) fprintf(stderr,"ERROR on inet_ntoa\n");
    printf("server received datagram from %s (%s)\n", hostp->h_name, hostaddrp);
    printf("server received %d/%d bytes: %s\n", strlen(buffer), n, buffer);
    printf("Here is the message:\n%s",buffer);

    //Now join the thread , so that we dont terminate before the thread
    //pthread_join( thread_id , NULL);
    puts("Handler assigned");
    close(sockfd);
    return 0;
}

