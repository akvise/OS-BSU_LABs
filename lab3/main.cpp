#include <iostream>
#include <vector>
#include <windows.h>
#include <process.h>
#include "Args.h"

CRITICAL_SECTION critical_selection;
HANDLE eventHandle;
std::vector<HANDLE> events;

int main() {
    std::cout << "Input size of array: ";
    int n; std::cin >> n;
    int *arr = new int[n];
    for(int i = 0; i < n; i++) {
        arr[i] = 0;
    }
    std::cout<<"Array of"<< n << " elements is created.\n";


    int threadCount;
    std::cout<<"Enter thread number: ";
    std::cin >> threadCount;

    eventHandle = CreateEvent(NULL, TRUE, FALSE, NULL);
    std::vector<HANDLE> threads;
    HANDLE currThread;
    std::vector<Args*> argsVec;
    Args* currArgs;

    bool* terminated = new bool[threadCount];

    for(int i = 0; i < threadCount; i++){
        currArgs = new Args(arr, n, i+1);
        currThread = (HANDLE)_beginthreadex(NULL, 0, marker, currArgs, 0, NULL);


        terminated[i] = false;
        events.push_back(CreateEvent(NULL, TRUE, FALSE, NULL));
        argsVec.push_back(currArgs);
        threads.push_back(currThread);
    }
    std::cout<<threadCount<<" are ready to start.\n";

    return 0;
}


UINT WINAPI marker(void *p){
    Args* args = static_cast<Args*>(p);
    WaitForSingleObject(eventHandle, INFINITE);

    int action = WaitForMultipleObjects(2,args->actions, FALSE, INFINITE) - WAIT_OBJECT_0;


    int count = 0;
    while(action != 1){
        EnterCriticalSection(&critical_selection);

        int i = rand() % args->n;

        if(args->arr[i] == 0){
            Sleep(5);
            std::cout << "Thread #" << args->num << ": Marked " << count << " elements, unable " << i << '\n';
            args->arr[i] = args->num;
            ++count;
            LeaveCriticalSection(&critical_selection);
            Sleep(5);
        } else {
            LeaveCriticalSection(&critical_selection);
            SetEvent(events[args->num-1]);
            int action = WaitForMultipleObjects(2,
                                                args->actions,
                                                FALSE,
                                                INFINITE) - WAIT_OBJECT_0;
            if(action == 1){
                for(int i = 0; i < args->n; i++){
                    if(args->arr[i] == args->num){
                        args->arr[i] = 0;
                    }
                }
                std::cout<<"Thread" <<  args->num << "is terminated.\n" ;
                break;
            }
        }
    }
    return NULL;
}
