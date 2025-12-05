// FILE: Staff.h
#ifndef STAFF_H
#define STAFF_H

#include <string>

class Staff {
public:
    int staffId;
    std::string firstName;
    std::string lastName;
    std::string email;
    std::string phone;
    std::string position;
    std::string hireDate;
    double salary;
    
    Staff() : staffId(0), salary(0.0) {}
    
    void display() const;
    std::string getFullName() const { return firstName + " " + lastName; }
};

#endif // STAFF_H