// FILE: DBManager.cpp
#include <cstring>
#include "DBManager.h"
#include "Book.h"
#include "Category.h"
#include "Member.h"
#include "Staff.h"
#include "Borrowing.h"
#include "Reservation.h"
#include <sstream>
#include <iomanip>
#include <ctime>

DBManager::DBManager() {
    logFile.open("library_db.log", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Warning: Could not open log file." << std::endl;
    }
    log("DBManager initialized");
}

DBManager::~DBManager() {
    disconnect();
    if (logFile.is_open()) {
        log("DBManager destroyed");
        logFile.close();
    }
}

/*void DBManager::log(const std::string& message) {
    if (logFile.is_open()) {
        time_t now = time(nullptr);
        char timeStr[26];
        ctime_s(timeStr, sizeof(timeStr), &now);
        timeStr[24] = '\0'; // Remove newline
        logFile << "[" << timeStr << "] " << message << std::endl;
    }
}*/
void DBManager::log(const std::string& message) {
    if (logFile.is_open()) {
        time_t now = time(nullptr);
        char* timePtr = std::ctime(&now); 

        
        char timeStr[26]; 
        
        std::strncpy(timeStr, timePtr, 25); 
        timeStr[25] = '\0'; 
       
        std::string timeString = timePtr;
        timeString.pop_back(); 
        logFile << "[" << timeString << "] " << message << std::endl;
    }
}

void DBManager::logError(const std::string& error) {
    log("ERROR: " + error);
    std::cerr << "ERROR: " << error << std::endl;
}

bool DBManager::connect(const std::string& connStr) {
    try {
        connectionString = connStr;
        conn = std::make_unique<nanodbc::connection>(connStr);
        log("Connected to database successfully");
        return true;
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Connection failed: ") + e.what());
        return false;
    }
}

bool DBManager::disconnect() {
    if (conn && conn->connected()) {
        try {
            conn->disconnect();
            log("Disconnected from database");
            return true;
        } catch (const nanodbc::database_error& e) {
            logError(std::string("Disconnect failed: ") + e.what());
            return false;
        }
    }
    return true;
}

bool DBManager::isConnected() const {
    return conn && conn->connected();
}

bool DBManager::testConnection() {
    if (!isConnected()) {
        logError("Not connected to database");
        return false;
    }
    
    try {
        nanodbc::result result = nanodbc::execute(*conn, "SELECT 1 AS TestConnection");
        if (result.next()) {
            int value = result.get<int>(0);
            if (value == 1) {
                log("Connection test successful");
                return true;
            }
        }
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Connection test failed: ") + e.what());
    }
    return false;
}

// ============================================
// Category Operations
// ============================================
bool DBManager::createCategory(const std::string& name, const std::string& description) {
    if (!isConnected()) return false;
    
    try {
        nanodbc::statement stmt(*conn);
        prepare(stmt, "INSERT INTO Categories (CategoryName, Description) VALUES (?, ?)");
        
        stmt.bind(0, name.c_str());
        stmt.bind(1, description.c_str());
        
        nanodbc::execute(stmt);
        log("Category created: " + name);
        return true;
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Create category failed: ") + e.what());
        return false;
    }
}

std::vector<Category> DBManager::getAllCategories() {
    std::vector<Category> categories;
    if (!isConnected()) return categories;
    
    try {
        nanodbc::result result = nanodbc::execute(*conn, 
            "SELECT CategoryID, CategoryName, Description FROM Categories ORDER BY CategoryName");
        
        while (result.next()) {
            Category cat;
            cat.categoryId = result.get<int>(0);
            cat.categoryName = result.get<std::string>(1);
            cat.description = result.get<std::string>(2, "");
            categories.push_back(cat);
        }
        
        log("Retrieved " + std::to_string(categories.size()) + " categories");
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Get categories failed: ") + e.what());
    }
    
    return categories;
}

