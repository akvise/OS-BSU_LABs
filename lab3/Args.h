#ifndef LAB3_ARGS_H
#define LAB3_ARGS_H

#include <windows.h>

struct Args{
    int* arr;
    int n;
    int num;
    HANDLE actions[2];
    Args(int* _arr, int _n, int _num) : arr(_arr), num(_num),  n(_n){
        actions[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
        actions[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
    }
};


#endif //LAB3_ARGS_H
