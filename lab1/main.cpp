#include <iostream>
#include <conio.h>
#include <iomanip>
#include <string>
#include <windows.h>
#include "employee.h"
#include <fstream>

using namespace std;

void printFile(string filename){
    ifstream in(filename.c_str(), ios::in|ios::binary|ios::ate);
    in.seekg(0, ios::end);
    int n = in.tellg()/sizeof(employee);
    in.seekg(0, ios::beg);
    employee* emps = new employee[n];
    in.read((char*)emps, n*sizeof(employee));
    in.close();
    cout << left;
    for(int i = 0; i < n; i++){
        cout << emps[i] << endl;
    }
}

bool createFile(int count, const string& filename){
    STARTUPINFO s;
    PROCESS_INFORMATION piApp;
    ZeroMemory(&s, sizeof(STARTUPINFO));
    s.cb = sizeof(STARTUPINFO);
    char args[50] = "Creator.exe ";
    char buff[10];
    strcat(args, itoa(count, buff, 10 ));
    strcat(args, " ");
    strcat(args, filename.c_str());
    bool isStarted = CreateProcess(NULL, args, NULL, NULL, FALSE,
                                   CREATE_NEW_CONSOLE, NULL, NULL, &s, &piApp);
    WaitForSingleObject(piApp.hProcess, INFINITE);
    CloseHandle(piApp.hThread);
    CloseHandle(piApp.hProcess);
    return isStarted;
}

bool writeReportFile(string filename, string reportname, int salary){
    STARTUPINFO s;
    PROCESS_INFORMATION piApp;
    ZeroMemory(&s, sizeof(STARTUPINFO));
    s.cb = sizeof(STARTUPINFO);
    char args[50] = "Reporter.exe ";
    char buff[10];
    strcat(args, filename.c_str()); strcat(args, " ");
    strcat(args, reportname.c_str()); strcat(args, " ");
    strcat(args, itoa(salary, buff, 10));
    bool isStarted = CreateProcess(NULL, args, NULL, NULL, FALSE,
                                   CREATE_NEW_CONSOLE, NULL, NULL, &s, &piApp);
    WaitForSingleObject(piApp.hProcess, INFINITE);
    CloseHandle(piApp.hThread);
    CloseHandle(piApp.hProcess);
    return isStarted;
}

int main(int argc, char *argv[]){
    setlocale(LC_ALL, "Russian");
    int count;
    cout << "Input number:";
    cin >> count;

    string filename;
    cout << "Input filename:";
    cin >> filename;
    if(createFile(count, filename)) printFile(filename);
    else cout << "Something wrong!" << endl;

    cout << "Input report filename:";
    string reportFile;
    cin >> reportFile;
    int salary;
    cout << "Salary: ";
    cin >> salary;

    if(writeReportFile(filename, reportFile, salary)){
        ifstream in(reportFile.c_str());
        string line;
        while(getline(in, line)){
            cout << line << endl;
        }
    }
    else cout << "Something wrong!" << endl;

    system("pause");
    return 0;
}