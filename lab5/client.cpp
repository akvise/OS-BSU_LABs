#include <iostream>
#include <conio.h>
#include <windows.h>
#include "server.h"

std::string pipeName = "\\\\.\\pipe\\pipe_name";


void messaging(HANDLE hPipe){
    std::cout << "\n\nTo quit press Ctrl+Z" << std::endl;
    while(true){
        char command[10];
        std::cout << "Input command: \n>";
        std::cin.getline(command, 10, '\n');
        if(std::cin.eof()) {
            return;
        }
        DWORD bytes;
        bool isSent = WriteFile(hPipe, command, 10, &bytes, NULL);
        if(!isSent){
            std::cerr << "Message cannot be sent" << std::endl;
            getch();
            return;
        }
        //receiving the answer
        server emp;
        DWORD readBytes;
        bool isRead = ReadFile(hPipe, &emp, sizeof(emp), &readBytes, NULL);
        if(!isRead){
            std::cerr << "Error in receiving answer." << std::endl;
        }
        else{
            if(emp.num < 0) {
                std::cerr << "server not found or being modified." << std::endl;
                continue;
            }
            else {
                //if everything is correct, received an emp
                emp.print(std::cout);
                if('w' == command[0]) {
                    std::cout << "Enter ID, name and working hours of the server:\n>" << std::flush;
                    std::cin >> emp.num >> emp.name >> emp.hours;
                    std::cin.ignore(2, '\n');
                    isSent = WriteFile(hPipe, &emp, sizeof (emp), &bytes, NULL);
                    if (isSent)
                        std::cout << "New record is sent to the server" << std::endl;
                    else {
                        std::cerr << "Error in sending." << std::endl;
                        getch();
                        break;
                    }
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    HANDLE hReadyEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, argv[1]);
    HANDLE hStartEvent = OpenEvent(SYNCHRONIZE, FALSE, "START_ALL");
    if(NULL == hStartEvent || NULL == hReadyEvent){
        std::cerr << "Error in opening start/ready Event."  << std::endl;
        getch();
        return GetLastError();
    }
    SetEvent(hReadyEvent);
    std::cout << "Process is ready." << std::endl;
    WaitForSingleObject(hStartEvent, INFINITE);
    std::cout << "Process is started." << std::endl;

    //connecting to pipe
    HANDLE hPipe;
    while(true) {
        hPipe = CreateFile(pipeName.c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL,
                           OPEN_EXISTING, 0, NULL);
        //if valid, stop trying to connect
        if (INVALID_HANDLE_VALUE != hPipe) {
            break;
        }
        if(!WaitNamedPipe(pipeName.c_str(), 5000)){
            std::cout << "5 second wait timed out." << std::endl;
            getch();
            return 0;
        }
    }
    std::cout << "Connected to pipe." << std::endl;
    messaging(hPipe);
    return 0;
}