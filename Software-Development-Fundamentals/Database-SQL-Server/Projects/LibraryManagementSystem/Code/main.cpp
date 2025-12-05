// FILE: main.cpp
#include "DBManager.h"
#include "Book.h"
#include "Category.h"
#include "Member.h"
#include "Staff.h"
#include "Borrowing.h"
#include "Reservation.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <limits>

using namespace std;

// Connection string examples:
// DSN-less (replace SERVER, DATABASE, UID, PWD with your values):
// "Driver={ODBC Driver 17 for SQL Server};Server=localhost;Database=LibraryDB;UID=sa;PWD=YourPassword123;"
// "Driver={ODBC Driver 17 for SQL Server};Server=localhost\\SQLEXPRESS;Database=LibraryDB;Trusted_Connection=yes;"
//
// DSN-based (after creating a DSN named "LibraryDSN"):
// "DSN=LibraryDSN;UID=sa;PWD=YourPassword123;"

void displayMenu() {
    cout << "\n╔════════════════════════════════════════╗\n";
    cout << "║    LIBRARY MANAGEMENT SYSTEM - MENU    ║\n";
    cout << "╚════════════════════════════════════════╝\n";
    cout << "1.  Display All Books\n";
    cout << "2.  Display Available Books\n";
    cout << "3.  Search Books by Title\n";
    cout << "4.  Add New Book\n";
    cout << "5.  Display All Members\n";
    cout << "6.  Add New Member\n";
    cout << "7.  Display All Staff\n";
    cout << "8.  Display All Categories\n";
    cout << "9.  Create Borrowing\n";
    cout << "10. Display All Borrowings\n";
    cout << "11. Display Current Borrowings\n";
    cout << "12. Return Book\n";
    cout << "13. Create Reservation\n";
    cout << "14. Display All Reservations\n";
    cout << "15. Update Overdue Books\n";
    cout << "16. Calculate Overdue Fines\n";
    cout << "17. Test Connection\n";
    cout << "0.  Exit\n";
    cout << "════════════════════════════════════════\n";
    cout << "Enter choice: ";
}

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string getLine(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input;
}

int getInt(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            clearInput();
            return value;
        }
        cout << "Invalid input. Please enter a number.\n";
        clearInput();
    }
}

double getDouble(const string& prompt) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            clearInput();
            return value;
        }
        cout << "Invalid input. Please enter a number.\n";
        clearInput();
    }
}

void displayAllBooks(DBManager& db) {
    cout << "\n=== ALL BOOKS ===\n";
    vector<Book> books = db.getAllBooks();
    
    if (books.empty()) {
        cout << "No books found.\n";
        return;
    }
    
    cout << left << setw(5) << "ID" << setw(18) << "ISBN" 
         << setw(30) << "Title" << setw(20) << "Author" 
         << setw(15) << "Category" << setw(10) << "Available\n";
    cout << string(98, '-') << "\n";
    
    for (const auto& book : books) {
        cout << left << setw(5) << book.bookId 
             << setw(18) << book.isbn.substr(0, 17)
             << setw(30) << book.title.substr(0, 29)
             << setw(20) << book.author.substr(0, 19)
             << setw(15) << book.categoryName.substr(0, 14)
             << setw(10) << (to_string(book.availableCopies) + "/" + to_string(book.totalCopies))
             << "\n";
    }
    cout << "\nTotal: " << books.size() << " books\n";
}

void displayAvailableBooks(DBManager& db) {
    cout << "\n=== AVAILABLE BOOKS ===\n";
    vector<Book> books = db.getAvailableBooks();
    
    if (books.empty()) {
        cout << "No available books.\n";
        return;
    }
    
    for (const auto& book : books) {
        book.display();
    }
    cout << "\nTotal: " << books.size() << " available books\n";
}

void searchBooks(DBManager& db) {
    string title = getLine("Enter book title to search: ");
    
    cout << "\n=== SEARCH RESULTS ===\n";
    vector<Book> books = db.searchBooksByTitle(title);
    
    if (books.empty()) {
        cout << "No books found matching: " << title << "\n";
        return;
    }
    
    for (const auto& book : books) {
        book.display();
    }
    cout << "\nFound: " << books.size() << " books\n";
}

