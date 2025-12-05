// FILE: Borrowing.h
#ifndef BORROWING_H
#define BORROWING_H

#include <string>

class Borrowing {
public:
    int borrowingId;
    int bookId;
    std::string bookTitle;
    int memberId;
    std::string memberName;
    std::string borrowDate;
    std::string dueDate;
    std::string returnDate;
    std::string status;
    
    Borrowing() : borrowingId(0), bookId(0), memberId(0) {}
    
    void display() const;
    bool isOverdue() const { return status == "Overdue"; }
    bool isReturned() const { return status == "Returned"; }
};

#endif // BORROWING_H