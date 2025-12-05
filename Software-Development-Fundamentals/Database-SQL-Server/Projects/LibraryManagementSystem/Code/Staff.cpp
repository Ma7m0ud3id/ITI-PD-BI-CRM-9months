// FILE: Staff.cpp
#include "Staff.h"
#include <iostream>
#include <iomanip>

void Staff::display() const {
    std::cout << "========================================\n";
    std::cout << "Staff ID: " << staffId << "\n";
    std::cout << "Name: " << getFullName() << "\n";
    std::cout << "Email: " << email << "\n";
    std::cout << "Phone: " << phone << "\n";
    std::cout << "Position: " << position << "\n";
    std::cout << "Hire Date: " << hireDate << "\n";
    std::cout << "Salary: $" << std::fixed << std::setprecision(2) << salary << "\n";
    std::cout << "========================================\n";
}