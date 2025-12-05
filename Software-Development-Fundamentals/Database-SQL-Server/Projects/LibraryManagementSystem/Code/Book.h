// FILE: Book.h
#ifndef BOOK_H
#define BOOK_H

#include <string>

class Book {
public:
    int bookId;
    std::string isbn;
    std::string title;
    std::string author;
    std::string publisher;
    int publicationYear;
    int categoryId;
    std::string categoryName;
    int totalCopies;
    int availableCopies;
    double price;
    std::string shelfLocation;
    
    Book() : bookId(0), publicationYear(0), categoryId(0), 
             totalCopies(0), availableCopies(0), price(0.0) {}
    
    void display() const;
    bool isAvailable() const { return availableCopies > 0; }
};

#endif // BOOK_H