#include <iostream>
#include <fstream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <sys/select.h>
//#include <winsock2.h>
//#include <ws2tcpip.h>
#include <stdio.h>
//#include <windows.h>
//#include <regex>
#include <string>
#include <sstream>
//#include <ctime>
#include <sys/time.h>
#include <pthread.h>
#include <stdint.h>
//#include <chrono>
using namespace std;

//#define auto_init(variable, value) std::decay<decltype(value)>::type variable = value


struct arg_struct{
    //const char *str;
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
    uint64_t& DevRTT;
    uint64_t& TimeoutInterval;
    clock_t& sampleRTT;
    uint64_t& EstimatedRTT;
    int& ssthresh;
    int& cwndIncrmt;
    int& total_file_size;
    int& dead_end_cnt;
    //struct timespec* st;
    long& secs, usecs;
    //auto_init(val, 5);
    int& rwnd;
    const char* phase;
    long& dead_secs, dead_nsecs;
    clock_t& begin;
    clock_t& end;
    //int& NextPosSent;
    int a= 0, b = 0, c = 0, d = 0, e = 0, f = 0, i = 0, j = 0, k = 0, l = 0, g = 0;
    uint64_t h = 0, m = 0, n = 0;
    long o = 0, p = 0, q = 0, r = 0;
    clock_t t1 = clock();
    clock_t t2 = clock();
    clock_t t3 = clock();
    const char* ch = "";
    arg_struct() :buffer(""), buffer_size(0), LastByteSent(a), total_file_size(k), rwnd(l),
                  LastByteAcked(b), cwnd(c), MSS(d), ByteforSampleRTT(f), phase(ch),
                    sampleRTT(t1), EstimatedRTT(h), ssthresh(i), cwndIncrmt(j), dead_secs(q), dead_nsecs(r),
                    begin(t3), end(t2), DevRTT(m), TimeoutInterval(n), dead_end_cnt(g), usecs(o), secs(p) {}
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
    char header[23];
    sprintf (header, "%10d%10d%1d%1d", sendg_header.squnc_number, sendg_header.ack_number,
             sendg_header.ack_flag, sendg_header.fin_flag);
    stream << header;
}
void extract_Header(char* buffer, header_struct& recvg_header, int &buffer_size, bool&acptHeader){
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
void make_Message(char* message, stringstream &stream){
    char msg[60];
    sprintf(msg, "%s\n", message);
    stream << msg;
    /*
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
     */
}
void *send_data(void *pnt_Args) {
    struct arg_struct *args =(struct arg_struct *) pnt_Args;
    int* skt_Connection =  args -> skt;// pnt_Socket;
    //const char *connection_type = args -> str;
    struct sockaddr_in* clnt_Addr = args -> clnt_Addr ;
    socklen_t* clnt_Len = args -> clnt_Len;
    string str = args->buffer;
    char temp[10];
    memmove (temp, args->buffer.c_str(), 10);

    try {
        stoi(temp);
    }
    catch(...) {
        printf("bad buffer to send\n");
    }

    int buffer_size = args->buffer_size;
    args->begin = clock();
    struct timespec temp_dead;
    clock_gettime(CLOCK_REALTIME, &temp_dead);
    args->dead_nsecs = temp_dead.tv_nsec;
    args->dead_secs = temp_dead.tv_sec;
    //printf("time numbers: %d %d\n",args->dead_nsecs,args->dead_secs);
    int send_size = sendto(*skt_Connection, str.c_str(), buffer_size, 0,
                       (struct sockaddr *) clnt_Addr, *clnt_Len);

        //printf("\nsent data: %.*s, LastAck: %d, cwnd: %d\n", 22, str.c_str(), args->LastByteAcked, args->cwnd);
        if (args->ByteforSampleRTT == 0) {
        //printf("\n start sample RTT\n");
        //printf(clock());
        struct timespec temp;
        clock_gettime(CLOCK_REALTIME, &temp);
        args->secs = temp.tv_sec;
        args->usecs = temp.tv_nsec;
        args->sampleRTT = clock();
        args->ByteforSampleRTT = args->LastByteSent;
    }
}
void* receive_data(void *pnt_Args){
    struct arg_struct *args =(struct arg_struct *) pnt_Args;
    int* skt_Connection =  args -> skt;// pnt_Socket;
    //const char *connection_type = args -> str;
    struct sockaddr_in* clnt_Addr = args -> clnt_Addr ;
    socklen_t* clnt_Len = args -> clnt_Len;
    int bufsize = 1024;
    char buffer[bufsize];// = malloc(bufsize);

    //struct timeval tv;
    //tv.tv_sec = 1;
    //tv.tv_usec = 100;
    //if (setsockopt(*skt_Connection, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
    //    perror("Error socket timeout");
    //}
    //setsockopt(*skt_Connection, SOL_SOCKET, SO_RCVTIMEO,(char *)&tv,sizeof(struct timeval));
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 3 * args->TimeoutInterval;

    // Setup fd_set structure
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(*skt_Connection, &fds);

    // Return value:
    // -1: error occurred
    // 0: timed out
    // > 0: data ready to be read
    int retval = select(*skt_Connection+1, &fds, NULL, NULL, &timeout);
    if(retval == -1){
        //printf("Error");
        return NULL;
    }
    else if(retval == 0){
        //printf("\n------Timeout-------\n");
        return NULL;
    }
    else {
        int rcvd_Data = recvfrom(*skt_Connection, buffer, bufsize, 0, (struct sockaddr *) clnt_Addr, clnt_Len);
        if (rcvd_Data > 0) {
            if (rcvd_Data < 22) printf("short reading from client!\n");
            //printf("\nreceived Ack: %.*s, LastAck: %d, cwnd: %d\n", rcvd_Data, buffer, args->LastByteAcked, args->cwnd);
            struct header_struct recvg_header;
            bool is_header = true;
            extract_Header(buffer, recvg_header, bufsize, is_header);
            if (is_header) {
                if (args->LastByteAcked == recvg_header.ack_number) {
                    //Fast recovery = dupAcks
                    //args->position = args->LastByteAcked;
                    args->LastByteSent = args->LastByteAcked;
                    if (strcmp(args->phase, "Fast recovery") != 0) {
                        args->phase = "Fast recovery";
                        args->cwndIncrmt = args->MSS * int(args->MSS / args->cwnd);
                        args->ssthresh = args->cwnd / 2;
                        args->cwnd = args->ssthresh + 3 * args->MSS;
                        printf("\nFast recovery \nNumber of transferred packets: %d\n"
                                       "Percentage of transferred packets: %d %%\n"
                                       "EstimatedRTT(micro Sec): %d \n",
                               args->LastByteAcked / args->MSS, 100 * args->LastByteAcked / args->total_file_size,
                               args->EstimatedRTT);
                    }
                } else {
                    //Congestion avoidance
                    if (strcmp(args->phase, "Congestion avoidance") != 0 && args->cwnd >= args->ssthresh) {
                        args->phase = "Congestion avoidance";
                        args->cwndIncrmt = args->MSS * int(args->MSS / args->cwnd);
                        printf("\nCongestion avoidance \nNumber of transferred packets: %d\n"
                                       "Percentage of transferred packets: %d %% \n"
                                       "EstimatedRTT(micro Sec): %d \n",
                               args->LastByteAcked / args->MSS, 100 * args->LastByteAcked / args->total_file_size,
                               args->EstimatedRTT);
                    } else if (strcmp(args->phase, "Slow start") == 0) {

                    }
                    args->LastByteAcked = recvg_header.ack_number;
                    //printf("\nLastByteAcked %d ByteforSampleRTT %d is equal %d\n", args->LastByteAcked, args->ByteforSampleRTT, (args->LastByteAcked == args->ByteforSampleRTT));
                    if (args->LastByteAcked >= args->ByteforSampleRTT) {
                        if (args->LastByteAcked == args->ByteforSampleRTT) {
                            struct timespec end;//,temp;
                            clock_gettime(CLOCK_REALTIME, &end);
                            //temp = *args->st;
                            uint64_t delta_us = ((end.tv_sec - args->secs) * 1000000 + (end.tv_nsec - args->usecs) / 1000);
                            //printf("delta_us %d %d %d %d %d", delta_us, end.tv_nsec, args->usecs, end.tv_sec, args->secs);
                            double sampleTime = double(clock() - args->sampleRTT);// * 1000 / CLOCKS_PER_SEC;
                            if (delta_us >= args->TimeoutInterval) {
                                args->ssthresh = args->cwnd / 2;
                                args->phase = "Slow start";
                                args->cwnd = args->MSS;
                                args->cwndIncrmt = args->MSS;
                                printf("\nSlow start \nNumber of transferred packets: %d\n"
                                               "Percentage of transferred packets: %d %% \n"
                                               "EstimatedRTT(micro Sec): %d \n",
                                       args->LastByteAcked / args->MSS,
                                       100 * args->LastByteAcked / args->total_file_size,
                                       args->EstimatedRTT);
                            }// else {
                            //cout<< "\nupdate RTT\n"<<  sampleTime<< endl;
                            //printf(double(clock() - args->sampleRTT));
                            //args->sampleRTT = sampleTime;
                            args->EstimatedRTT = int(0.875 * args->EstimatedRTT + 0.125 * delta_us);
                            args->DevRTT = int(0.75 * args->DevRTT + 0.05 * delta_us);//0.25 * abs(double(delta_us - args->EstimatedRTT)));
                            args->TimeoutInterval = int(args->EstimatedRTT + 4 * args->DevRTT);
                            //printf("EstimatedRTT %d %d %d %d\n", args->EstimatedRTT, args->DevRTT, args->TimeoutInterval, 0.05 * delta_us);
                            //}
                        }
                        args->ByteforSampleRTT = 0;
                        //printf("\n%s\n", "update EstimatedRTT");
                    }
                }
                args->cwnd += args->cwndIncrmt;
            } else printf("bad header from client!\n");
        }
    }
}
void send_File(void *pnt_Args, const char* pntFilePath, struct header_struct& recvg_header) {
    struct arg_struct *args =(struct arg_struct *) pnt_Args;
    int* skt_Connection =  args -> skt;// pnt_Socket;
    //const char *connection_type = args -> str;
    struct sockaddr_in* clnt_Addr = args -> clnt_Addr ;
    socklen_t* clnt_Len = args -> clnt_Len;
    string sending_Text;
    stringstream stream;
    //stringstream& refStream = stream;
    if (strcmp(pntFilePath, "404") == 0) {
        struct header_struct sendg_header(recvg_header.squnc_number, recvg_header.ack_number,
                                        recvg_header.ack_flag, recvg_header.fin_flag);
        add_header(sendg_header, stream);
        make_Message("404 Not Found", stream);
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
        args->EstimatedRTT = 1000;
        args->TimeoutInterval = 1000;
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
        make_Message("200 OK", stream);
        //fseek(sending_File, 0L, SEEK_SET);
        fpos_t position;
        sending_Text = stream.str();
        sendto(*skt_Connection, sending_Text.c_str(), sending_Text.length(), 0, (struct sockaddr *) clnt_Addr, *clnt_Len);
        printf("%s\n", args->phase);
        int thread_cnt = 0;
        // sending the file
        while (args->LastByteAcked != args->total_file_size) {
            int read_Size, rcvd_Data;
            char read_Buffer[args->MSS];
            read_Size = min(args->MSS, args->total_file_size - args->LastByteSent);
            if (args->LastByteSent - args->LastByteAcked <= min(args->rwnd, args->cwnd) && read_Size>0) {
                args->dead_end_cnt = 0;
                //fsetpos (sending_File, &args->position);

                memmove(read_Buffer, sending_File + args->LastByteSent, read_Size);

                //read_Size = fread(read_Buffer, sizeof(unsigned char), args->MSS, sending_File);
                //fgetpos (sending_File, &args->LastPsSent);
                //args->position = position;
                //if (read_Size > 0) {
                stringstream stream;
                struct header_struct sendg_header(args->LastByteSent, args->LastByteSent + read_Size,
                                                  false, (args->LastByteSent + read_Size != args->total_file_size) ? false : true);
                //printf("sent header %10d%10d%1d%1d\n", sendg_header.squnc_number, sendg_header.ack_number,
                //         sendg_header.ack_flag, sendg_header.fin_flag);
                add_header(sendg_header, stream);
                args->LastByteSent += read_Size;
                //LastByteSent += read_Size;
                squnc_cntr++;
                stream << read_Buffer;
                string str(stream.str());
                string* ptrstr= &str;
                //printf("args->LastByteSent: %d read size: %d", args->LastByteSent, read_Size);
                //printf(" sent data completely: %s", str.c_str());
                args->buffer = str;
                args->buffer_size = read_Size + headerSize;
                pthread_t thread_id_sender, thread_id_recvr;
                int            rc, stat;
                void *status;
                size_t         s1;
                pthread_attr_t attr;

                rc = pthread_attr_init(&attr);
                if (rc == -1) {
                    perror("error in pthread_attr_init");
                    exit(1);
                }
                s1 = 4096;
                rc = pthread_attr_setstacksize(&attr, s1);
                if (rc == -1) {
                    perror("error in pthread_attr_setstacksize");
                    exit(2);
                }
                if( pthread_create( &thread_id_sender , &attr ,  send_data , (void*) args) < 0)
                {
                    perror("could not create send thread");
                }

                thread_cnt++;
                //printf("\nsent data: %.*s\n", read_Size + headerSize, ptr_Read_Buffer);
                //bzero(buffer, bufsize);
                //printf("data sent\n");
                if( pthread_create( &thread_id_recvr , &attr ,  receive_data , (void*) args) < 0)
                {
                    perror("could not create receive thread");
                }
                rc = pthread_detach(thread_id_recvr);//, &status);
                if (rc) {
                    cout << "Error:unable to join," << rc << endl;
                    exit(-1);
                }
                rc = pthread_detach(thread_id_sender);//, &status);
                if (rc) {
                    cout << "Error:unable to join," << rc << endl;
                    exit(-1);
                }
                thread_cnt++;
                //printf("thread numbers: %d\n",thread_cnt);
                //}
                //else {printf("could not read the file! %d\n", read_Size);}
            }
            else{
                struct timespec en;
                clock_gettime(CLOCK_REALTIME, &en);
                uint64_t delta_us = (en.tv_sec - args->dead_secs) * 1000000 + (en.tv_nsec - args->dead_nsecs)/ 1000;

                if (delta_us > args->TimeoutInterval * 10){//(clock()- args->begin) / 1000 > 2){//}
                    //printf("\nclock delta %ld timeout %ld for dead end %d %d %d %d \n", delta_us, args->TimeoutInterval,
                    //       end.tv_sec, args->dead_secs, end.tv_nsec, args->dead_nsecs);
                    if (args->dead_end_cnt >= 1){
                        //perror("Dead end finished, no ack from client!\n");
                        break;
                    }
                    else{
                        args->dead_end_cnt ++;
                    }
                    printf("\nDead end cycle avoidance!\n");
                    stringstream stream;
                    read_Size = min(args->MSS, args->total_file_size - args->LastByteAcked);
                    memmove(read_Buffer, sending_File + args->LastByteAcked, read_Size);
                    if (read_Size > 0) {
                        struct header_struct sendg_header(args->LastByteAcked, args->LastByteAcked + read_Size,
                                                          false,
                                                          (args->LastByteAcked + read_Size != args->total_file_size) ? false
                                                                                                                     : true);
                        add_header(sendg_header, stream);
                        args->LastByteSent = args->LastByteAcked + read_Size;
                        //LastByteSent += read_Size;
                        squnc_cntr++;
                        stream << read_Buffer;
                        string str(stream.str());
                        string *ptrstr = &str;
                        //char ptr_Read_Buffer=
                        args->buffer = str;
                        args->buffer_size = read_Size + headerSize;
                        //printf("sent header for dead end cycle %.*s\n", 50, str.c_str());
                        pthread_t thread_id_sender;
                        int            rc, stat;
                        void *status;
                        size_t         s1;
                        pthread_attr_t attr;

                        rc = pthread_attr_init(&attr);
                        if (rc == -1) {
                            perror("error in pthread_attr_init");
                            exit(1);
                        }

                        s1 = 4096;
                        rc = pthread_attr_setstacksize(&attr, s1);
                        if (rc == -1) {
                            perror("error in pthread_attr_setstacksize");
                            exit(2);
                        }
                        if (pthread_create(&thread_id_sender, &attr, send_data, (void *) args) < 0) {
                            perror("could not create send thread");
                        }
                        rc = pthread_detach(thread_id_sender);//, &status);
                        if (rc) {
                            cout << "Error:unable to join," << rc << endl;
                            exit(-1);
                        }
                        //printf("data has been sent in the dead cycle! delta_us: %d\n",delta_us);
                    }
                    else {
                        printf("args->LastByteAcked: %d read size: %d", args->LastByteAcked, read_Size);
                        perror("could not read the file in a Dead end cycle!\n");
                    }
                    printf("receing allowance: %d %d\n", args->LastByteSent - args->LastByteAcked, min(args->rwnd, args->cwnd));
                    printf("last byte ack: %d %d ", args->LastByteAcked , args->total_file_size);
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
    //const char *connection_type = args->str;
    struct sockaddr_in *clnt_Addr = args->clnt_Addr;
    socklen_t *clnt_Len = args->clnt_Len;
    int rcvd_Data;
    int bufsize = 1024;
    char buffer[bufsize];// = malloc(bufsize);

    bzero(buffer, bufsize);
    rcvd_Data = recvfrom(*serverSocket, buffer, bufsize, 0, (struct sockaddr *) clnt_Addr, clnt_Len);

    if (rcvd_Data < 0) perror("ERROR reading from socket");
    struct header_struct recvg_header;
    bool is_header = true;
    extract_Header(buffer, recvg_header, bufsize, is_header);

    //regex pattern("Filename:.*\n");
    //smatch m;
    //printf("Here is the client message:\n%s\n", buffer);
    string file_Path(buffer);
    if (file_Path.substr(0, 9) == "Filename:"){ //(regex_search(str, m, pattern)) {
        //string file_Path = m.str();
        file_Path = file_Path.substr(9, file_Path.size() - 10);
        //printf("file Path:%s\n",file_Path.c_str());
        if (access(file_Path.c_str(), F_OK) != -1) {
            send_File(pnt_Args, file_Path.c_str(), recvg_header);
        } else {
            const char *fndState = "404";
            send_File(pnt_Args, fndState, recvg_header);
        }
    }
}
int main(int arg_Num, char *args[]) {//){//
    fflush(stdout);
    int serverSocket, connectionSocket;
    struct sockaddr_in srvr_Addr, clnt_Addr;
    //const char *connection_type, *connection_order;
    socklen_t clnt_Len = sizeof(clnt_Addr);

    if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) > 0) {
        printf("The socket was created\n");
    }

    //connection_type = "non-persistent";//args[2];//
    srvr_Addr.sin_family = AF_INET;
    srvr_Addr.sin_addr.s_addr = INADDR_ANY;
    srvr_Addr.sin_port = htons(atoi(args[1]));//"15010"));//
    //srvr_Addr.sin_port = htons(atoi("15010"));//
    /*
    if (strcmp(connection_type,"persistent") == 0) connection_order = "keep-alive";
    else if (strcmp(connection_type, "non-persistent") == 0) connection_order = "close";
    else{
        perror("Entered connection type is no valid!");
        exit(1);
    }
*/
    if (bind(serverSocket, (struct sockaddr *) &srvr_Addr, sizeof(srvr_Addr)) == 0) {
        printf("Binding Socket\n");
    }

    if (serverSocket < 0) {
        perror("server cannot connect to client");
        exit(1);
    }
    printf("\nNew Client connection...\n");

    struct arg_struct thrd_Args;
    //struct timespec *temp = thrd_Args.st;
    //clock_gettime(CLOCK_MONOTONIC_RAW, (void *) thrd_Args.st);
    //thrd_Args.str = connection_order;
    thrd_Args.skt = &serverSocket;
    thrd_Args.clnt_Addr = &clnt_Addr;
    thrd_Args.clnt_Len = &clnt_Len;
    //thrd_Args.rwnd = atoi(args[2]);
    thrd_Args.rwnd = atoi(args[2]);//150000;//
    crt_new_Connect((void *) &thrd_Args);

    //Now join the thread , so that we dont terminate before the thread
    //pthread_join( thread_id , NULL);
    puts("Handler assigned");
    close(serverSocket);

        return 0;
}

