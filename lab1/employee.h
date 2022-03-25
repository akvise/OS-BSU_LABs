class employee{
public:
    int num;
    char name[10];
    double hours;

    employee() {
        num = 0;
        strcpy(name, "Noname");
        hours = 0.;
    }
    employee(int num_, const char name_[], double hours_) {
        num = num_;
        strcpy(name, name_);
        hours = hours_;
    }

    friend std::ostream& operator<<(std::ostream &out, employee e){
        out << std::setw(20) << e.num
             << std::setw(20) << e.name
             << std::setw(10)<< e.hours;
        return out;
    }
};
