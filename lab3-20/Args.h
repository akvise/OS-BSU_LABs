#ifndef lab3_20
#define lab3_20
#include <vector>
#include <thread>
#include <condition_variable>
#include <mutex>

struct threadArgs{
    std::vector<int> arr;
    int n;
    int num;
    bool actions[2];
    std::condition_variable doAction;
    std::mutex actionMutex;
    threadArgs(std::vector<int> _arr, int _n, int _num) : arr(_arr), num(_num),  n(_n){
        actions[0] = false;
        actions[1] = false;
    }
};

#endif