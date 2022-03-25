struct employee{
    int num;
    char name[10];
    double hours;

    friend std::ostream& operator<<(std::ostream &out, employee e){
        out << std::setw(20) << e.num
             << std::setw(20) << e.name
             << std::setw(10)<< e.hours;
        return out;
    }
};
