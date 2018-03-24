#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

using namespace std;

//
// Created by nabiyuni on 3/23/2018.
//
main() {
    char buffer[] = "Filename:somt.txt\n";
    //std::regex pattern("/.*");
    //smatch m;
    printf("Here is the client message:\n%s\n", buffer);
    string file_Path(buffer);
    //char part[8];
    char part[10];
    memcpy(part, buffer, 8);
    if (file_Path.substr(0, 9) == "Filename:") {printf(file_Path.substr(9, file_Path.size()-10).c_str());}
    else printf(file_Path.c_str());
}