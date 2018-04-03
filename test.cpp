#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
using namespace std;

//
// Created by nabiyuni on 3/23/2018.
//
main() {
    uint64_t a=120;
    uint64_t b = 200;
    uint64_t temp= int(0.875 * a + 0.125 * b);
    printf("%d",temp);
}