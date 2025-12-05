// FILE: Borrowing.cpp
#include "Borrowing.h"
#include <iostream>

void Borrowing::display() const {
    std::cout << "========================================\n";
    std::cout << "Borrowing ID: " << borrowingId << "\n";
    std::cout << "Member: " << memberName << " (ID: " << memberId << ")\n";
    std::cout << "Book: " << bookTitle << " (ID: " << bookId << ")\n";
    std::cout << "Borrow Date: " << borrowDate << "\n";
    std::cout << "Due Date: " << dueDate << "\n";
    
    if (!returnDate.empty()) {
        std::cout << "Return Date: " << returnDate << "\n";
    } else {
        std::cout << "Return Date: Not returned yet\n";
    }
    
    std::cout << "Status: " << status;
    if (isOverdue()) {
        std::cout << " (OVERDUE!)";
    }
    std::cout << "\n========================================\n";
}