// FILE: DBManager.h
#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <nanodbc/nanodbc.h>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <stdexcept>
#include <iostream>

// Forward declarations
class Book;
class Category;
class Member;
class Staff;
class Borrowing;
class Reservation;

class DBManager {
private:
    std::unique_ptr<nanodbc::connection> conn;
    std::string connectionString;
    std::ofstream logFile;
    
    void log(const std::string& message);
    void logError(const std::string& error);

public:
    // Constructor/Destructor
    DBManager();
    ~DBManager();
    
    // Connection management
    bool connect(const std::string& connStr);
    bool disconnect();
    bool isConnected() const;
    bool testConnection();
    
    // Category operations
    bool createCategory(const std::string& name, const std::string& description);
    std::vector<Category> getAllCategories();
    
    // Book operations
    bool createBook(const std::string& isbn, const std::string& title, 
                    const std::string& author, const std::string& publisher,
                    int year, int categoryId, int totalCopies, 
                    double price, const std::string& shelfLocation);
    std::vector<Book> getAllBooks();
    std::vector<Book> getAvailableBooks();
    std::vector<Book> searchBooksByTitle(const std::string& title);
    Book getBookById(int bookId);
    bool updateBookAvailability(int bookId, int availableCopies);
    bool deleteBook(int bookId);
    
    // Member operations
    bool createMember(const std::string& firstName, const std::string& lastName,
                      const std::string& email, const std::string& phone,
                      const std::string& address);
    std::vector<Member> getAllMembers();
    Member getMemberById(int memberId);
    bool updateMemberStatus(int memberId, const std::string& status);
    
    // Staff operations
    bool createStaff(const std::string& firstName, const std::string& lastName,
                     const std::string& email, const std::string& phone,
                     const std::string& position, double salary);
    std::vector<Staff> getAllStaff();
    
    // Borrowing operations
    bool createBorrowing(int bookId, int memberId, int staffId, 
                         const std::string& dueDate);
    std::vector<Borrowing> getAllBorrowings();
    std::vector<Borrowing> getCurrentBorrowings();
    std::vector<Borrowing> getMemberBorrowings(int memberId);
    bool returnBook(int borrowingId);
    bool markOverdueBooks();
    
    // Reservation operations
    bool createReservation(int bookId, int memberId);
    std::vector<Reservation> getAllReservations();
    bool cancelReservation(int reservationId);
    
    // Stored procedure calls
    int executeUpdateOverdueBooks();
    int executeCalculateOverdueFines(double dailyRate = 1.0);
    
    // Utility
    std::string getLastError() const;
};

#endif // DBMANAGER_H