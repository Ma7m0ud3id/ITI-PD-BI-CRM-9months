// FILE: Reservation.h
#ifndef RESERVATION_H
#define RESERVATION_H

#include <string>

class Reservation {
public:
    int reservationId;
    int bookId;
    std::string bookTitle;
    int memberId;
    std::string memberName;
    std::string reservationDate;
    std::string expiryDate;
    std::string status;
    
    Reservation() : reservationId(0), bookId(0), memberId(0) {}
    
    void display() const;
    bool isPending() const { return status == "Pending"; }
    bool isFulfilled() const { return status == "Fulfilled"; }
};

#endif // RESERVATION_H