// FILE: Book.cpp
#include "Book.h"
#include <iostream>
#include <iomanip>

void Book::display() const {
    std::cout << "========================================\n";
    std::cout << "Book ID: " << bookId << "\n";
    std::cout << "ISBN: " << isbn << "\n";
    std::cout << "Title: " << title << "\n";
    std::cout << "Author: " << author << "\n";
    std::cout << "Publisher: " << publisher << "\n";
    std::cout << "Year: " << publicationYear << "\n";
    std::cout << "Category: " << categoryName << "\n";
    std::cout << "Available: " << availableCopies << "/" << totalCopies << "\n";
    std::cout << "Price: $" << std::fixed << std::setprecision(2) << price << "\n";
    std::cout << "Location: " << shelfLocation << "\n";
    std::cout << "Status: " << (isAvailable() ? "Available" : "Not Available") << "\n";
    std::cout << "========================================\n";
}