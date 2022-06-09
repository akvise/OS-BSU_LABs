#include <iostream>
#include <fstream>
#include<conio.h>
#include <Windows.h>

HANDLE *Events;
const int MESSAGE_SIZE = 20;
int Create(int sendersCount, std::string fileName){

    Events = new HANDLE[sendersCount];
    char buff[10];

    for(int i = 0; i < sendersCount; i++){

        char eventName[30] = "sender";
        strcat(eventName, itoa(i, buff, 10));
        Events[i] = CreateEvent(NULL, TRUE, FALSE, eventName);

        if (NULL == Events[i]) {
            printf("Creation event failed.");
            return GetLastError();

        }

        std::string senderEXE = "Sender.exe ";
        senderEXE += fileName + " " + eventName;

        char* SendEXE;
        strcpy(SendEXE, senderEXE.c_str());

        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(STARTUPINFO));
        si.cb = sizeof(STARTUPINFO);
        if (!CreateProcess(NULL, SendEXE, NULL, NULL, FALSE,
                           CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
            printf("Creating process error.\n");
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }
    std::cout <<"Receiver created " << sendersCount << " senders.\n";
}

std::string Message(std::string filename){
    std::fstream in(filename.c_str(), std::ios::binary | std::ios::in);
    if(!in.is_open()){
        return "File failed.\n";
    }

    if(in.peek() == std::ifstream::traits_type::eof())
        return "File is empty.";


    char res[MESSAGE_SIZE];
    in.read(res, MESSAGE_SIZE);

    in.seekg(0, std::ios::end);
    int n = in.tellg();
    in.seekg(0, std::ios::beg);
    char *temp = new char[n];
    in.read(temp, n);
    in.close();
    in.open(filename.c_str(), std::ios::binary | std::ios::out);
    in.clear();
    in.write(temp + MESSAGE_SIZE, n - MESSAGE_SIZE);
    in.close();
    delete[] temp;
    return res;
}

int main() {
    std::string filename;
    std::cout <<"Input filename: ";
    int senderCount;
    std::cin >> filename >> senderCount;
    std::fstream in(filename.c_str(), std::ios::binary | std::ios::out);
    in.clear();
    in.close();

    HANDLE startALL = CreateEvent(NULL, TRUE, FALSE, "START_ALL");
    HANDLE fileMutex = CreateMutex(NULL, FALSE, "FILE_ACCESS");
    if(NULL == fileMutex){
        printf("Mutex creation failed.");
        return GetLastError();
    }

    HANDLE senderSemaphore = CreateSemaphore(NULL, 0, senderCount, "MESSAGES_COUNT_SEM");
    HANDLE mesReadEvent = CreateEvent(NULL, FALSE, FALSE, "MESSAGE_READ");
    if (NULL == senderSemaphore|| NULL == mesReadEvent)
        return GetLastError();

    Create(senderCount, filename);
    WaitForMultipleObjects(senderCount, Events, TRUE, INFINITE);
    std::cout << "All senders are ready. Starting." << std::endl;
    SetEvent(startALL);
    char tmp[MESSAGE_SIZE];
    char message[MESSAGE_SIZE];

    while(true){
        std::cin >> tmp;

        if(std::cin.eof())
            break;

        std::cout << "Waiting for a message." << std::endl;
        WaitForSingleObject(senderSemaphore, INFINITE);
        WaitForSingleObject(fileMutex, INFINITE);

        std::cout << Message(filename) << '\n';

        ReleaseMutex(fileMutex);
        SetEvent(mesReadEvent);
    }
    delete[] Events;
}