#ifndef UNTITLED_ARGS_H
#define UNTITLED_ARGS_H

#include <iostream>
#include <windows.h>
#include <process.h>

using namespace std;

struct Args {
    int* arr;
    int size;
    int maxEl;
    int minEl;
    int avg;
};

void inputArray(Args a, int size);
void outputArray(Args a, int size);
UINT WINAPI min_max(void* p);
UINT WINAPI average(void* p);

#endif //UNTITLED_ARGS_H
