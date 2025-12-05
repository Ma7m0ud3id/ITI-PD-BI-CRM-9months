// FILE: Member.h
#ifndef MEMBER_H
#define MEMBER_H

#include <string>

class Member {
public:
    int memberId;
    std::string firstName;
    std::string lastName;
    std::string email;
    std::string phone;
    std::string address;
    std::string membershipDate;
    std::string membershipStatus;
    
    Member() : memberId(0) {}
    
    void display() const;
    std::string getFullName() const { return firstName + " " + lastName; }
    bool isActive() const { return membershipStatus == "Active"; }
};

#endif // MEMBER_H