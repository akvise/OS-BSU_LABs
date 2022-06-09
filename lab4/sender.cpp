#include <iostream>
#include<windows.h>
#include <fstream>

#define MESSAGE_SIZE 20
#define INF INFINITE

void Message(std::ofstream &out, std::string message, std::string fileName){
    out.open(fileName.c_str(), std::ios::binary | std::ios::app);
    out.write(message.c_str(), MESSAGE_SIZE);
    out.close();
}

int main(int argc, char** argv) {
    std::string fileName = argv[1], nameOfEvent = argv[2];
    std::ofstream out(fileName.c_str(), std::ios::binary);

    if (out.is_open()) {
        HANDLE ReadyEvent = OpenEvent(EVENT_MODIFY_STATE, 0,
                                      nameOfEvent.c_str());
        HANDLE StartAll = OpenEvent(SYNCHRONIZE, 0, "START_ALL");

        if (ReadyEvent == NULL || StartAll == NULL) {
            std::cout << "Error.\n";
            return GetLastError();
        }

        SetEvent(ReadyEvent);

        std::cout << "Launched.\n";

        HANDLE fileMutex = OpenMutex(SYNCHRONIZE, 0, "FILE_ACCESS");

        if(fileMutex == NULL){
            std::cout << "Open mutex has been failed.\n";
            return GetLastError();
        }

        HANDLE senderSemaphore = OpenSemaphore(SEMAPHORE_MODIFY_STATE,
                                               0, "MESSAGES_COUNT_SEM");
        HANDLE mesReadEvent = OpenEvent(EVENT_ALL_ACCESS, 0,
                                        "MESSAGE_READ");

        if (senderSemaphore == NULL || mesReadEvent == NULL) {
            return GetLastError();
        }

        WaitForSingleObject(StartAll, INF);
        std::cout << "Input message: ";
        std::string message;

        while(true){
            std::cin >> message;
            message += '\n';

            if(!std::cin.eof()) {
                WaitForSingleObject(fileMutex, INF);
                Message(out, message.c_str(), fileName);
                ReleaseMutex(fileMutex);

                if(ReleaseSemaphore(senderSemaphore, 1, NULL) != 1){
                    std::cout << "Message file is full. Waiting for receiver.\n";
                    ResetEvent(mesReadEvent);
                    WaitForSingleObject(mesReadEvent, INF);
                    ReleaseSemaphore(senderSemaphore, 1, NULL);
                }
                std::cout << "Message has been sent.\n";
            } else {
                break;
            }
        }
        out.close();
        return 0;
    } else {
        std::cout << "Not open." << std::endl;
        out.close();
        return -1;
    }
}