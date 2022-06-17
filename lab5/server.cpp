#include <iostream>
#include <conio.h>
#include <fstream>
#include <time.h>
#include <algorithm>
#include <process.h>
#include <windows.h>
#include "server.h"

int servCount;
server* serv;
HANDLE* events;
CRITICAL_SECTION servCS;
bool *servIsModifying;
std::string pipeName = "\\\\.\\pipe\\pipe_name";

void writeData(char filename[50]){
    std::fstream fin(filename, std::ios::binary | std::ios::out);
    fin.write(reinterpret_cast<char*>(serv), sizeof(server) * servCount);
    fin.close();
}

void readDataSTD(){
    serv = new server[servCount];
    std::cout << "Enter ID, name and working hours of each server:" << std::endl;
    for(int i = 1; i <= servCount; ++i){
        std::cout << "#" << i << ">";
        std::cin >> serv[i-1].num >> serv[i-1].name >> serv[i-1].hours;
    }
}

server* findEmp(int id){
    server key;
    key.num = id;
    return (server*)bsearch((const char*)(&key), (const char*)(serv), servCount, sizeof(server), compare);
}

void startPocesses(int count){
    char buff[10];
    for(int i = 0; i < count; i++) {
        char cmdargs[80] = "..\\..\\Client\\cmake-build-debug\\Client.exe ";
        char eventName[50] = "READY_EVENT_";
        itoa(i + 1, buff, 10);
        strcat(eventName, buff);
        strcat(cmdargs, eventName);
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(STARTUPINFO));
        si.cb = sizeof(STARTUPINFO);
        events[i] = CreateEvent(NULL, TRUE, FALSE, eventName);
        if (!CreateProcess(NULL, cmdargs, NULL, NULL, FALSE, CREATE_NEW_CONSOLE,
                           NULL, NULL, &si, &pi)) {
            printf("Creation process error.\n");
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }
}

DWORD WINAPI messaging(LPVOID p){
    HANDLE hPipe = (HANDLE)p;
    //getting emp with id -1 means for client that error occurred
    server* errorEmp = new server;
    errorEmp->num = -1;
    while(true){
        DWORD readBytes;
        char message[10];
        //receiving a message
        bool isRead = ReadFile(hPipe, message, 10, &readBytes, NULL);
        if(!isRead){
            if(ERROR_BROKEN_PIPE == GetLastError()){
                std::cout << "Client disconnected." << std::endl;
                break;
            }
            else {
                std::cerr << "Error in reading a message." << std::endl;
                break;
            }
        }
        //sending answer
        if(strlen(message) > 0) {
            char command = message[0];
            message[0] = ' ';
            int id = atoi(message);
            DWORD bytesWritten;
            EnterCriticalSection(&servCS);
            server* empToSend = findEmp(id);
            LeaveCriticalSection(&servCS);
            if(NULL == empToSend){
                empToSend = errorEmp;
            }
            else{
                int ind = empToSend -  serv;
                if(servIsModifying[ind])
                    empToSend = errorEmp;
                else{
                    switch (command) {
                        case 'w':
                            printf("Requested to modify ID %d.", id);
                            servIsModifying[ind] = true;
                            break;
                        case 'r':
                            printf("Requested to read ID %d.", id);
                            break;
                        default:
                            std::cout << "Unknown request. ";
                            empToSend = errorEmp;
                    }
                }
            }
            bool isSent = WriteFile(hPipe, empToSend, sizeof(server), &bytesWritten, NULL);
            if(isSent)
                std::cout << "Answer is sent." << std::endl;
            else
                std::cout << "Error in sending answer." << std::endl;
            //receiving a changed record
            if('w' == command && empToSend != errorEmp){
                isRead = ReadFile(hPipe, empToSend, sizeof(server), &readBytes, NULL);
                if(isRead){
                    std::cout << "server record changed." << std::endl;
                    servIsModifying[empToSend - serv] = false;
                    EnterCriticalSection(&servCS);
                    qsort(serv, servCount, sizeof(server), compare);
                    LeaveCriticalSection(&servCS);
                }
                else{
                    std::cerr << "Error in reading a message." << std::endl;
                    break;
                }
            }
        }
    }
    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
    delete errorEmp;
    return 0;
}

void openPipes(int clientCount){
    HANDLE hPipe;
    HANDLE* hThreads = new HANDLE[clientCount];
    for(int i = 0; i < clientCount; i++){
        hPipe = CreateNamedPipe(pipeName.c_str(), PIPE_ACCESS_DUPLEX,
                                PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                                PIPE_UNLIMITED_INSTANCES,0, 0,INFINITE, NULL);
        if (INVALID_HANDLE_VALUE == hPipe){
            std::cerr << "Create named pipe failed." << std::endl;
            getch();
            return;
        }
        if(!ConnectNamedPipe(hPipe, NULL)){
            std::cout << "No connected clients." << std::endl;
            break;
        }
        hThreads[i] = CreateThread(NULL, 0, messaging, (LPVOID)hPipe,0,NULL);
    }
    std::cout << "Clients connected to pipe." << std::endl;
    WaitForMultipleObjects(clientCount, hThreads, TRUE, INFINITE);
    std::cout << "All clients are disconnected." << std::endl;
    delete[] hThreads;
}

int main() {
    //data input
    char filename[50];
    std::cout << "Enter the file name and the count of servers. \n>";
    std::cin >> filename >> servCount;
    readDataSTD();
    writeData(filename);
    qsort(serv, servCount, sizeof(server), compare);

    //creating processes
    InitializeCriticalSection(&servCS);
    srand(time(0));
    int clientCount = 2 + rand() % 3; //from 2 to 4
    HANDLE hstartALL = CreateEvent(NULL, TRUE, FALSE, "START_ALL");
    servIsModifying = new bool[servCount];
    for(int i = 0; i < servCount; ++i)
        servIsModifying[i] = false;
    events = new HANDLE[clientCount];
    startPocesses(clientCount);
    WaitForMultipleObjects(clientCount, events, TRUE, INFINITE);
    std::cout << "All processes are ready.Starting." << std::endl;
    SetEvent(hstartALL);

    //creating pipes
    openPipes(clientCount);
    for(int i = 0; i < servCount; i++)
        serv[i].print(std::cout);
    std::cout << "Press any key to exit" << std::endl;
    getch();
    DeleteCriticalSection(&servCS);
    delete[] servIsModifying;
    delete[] events;
    delete[] serv;
    return 0;
}