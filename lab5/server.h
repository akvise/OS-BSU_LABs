//
// Created by Akim on 6/17/2022.
//

#ifndef LAB5_SERVER_H
#define LAB5_SERVER_H


#include <ostream>
class server {
public:
    int num;
    char name[10];
    double hours;
    void print(std::ostream &out){
        out << "ID: " << num << "\tName: " << name << "\tHours: " << hours << std::endl;
    }
};

int compare(const void* p1, const void* p2){
    return ((server*)p1)->num - ((server*)p2)->num;
}


#endif //LAB5_SERVER_H