void addNewBook(DBManager& db) {
    cout << "\n=== ADD NEW BOOK ===\n";
    
    // Display categories first
    vector<Category> categories = db.getAllCategories();
    if (categories.empty()) {
        cout << "No categories available. Please add categories first.\n";
        return;
    }
    
    cout << "Available Categories:\n";
    for (const auto& cat : categories) {
        cout << cat.categoryId << ". " << cat.categoryName << "\n";
    }
    
    string isbn = getLine("ISBN: ");
    string title = getLine("Title: ");
    string author = getLine("Author: ");
    string publisher = getLine("Publisher: ");
    int year = getInt("Publication Year: ");
    int categoryId = getInt("Category ID: ");
    int totalCopies = getInt("Total Copies: ");
    double price = getDouble("Price: ");
    string shelfLocation = getLine("Shelf Location: ");
    
    if (db.createBook(isbn, title, author, publisher, year, categoryId, 
                      totalCopies, price, shelfLocation)) {
        cout << "✓ Book added successfully!\n";
    } else {
        cout << "✗ Failed to add book.\n";
    }
}

void displayAllMembers(DBManager& db) {
    cout << "\n=== ALL MEMBERS ===\n";
    vector<Member> members = db.getAllMembers();
    
    if (members.empty()) {
        cout << "No members found.\n";
        return;
    }
    
    cout << left << setw(5) << "ID" << setw(25) << "Name" 
         << setw(30) << "Email" << setw(15) << "Status\n";
    cout << string(75, '-') << "\n";
    
    for (const auto& member : members) {
        cout << left << setw(5) << member.memberId
             << setw(25) << member.getFullName().substr(0, 24)
             << setw(30) << member.email.substr(0, 29)
             << setw(15) << member.membershipStatus << "\n";
    }
    cout << "\nTotal: " << members.size() << " members\n";
}

void addNewMember(DBManager& db) {
    cout << "\n=== ADD NEW MEMBER ===\n";
    
    string firstName = getLine("First Name: ");
    string lastName = getLine("Last Name: ");
    string email = getLine("Email: ");
    string phone = getLine("Phone: ");
    string address = getLine("Address: ");
    
    if (db.createMember(firstName, lastName, email, phone, address)) {
        cout << "✓ Member added successfully!\n";
    } else {
        cout << "✗ Failed to add member.\n";
    }
}

void displayAllStaff(DBManager& db) {
    cout << "\n=== ALL STAFF ===\n";
    vector<Staff> staffList = db.getAllStaff();
    
    if (staffList.empty()) {
        cout << "No staff found.\n";
        return;
    }
    
    for (const auto& staff : staffList) {
        staff.display();
    }
    cout << "\nTotal: " << staffList.size() << " staff members\n";
}

void displayAllCategories(DBManager& db) {
    cout << "\n=== ALL CATEGORIES ===\n";
    vector<Category> categories = db.getAllCategories();
    
    if (categories.empty()) {
        cout << "No categories found.\n";
        return;
    }
    
    for (const auto& cat : categories) {
        cat.display();
    }
    cout << "\nTotal: " << categories.size() << " categories\n";
}

void createBorrowing(DBManager& db) {
    cout << "\n=== CREATE BORROWING ===\n";
    
    int bookId = getInt("Book ID: ");
    int memberId = getInt("Member ID: ");
    int staffId = getInt("Staff ID: ");
    string dueDate = getLine("Due Date (YYYY-MM-DD): ");
    
    if (db.createBorrowing(bookId, memberId, staffId, dueDate)) {
        cout << "✓ Borrowing created successfully!\n";
    } else {
        cout << "✗ Failed to create borrowing.\n";
    }
}

void displayAllBorrowings(DBManager& db) {
    cout << "\n=== ALL BORROWINGS ===\n";
    vector<Borrowing> borrowings = db.getAllBorrowings();
    
    if (borrowings.empty()) {
        cout << "No borrowings found.\n";
        return;
    }
    
    for (const auto& borrowing : borrowings) {
        borrowing.display();
    }
    cout << "\nTotal: " << borrowings.size() << " borrowings\n";
}

void displayCurrentBorrowings(DBManager& db) {
    cout << "\n=== CURRENT BORROWINGS ===\n";
    vector<Borrowing> borrowings = db.getCurrentBorrowings();
    
    if (borrowings.empty()) {
        cout << "No current borrowings.\n";
        return;
    }
    
    cout << left << setw(5) << "ID" << setw(25) << "Member" 
         << setw(30) << "Book" << setw(12) << "Due Date" 
         << setw(10) << "Status\n";
    cout << string(82, '-') << "\n";
    
    for (const auto& borrowing : borrowings) {
        cout << left << setw(5) << borrowing.borrowingId
             << setw(25) << borrowing.memberName.substr(0, 24)
             << setw(30) << borrowing.bookTitle.substr(0, 29)
             << setw(12) << borrowing.dueDate.substr(0, 10)
             << setw(10) << borrowing.status << "\n";
    }
    cout << "\nTotal: " << borrowings.size() << " current borrowings\n";
}

void returnBook(DBManager& db) {
    cout << "\n=== RETURN BOOK ===\n";
    
    int borrowingId = getInt("Borrowing ID: ");
    
    if (db.returnBook(borrowingId)) {
        cout << "✓ Book returned successfully!\n";
    } else {
        cout << "✗ Failed to return book.\n";
    }
}

