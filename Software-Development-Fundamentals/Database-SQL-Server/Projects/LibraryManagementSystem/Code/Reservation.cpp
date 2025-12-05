// FILE: Reservation.cpp
#include "Reservation.h"
#include <iostream>

void Reservation::display() const {
    std::cout << "========================================\n";
    std::cout << "Reservation ID: " << reservationId << "\n";
    std::cout << "Member: " << memberName << " (ID: " << memberId << ")\n";
    std::cout << "Book: " << bookTitle << " (ID: " << bookId << ")\n";
    std::cout << "Reservation Date: " << reservationDate << "\n";
    std::cout << "Expiry Date: " << expiryDate << "\n";
    std::cout << "Status: " << status << "\n";
    std::cout << "========================================\n";
}