// ============================================
// Book Operations
// ============================================
bool DBManager::createBook(const std::string& isbn, const std::string& title,
                           const std::string& author, const std::string& publisher,
                           int year, int categoryId, int totalCopies,
                           double price, const std::string& shelfLocation) {
    if (!isConnected()) return false;
    
    try {
        nanodbc::statement stmt(*conn);
        prepare(stmt, "INSERT INTO Books (ISBN, Title, Author, Publisher, PublicationYear, "
                     "CategoryID, TotalCopies, AvailableCopies, Price, ShelfLocation) "
                     "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        
        stmt.bind(0, isbn.c_str());
        stmt.bind(1, title.c_str());
        stmt.bind(2, author.c_str());
        stmt.bind(3, publisher.c_str());
        stmt.bind(4, &year);
        stmt.bind(5, &categoryId);
        stmt.bind(6, &totalCopies);
        stmt.bind(7, &totalCopies); // AvailableCopies initially equals TotalCopies
        stmt.bind(8, &price);
        stmt.bind(9, shelfLocation.c_str());
        
        nanodbc::execute(stmt);
        log("Book created: " + title + " (ISBN: " + isbn + ")");
        return true;
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Create book failed: ") + e.what());
        return false;
    }
}

std::vector<Book> DBManager::getAllBooks() {
    std::vector<Book> books;
    if (!isConnected()) return books;
    
    try {
        nanodbc::result result = nanodbc::execute(*conn,
            "SELECT b.BookID, b.ISBN, b.Title, b.Author, b.Publisher, b.PublicationYear, "
            "b.CategoryID, c.CategoryName, b.TotalCopies, b.AvailableCopies, b.Price, b.ShelfLocation "
            "FROM Books b "
            "INNER JOIN Categories c ON b.CategoryID = c.CategoryID "
            "ORDER BY b.Title");
        
        while (result.next()) {
            Book book;
            book.bookId = result.get<int>(0);
            book.isbn = result.get<std::string>(1);
            book.title = result.get<std::string>(2);
            book.author = result.get<std::string>(3);
            book.publisher = result.get<std::string>(4, "");
            book.publicationYear = result.get<int>(5);
            book.categoryId = result.get<int>(6);
            book.categoryName = result.get<std::string>(7);
            book.totalCopies = result.get<int>(8);
            book.availableCopies = result.get<int>(9);
            book.price = result.get<double>(10);
            book.shelfLocation = result.get<std::string>(11, "");
            books.push_back(book);
        }
        
        log("Retrieved " + std::to_string(books.size()) + " books");
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Get all books failed: ") + e.what());
    }
    
    return books;
}

std::vector<Book> DBManager::getAvailableBooks() {
    std::vector<Book> books;
    if (!isConnected()) return books;
    
    try {
        nanodbc::result result = nanodbc::execute(*conn,
            "SELECT BookID, ISBN, Title, Author, Publisher, PublicationYear, "
            "CategoryName, AvailableCopies, TotalCopies, Price, ShelfLocation "
            "FROM AvailableBooks ORDER BY Title");
        
        while (result.next()) {
            Book book;
            book.bookId = result.get<int>(0);
            book.isbn = result.get<std::string>(1);
            book.title = result.get<std::string>(2);
            book.author = result.get<std::string>(3);
            book.publisher = result.get<std::string>(4, "");
            book.publicationYear = result.get<int>(5);
            book.categoryName = result.get<std::string>(6);
            book.availableCopies = result.get<int>(7);
            book.totalCopies = result.get<int>(8);
            book.price = result.get<double>(9);
            book.shelfLocation = result.get<std::string>(10, "");
            books.push_back(book);
        }
        
        log("Retrieved " + std::to_string(books.size()) + " available books");
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Get available books failed: ") + e.what());
    }
    
    return books;
}

std::vector<Book> DBManager::searchBooksByTitle(const std::string& title) {
    std::vector<Book> books;
    if (!isConnected()) return books;
    
    try {
        nanodbc::statement stmt(*conn);
        prepare(stmt, "SELECT b.BookID, b.ISBN, b.Title, b.Author, b.Publisher, b.PublicationYear, "
                     "b.CategoryID, c.CategoryName, b.TotalCopies, b.AvailableCopies, b.Price, b.ShelfLocation "
                     "FROM Books b "
                     "INNER JOIN Categories c ON b.CategoryID = c.CategoryID "
                     "WHERE b.Title LIKE ? ORDER BY b.Title");
        
        std::string searchPattern = "%" + title + "%";
        stmt.bind(0, searchPattern.c_str());
        
        nanodbc::result result = nanodbc::execute(stmt);
        
        while (result.next()) {
            Book book;
            book.bookId = result.get<int>(0);
            book.isbn = result.get<std::string>(1);
            book.title = result.get<std::string>(2);
            book.author = result.get<std::string>(3);
            book.publisher = result.get<std::string>(4, "");
            book.publicationYear = result.get<int>(5);
            book.categoryId = result.get<int>(6);
            book.categoryName = result.get<std::string>(7);
            book.totalCopies = result.get<int>(8);
            book.availableCopies = result.get<int>(9);
            book.price = result.get<double>(10);
            book.shelfLocation = result.get<std::string>(11, "");
            books.push_back(book);
        }
        
        log("Search found " + std::to_string(books.size()) + " books for: " + title);
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Search books failed: ") + e.what());
    }
    
    return books;
}

Book DBManager::getBookById(int bookId) {
    Book book;
    if (!isConnected()) return book;
    
    try {
        nanodbc::statement stmt(*conn);
        prepare(stmt, "SELECT b.BookID, b.ISBN, b.Title, b.Author, b.Publisher, b.PublicationYear, "
                     "b.CategoryID, c.CategoryName, b.TotalCopies, b.AvailableCopies, b.Price, b.ShelfLocation "
                     "FROM Books b "
                     "INNER JOIN Categories c ON b.CategoryID = c.CategoryID "
                     "WHERE b.BookID = ?");
        
        stmt.bind(0, &bookId);
        nanodbc::result result = nanodbc::execute(stmt);
        
        if (result.next()) {
            book.bookId = result.get<int>(0);
            book.isbn = result.get<std::string>(1);
            book.title = result.get<std::string>(2);
            book.author = result.get<std::string>(3);
            book.publisher = result.get<std::string>(4, "");
            book.publicationYear = result.get<int>(5);
            book.categoryId = result.get<int>(6);
            book.categoryName = result.get<std::string>(7);
            book.totalCopies = result.get<int>(8);
            book.availableCopies = result.get<int>(9);
            book.price = result.get<double>(10);
            book.shelfLocation = result.get<std::string>(11, "");
        }
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Get book by ID failed: ") + e.what());
    }
    
    return book;
}

bool DBManager::updateBookAvailability(int bookId, int availableCopies) {
    if (!isConnected()) return false;
    
    try {
        nanodbc::statement stmt(*conn);
        prepare(stmt, "UPDATE Books SET AvailableCopies = ?, UpdatedAt = GETDATE() WHERE BookID = ?");
        
        stmt.bind(0, &availableCopies);
        stmt.bind(1, &bookId);
        
        nanodbc::execute(stmt);
        log("Book availability updated: BookID " + std::to_string(bookId) + 
            ", Available: " + std::to_string(availableCopies));
        return true;
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Update book availability failed: ") + e.what());
        return false;
    }
}

bool DBManager::deleteBook(int bookId) {
    if (!isConnected()) return false;
    
    try {
        nanodbc::statement stmt(*conn);
        prepare(stmt, "DELETE FROM Books WHERE BookID = ?");
        
        stmt.bind(0, &bookId);
        nanodbc::execute(stmt);
        
        log("Book deleted: BookID " + std::to_string(bookId));
        return true;
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Delete book failed: ") + e.what());
        return false;
    }
}

// ============================================
// Member Operations
// ============================================
bool DBManager::createMember(const std::string& firstName, const std::string& lastName,
                             const std::string& email, const std::string& phone,
                             const std::string& address) {
    if (!isConnected()) return false;
    
    try {
        nanodbc::statement stmt(*conn);
        prepare(stmt, "INSERT INTO Members (FirstName, LastName, Email, Phone, Address) "
                     "VALUES (?, ?, ?, ?, ?)");
        
        stmt.bind(0, firstName.c_str());
        stmt.bind(1, lastName.c_str());
        stmt.bind(2, email.c_str());
        stmt.bind(3, phone.c_str());
        stmt.bind(4, address.c_str());
        
        nanodbc::execute(stmt);
        log("Member created: " + firstName + " " + lastName);
        return true;
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Create member failed: ") + e.what());
        return false;
    }
}

std::vector<Member> DBManager::getAllMembers() {
    std::vector<Member> members;
    if (!isConnected()) return members;
    
    try {
        nanodbc::result result = nanodbc::execute(*conn,
            "SELECT MemberID, FirstName, LastName, Email, Phone, Address, "
            "CONVERT(VARCHAR, MembershipDate, 23) AS MembershipDate, MembershipStatus "
            "FROM Members ORDER BY LastName, FirstName");
        
        while (result.next()) {
            Member member;
            member.memberId = result.get<int>(0);
            member.firstName = result.get<std::string>(1);
            member.lastName = result.get<std::string>(2);
            member.email = result.get<std::string>(3);
            member.phone = result.get<std::string>(4, "");
            member.address = result.get<std::string>(5, "");
            member.membershipDate = result.get<std::string>(6);
            member.membershipStatus = result.get<std::string>(7);
            members.push_back(member);
        }
        
        log("Retrieved " + std::to_string(members.size()) + " members");
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Get all members failed: ") + e.what());
    }
    
    return members;
}

Member DBManager::getMemberById(int memberId) {
    Member member;
    if (!isConnected()) return member;
    
    try {
        nanodbc::statement stmt(*conn);
        prepare(stmt, "SELECT MemberID, FirstName, LastName, Email, Phone, Address, "
                     "CONVERT(VARCHAR, MembershipDate, 23) AS MembershipDate, MembershipStatus "
                     "FROM Members WHERE MemberID = ?");
        
        stmt.bind(0, &memberId);
        nanodbc::result result = nanodbc::execute(stmt);
        
        if (result.next()) {
            member.memberId = result.get<int>(0);
            member.firstName = result.get<std::string>(1);
            member.lastName = result.get<std::string>(2);
            member.email = result.get<std::string>(3);
            member.phone = result.get<std::string>(4, "");
            member.address = result.get<std::string>(5, "");
            member.membershipDate = result.get<std::string>(6);
            member.membershipStatus = result.get<std::string>(7);
        }
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Get member by ID failed: ") + e.what());
    }
    
    return member;
}

bool DBManager::updateMemberStatus(int memberId, const std::string& status) {
    if (!isConnected()) return false;
    
    try {
        nanodbc::statement stmt(*conn);
        prepare(stmt, "UPDATE Members SET MembershipStatus = ? WHERE MemberID = ?");
        
        stmt.bind(0, status.c_str());
        stmt.bind(1, &memberId);
        
        nanodbc::execute(stmt);
        log("Member status updated: MemberID " + std::to_string(memberId) + ", Status: " + status);
        return true;
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Update member status failed: ") + e.what());
        return false;
    }
}

// ============================================
// Staff Operations
// ============================================
bool DBManager::createStaff(const std::string& firstName, const std::string& lastName,
                            const std::string& email, const std::string& phone,
                            const std::string& position, double salary) {
    if (!isConnected()) return false;
    
    try {
        nanodbc::statement stmt(*conn);
        prepare(stmt, "INSERT INTO Staff (FirstName, LastName, Email, Phone, Position, Salary) "
                     "VALUES (?, ?, ?, ?, ?, ?)");
        
        stmt.bind(0, firstName.c_str());
        stmt.bind(1, lastName.c_str());
        stmt.bind(2, email.c_str());
        stmt.bind(3, phone.c_str());
        stmt.bind(4, position.c_str());
        stmt.bind(5, &salary);
        
        nanodbc::execute(stmt);
        log("Staff created: " + firstName + " " + lastName);
        return true;
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Create staff failed: ") + e.what());
        return false;
    }
}

std::vector<Staff> DBManager::getAllStaff() {
    std::vector<Staff> staffList;
    if (!isConnected()) return staffList;
    
    try {
        nanodbc::result result = nanodbc::execute(*conn,
            "SELECT StaffID, FirstName, LastName, Email, Phone, Position, "
            "CONVERT(VARCHAR, HireDate, 23) AS HireDate, Salary "
            "FROM Staff ORDER BY LastName, FirstName");
        
        while (result.next()) {
            Staff staff;
            staff.staffId = result.get<int>(0);
            staff.firstName = result.get<std::string>(1);
            staff.lastName = result.get<std::string>(2);
            staff.email = result.get<std::string>(3);
            staff.phone = result.get<std::string>(4, "");
            staff.position = result.get<std::string>(5, "");
            staff.hireDate = result.get<std::string>(6);
            staff.salary = result.get<double>(7);
            staffList.push_back(staff);
        }
        
        log("Retrieved " + std::to_string(staffList.size()) + " staff members");
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Get all staff failed: ") + e.what());
    }
    
    return staffList;
}

// ============================================
// Borrowing Operations
// ============================================
bool DBManager::createBorrowing(int bookId, int memberId, int staffId, 
                                const std::string& dueDate) {
    if (!isConnected()) return false;
    
    try {
        // Start transaction
        nanodbc::execute(*conn, "BEGIN TRANSACTION");
        
        // Insert borrowing
        nanodbc::statement stmt(*conn);
        prepare(stmt, "INSERT INTO Borrowings (BookID, MemberID, StaffID, DueDate) "
                     "VALUES (?, ?, ?, ?)");
        
        stmt.bind(0, &bookId);
        stmt.bind(1, &memberId);
        stmt.bind(2, &staffId);
        stmt.bind(3, dueDate.c_str());
        
        nanodbc::execute(stmt);
        
        // Update book availability
        nanodbc::statement updateStmt(*conn);
        prepare(updateStmt, "UPDATE Books SET AvailableCopies = AvailableCopies - 1, "
                           "UpdatedAt = GETDATE() WHERE BookID = ? AND AvailableCopies > 0");
        updateStmt.bind(0, &bookId);
        nanodbc::execute(updateStmt);
        
        // Commit transaction
        nanodbc::execute(*conn, "COMMIT TRANSACTION");
        
        log("Borrowing created: BookID " + std::to_string(bookId) + 
            ", MemberID " + std::to_string(memberId));
        return true;
    } catch (const nanodbc::database_error& e) {
        try {
            nanodbc::execute(*conn, "ROLLBACK TRANSACTION");
        } catch (...) {}
        logError(std::string("Create borrowing failed: ") + e.what());
        return false;
    }
}

std::vector<Borrowing> DBManager::getAllBorrowings() {
    std::vector<Borrowing> borrowings;
    if (!isConnected()) return borrowings;
    
    try {
        nanodbc::result result = nanodbc::execute(*conn,
            "SELECT br.BorrowingID, br.BookID, b.Title, br.MemberID, "
            "m.FirstName + ' ' + m.LastName AS MemberName, "
            "CONVERT(VARCHAR, br.BorrowDate, 120) AS BorrowDate, "
            "CONVERT(VARCHAR, br.DueDate, 120) AS DueDate, "
            "CONVERT(VARCHAR, br.ReturnDate, 120) AS ReturnDate, br.Status "
            "FROM Borrowings br "
            "INNER JOIN Books b ON br.BookID = b.BookID "
            "INNER JOIN Members m ON br.MemberID = m.MemberID "
            "ORDER BY br.BorrowDate DESC");
        
        while (result.next()) {
            Borrowing borrowing;
            borrowing.borrowingId = result.get<int>(0);
            borrowing.bookId = result.get<int>(1);
            borrowing.bookTitle = result.get<std::string>(2);
            borrowing.memberId = result.get<int>(3);
            borrowing.memberName = result.get<std::string>(4);
            borrowing.borrowDate = result.get<std::string>(5);
            borrowing.dueDate = result.get<std::string>(6);
            borrowing.returnDate = result.get<std::string>(7, "");
            borrowing.status = result.get<std::string>(8);
            borrowings.push_back(borrowing);
        }
        
        log("Retrieved " + std::to_string(borrowings.size()) + " borrowings");
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Get all borrowings failed: ") + e.what());
    }
    
    return borrowings;
}

std::vector<Borrowing> DBManager::getCurrentBorrowings() {
    std::vector<Borrowing> borrowings;
    if (!isConnected()) return borrowings;
    
    try {
        nanodbc::result result = nanodbc::execute(*conn,
            "SELECT BorrowingID, MemberName, Email, BookTitle, ISBN, "
            "CONVERT(VARCHAR, BorrowDate, 120) AS BorrowDate, "
            "CONVERT(VARCHAR, DueDate, 120) AS DueDate, DaysOverdue, Status "
            "FROM CurrentBorrowings ORDER BY DaysOverdue DESC");
        
        while (result.next()) {
            Borrowing borrowing;
            borrowing.borrowingId = result.get<int>(0);
            borrowing.memberName = result.get<std::string>(1);
            borrowing.bookTitle = result.get<std::string>(3);
            borrowing.borrowDate = result.get<std::string>(5);
            borrowing.dueDate = result.get<std::string>(6);
            borrowing.status = result.get<std::string>(8);
            borrowings.push_back(borrowing);
        }
        
        log("Retrieved " + std::to_string(borrowings.size()) + " current borrowings");
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Get current borrowings failed: ") + e.what());
    }
    
    return borrowings;
}

std::vector<Borrowing> DBManager::getMemberBorrowings(int memberId) {
    std::vector<Borrowing> borrowings;
    if (!isConnected()) return borrowings;
    
    try {
        nanodbc::statement stmt(*conn);
        prepare(stmt, "EXEC GetMemberBorrowings ?");
        stmt.bind(0, &memberId);
        
        nanodbc::result result = nanodbc::execute(stmt);
        
        while (result.next()) {
            Borrowing borrowing;
            borrowing.borrowingId = result.get<int>(0);
            borrowing.bookTitle = result.get<std::string>(1);
            borrowing.borrowDate = result.get<std::string>(4);
            borrowing.dueDate = result.get<std::string>(5);
            borrowing.returnDate = result.get<std::string>(6, "");
            borrowing.status = result.get<std::string>(7);
            borrowings.push_back(borrowing);
        }
        
        log("Retrieved borrowings for MemberID " + std::to_string(memberId));
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Get member borrowings failed: ") + e.what());
    }
    
    return borrowings;
}

bool DBManager::returnBook(int borrowingId) {
    if (!isConnected()) return false;
    
    try {
        nanodbc::execute(*conn, "BEGIN TRANSACTION");
        
        // Get BookID first
        nanodbc::statement getStmt(*conn);
        prepare(getStmt, "SELECT BookID FROM Borrowings WHERE BorrowingID = ?");
        getStmt.bind(0, &borrowingId);
        nanodbc::result res = nanodbc::execute(getStmt);
        
        if (!res.next()) {
            nanodbc::execute(*conn, "ROLLBACK TRANSACTION");
            return false;
        }
        int bookId = res.get<int>(0);
        
        // Update borrowing
        nanodbc::statement stmt(*conn);
        prepare(stmt, "UPDATE Borrowings SET ReturnDate = GETDATE(), Status = 'Returned' "
                     "WHERE BorrowingID = ?");
        stmt.bind(0, &borrowingId);
        nanodbc::execute(stmt);
        
        // Update book availability
        nanodbc::statement updateStmt(*conn);
        prepare(updateStmt, "UPDATE Books SET AvailableCopies = AvailableCopies + 1, "
                           "UpdatedAt = GETDATE() WHERE BookID = ?");
        updateStmt.bind(0, &bookId);
        nanodbc::execute(updateStmt);
        
        nanodbc::execute(*conn, "COMMIT TRANSACTION");
        log("Book returned: BorrowingID " + std::to_string(borrowingId));
        return true;
    } catch (const nanodbc::database_error& e) {
        try {
            nanodbc::execute(*conn, "ROLLBACK TRANSACTION");
        } catch (...) {}
        logError(std::string("Return book failed: ") + e.what());
        return false;
    }
}

bool DBManager::markOverdueBooks() {
    if (!isConnected()) return false;
    
    try {
        nanodbc::execute(*conn, 
            "UPDATE Borrowings SET Status = 'Overdue' "
            "WHERE Status = 'Borrowed' AND DueDate < GETDATE() AND ReturnDate IS NULL");
        
        log("Marked overdue books");
        return true;
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Mark overdue books failed: ") + e.what());
        return false;
    }
}

// ============================================
// Reservation Operations
// ============================================
bool DBManager::createReservation(int bookId, int memberId) {
    if (!isConnected()) return false;
    
    try {
        nanodbc::statement stmt(*conn);
        prepare(stmt, "INSERT INTO Reservations (BookID, MemberID, ExpiryDate) "
                     "VALUES (?, ?, DATEADD(DAY, 7, GETDATE()))");
        
        stmt.bind(0, &bookId);
        stmt.bind(1, &memberId);
        
        nanodbc::execute(stmt);
        log("Reservation created: BookID " + std::to_string(bookId) + 
            ", MemberID " + std::to_string(memberId));
        return true;
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Create reservation failed: ") + e.what());
        return false;
    }
}

std::vector<Reservation> DBManager::getAllReservations() {
    std::vector<Reservation> reservations;
    if (!isConnected()) return reservations;
    
    try {
        nanodbc::result result = nanodbc::execute(*conn,
            "SELECT r.ReservationID, r.BookID, b.Title, r.MemberID, "
            "m.FirstName + ' ' + m.LastName AS MemberName, "
            "CONVERT(VARCHAR, r.ReservationDate, 120) AS ReservationDate, "
            "CONVERT(VARCHAR, r.ExpiryDate, 120) AS ExpiryDate, r.Status "
            "FROM Reservations r "
            "INNER JOIN Books b ON r.BookID = b.BookID "
            "INNER JOIN Members m ON r.MemberID = m.MemberID "
            "ORDER BY r.ReservationDate DESC");
        
        while (result.next()) {
            Reservation reservation;
            reservation.reservationId = result.get<int>(0);
            reservation.bookId = result.get<int>(1);
            reservation.bookTitle = result.get<std::string>(2);
            reservation.memberId = result.get<int>(3);
            reservation.memberName = result.get<std::string>(4);
            reservation.reservationDate = result.get<std::string>(5);
            reservation.expiryDate = result.get<std::string>(6, "");
            reservation.status = result.get<std::string>(7);
            reservations.push_back(reservation);
        }
        
        log("Retrieved " + std::to_string(reservations.size()) + " reservations");
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Get all reservations failed: ") + e.what());
    }
    
    return reservations;
}

bool DBManager::cancelReservation(int reservationId) {
    if (!isConnected()) return false;
    
    try {
        nanodbc::statement stmt(*conn);
        prepare(stmt, "UPDATE Reservations SET Status = 'Cancelled' WHERE ReservationID = ?");
        
        stmt.bind(0, &reservationId);
        nanodbc::execute(stmt);
        
        log("Reservation cancelled: ReservationID " + std::to_string(reservationId));
        return true;
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Cancel reservation failed: ") + e.what());
        return false;
    }
}

// ============================================
// Stored Procedure Calls
// ============================================
int DBManager::executeUpdateOverdueBooks() {
    if (!isConnected()) return -1;
    
    try {
        nanodbc::statement stmt(*conn);
        prepare(stmt, "EXEC UpdateOverdueBooks");
        nanodbc::result result = nanodbc::execute(stmt);
        
        if (result.next()) {
            int count = result.get<int>(0);
            log("Updated " + std::to_string(count) + " overdue books");
            return count;
        }
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Execute UpdateOverdueBooks failed: ") + e.what());
    }
    
    return -1;
}

int DBManager::executeCalculateOverdueFines(double dailyRate) {
    if (!isConnected()) return -1;
    
    try {
        nanodbc::statement stmt(*conn);
        prepare(stmt, "EXEC CalculateOverdueFines ?");
        
        stmt.bind(0, &dailyRate);
        nanodbc::result result = nanodbc::execute(stmt);
        
        if (result.next()) {
            int count = result.get<int>(0);
            log("Calculated fines for " + std::to_string(count) + " borrowings");
            return count;
        }
    } catch (const nanodbc::database_error& e) {
        logError(std::string("Execute CalculateOverdueFines failed: ") + e.what());
    }
    
    return -1;
}

std::string DBManager::getLastError() const {
    return "Check library_db.log for detailed error information";
}
