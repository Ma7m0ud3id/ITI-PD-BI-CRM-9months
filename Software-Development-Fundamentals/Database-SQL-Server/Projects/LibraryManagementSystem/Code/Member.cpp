// FILE: Member.cpp
#include "Member.h"
#include <iostream>

void Member::display() const {
    std::cout << "========================================\n";
    std::cout << "Member ID: " << memberId << "\n";
    std::cout << "Name: " << getFullName() << "\n";
    std::cout << "Email: " << email << "\n";
    std::cout << "Phone: " << phone << "\n";
    std::cout << "Address: " << address << "\n";
    std::cout << "Membership Date: " << membershipDate << "\n";
    std::cout << "Status: " << membershipStatus << "\n";
    std::cout << "========================================\n";
}