void createReservation(DBManager& db) {
    cout << "\n=== CREATE RESERVATION ===\n";
    
    int bookId = getInt("Book ID: ");
    int memberId = getInt("Member ID: ");
    
    if (db.createReservation(bookId, memberId)) {
        cout << "✓ Reservation created successfully!\n";
    } else {
        cout << "✗ Failed to create reservation.\n";
    }
}

void displayAllReservations(DBManager& db) {
    cout << "\n=== ALL RESERVATIONS ===\n";
    vector<Reservation> reservations = db.getAllReservations();
    
    if (reservations.empty()) {
        cout << "No reservations found.\n";
        return;
    }
    
    for (const auto& reservation : reservations) {
        reservation.display();
    }
    cout << "\nTotal: " << reservations.size() << " reservations\n";
}

void updateOverdueBooks(DBManager& db) {
    cout << "\n=== UPDATE OVERDUE BOOKS ===\n";
    
    int count = db.executeUpdateOverdueBooks();
    if (count >= 0) {
        cout << "✓ Updated " << count << " overdue books.\n";
    } else {
        cout << "✗ Failed to update overdue books.\n";
    }
}

void calculateOverdueFines(DBManager& db) {
    cout << "\n=== CALCULATE OVERDUE FINES ===\n";
    
    double dailyRate = getDouble("Daily Fine Rate (default 1.00): ");
    
    int count = db.executeCalculateOverdueFines(dailyRate);
    if (count >= 0) {
        cout << "✓ Calculated fines for " << count << " borrowings.\n";
    } else {
        cout << "✗ Failed to calculate fines.\n";
    }
}

void testConnection(DBManager& db) {
    cout << "\n=== TEST CONNECTION ===\n";
    
    if (db.testConnection()) {
        cout << "✓ Connection is active and working!\n";
    } else {
        cout << "✗ Connection test failed.\n";
    }
}

int main() {
    cout << "╔═════════════════════════════════════════════╗\n";
    cout << "║   LIBRARY MANAGEMENT SYSTEM - SQL SERVER   ║\n";
    cout << "╚═════════════════════════════════════════════╝\n\n";
    
    DBManager db;
    
    // Connection string - MODIFY THIS FOR YOUR ENVIRONMENT
    // Option 1: DSN-less with SQL Server Authentication
    /*string connStr = "Driver={ODBC Driver 17 for SQL Server};"
                     "Server=localhost;"
                     "Database=LibraryDB;"
                     "UID=sa;"
                     "PWD=YourPassword123;";
    */
    // Option 2: DSN-less with Windows Authentication (Trusted Connection)
    string connStr = "Driver={ODBC Driver 17 for SQL Server};"
                      "Server=localhost;"
                      "Database=LibraryDB;"
                      "Trusted_Connection=yes;";
    
    // Option 3: DSN-based (after creating a DSN named "LibraryDSN")
    // string connStr = "DSN=LibraryDSN;UID=sa;PWD=YourPassword123;";
    
    cout << "Connecting to SQL Server...\n";
    if (!db.connect(connStr)) {
        cerr << "Failed to connect to database.\n";
        cerr << "Please check:\n";
        cerr << "  1. SQL Server is running\n";
        cerr << "  2. Database 'LibraryDB' exists\n";
        cerr << "  3. Connection string is correct\n";
        cerr << "  4. ODBC Driver 17 for SQL Server is installed\n";
        return 1;
    }
    
    cout << "✓ Connected successfully!\n";
    
    int choice;
    do {
        displayMenu();
        
        if (!(cin >> choice)) {
            clearInput();
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        clearInput();
        
        try {
            switch (choice) {
                case 1: displayAllBooks(db); break;
                case 2: displayAvailableBooks(db); break;
                case 3: searchBooks(db); break;
                case 4: addNewBook(db); break;
                case 5: displayAllMembers(db); break;
                case 6: addNewMember(db); break;
                case 7: displayAllStaff(db); break;
                case 8: displayAllCategories(db); break;
                case 9: createBorrowing(db); break;
                case 10: displayAllBorrowings(db); break;
                case 11: displayCurrentBorrowings(db); break;
                case 12: returnBook(db); break;
                case 13: createReservation(db); break;
                case 14: displayAllReservations(db); break;
                case 15: updateOverdueBooks(db); break;
                case 16: calculateOverdueFines(db); break;
                case 17: testConnection(db); break;
                case 0: cout << "\nExiting... Goodbye!\n"; break;
                default: cout << "Invalid choice. Try again.\n";
            }
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << "\n";
        }
        
        if (choice != 0) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
        
    } while (choice != 0);
    
    return 0;
}