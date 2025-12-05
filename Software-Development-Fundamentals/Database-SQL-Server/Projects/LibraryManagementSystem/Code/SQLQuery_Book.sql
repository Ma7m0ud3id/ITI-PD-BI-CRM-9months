-- FILE: database_sqlserver.sql
-- Microsoft SQL Server T-SQL Script for Library Management System
-- Compatible with SQL Server 2016+

-- Drop database if exists and create fresh
IF DB_ID('LibraryDB') IS NOT NULL
BEGIN
    ALTER DATABASE LibraryDB SET SINGLE_USER WITH ROLLBACK IMMEDIATE;
    DROP DATABASE LibraryDB;
END
GO

CREATE DATABASE LibraryDB;
GO

USE LibraryDB;
GO

-- =============================================
-- Table: Categories
-- =============================================
CREATE TABLE Categories (
    CategoryID INT IDENTITY(1,1) PRIMARY KEY,
    CategoryName NVARCHAR(100) NOT NULL UNIQUE,
    Description NVARCHAR(500),
    CreatedAt DATETIME2 DEFAULT GETDATE()
);
GO

-- =============================================
-- Table: Books
-- =============================================
CREATE TABLE Books (
    BookID INT IDENTITY(1,1) PRIMARY KEY,
    ISBN NVARCHAR(20) UNIQUE NOT NULL,
    Title NVARCHAR(255) NOT NULL,
    Author NVARCHAR(255) NOT NULL,
    Publisher NVARCHAR(255),
    PublicationYear INT CHECK (PublicationYear >= 1000 AND PublicationYear <= YEAR(GETDATE()) + 1),
    CategoryID INT NOT NULL,
    TotalCopies INT DEFAULT 1 CHECK (TotalCopies >= 0),
    AvailableCopies INT DEFAULT 1 CHECK (AvailableCopies >= 0),
    Price DECIMAL(10,2) CHECK (Price >= 0),
    ShelfLocation NVARCHAR(50),
    CreatedAt DATETIME2 DEFAULT GETDATE(),
    UpdatedAt DATETIME2 DEFAULT GETDATE(),
    CONSTRAINT FK_Books_Categories FOREIGN KEY (CategoryID) REFERENCES Categories(CategoryID) ON DELETE CASCADE,
    CONSTRAINT CHK_AvailableCopies CHECK (AvailableCopies <= TotalCopies)
);
GO

-- =============================================
-- Table: Members
-- =============================================
CREATE TABLE Members (
    MemberID INT IDENTITY(1,1) PRIMARY KEY,
    FirstName NVARCHAR(100) NOT NULL,
    LastName NVARCHAR(100) NOT NULL,
    Email NVARCHAR(255) UNIQUE NOT NULL,
    Phone NVARCHAR(20),
    Address NVARCHAR(500),
    MembershipDate DATE DEFAULT CAST(GETDATE() AS DATE),
    MembershipStatus NVARCHAR(20) DEFAULT 'Active' CHECK (MembershipStatus IN ('Active', 'Suspended', 'Expired')),
    CreatedAt DATETIME2 DEFAULT GETDATE()
);
GO

-- =============================================
-- Table: Staff
-- =============================================
CREATE TABLE Staff (
    StaffID INT IDENTITY(1,1) PRIMARY KEY,
    FirstName NVARCHAR(100) NOT NULL,
    LastName NVARCHAR(100) NOT NULL,
    Email NVARCHAR(255) UNIQUE NOT NULL,
    Phone NVARCHAR(20),
    Position NVARCHAR(100),
    HireDate DATE DEFAULT CAST(GETDATE() AS DATE),
    Salary DECIMAL(10,2) CHECK (Salary >= 0),
    CreatedAt DATETIME2 DEFAULT GETDATE()
);
GO

-- =============================================
-- Table: Borrowings
-- =============================================
CREATE TABLE Borrowings (
    BorrowingID INT IDENTITY(1,1) PRIMARY KEY,
    BookID INT NOT NULL,
    MemberID INT NOT NULL,
    StaffID INT,
    BorrowDate DATETIME2 DEFAULT GETDATE(),
    DueDate DATETIME2 NOT NULL,
    ReturnDate DATETIME2,
    Status NVARCHAR(20) DEFAULT 'Borrowed' CHECK (Status IN ('Borrowed', 'Returned', 'Overdue', 'Lost')),
    CONSTRAINT FK_Borrowings_Books FOREIGN KEY (BookID) REFERENCES Books(BookID) ON DELETE CASCADE,
    CONSTRAINT FK_Borrowings_Members FOREIGN KEY (MemberID) REFERENCES Members(MemberID) ON DELETE CASCADE,
    CONSTRAINT FK_Borrowings_Staff FOREIGN KEY (StaffID) REFERENCES Staff(StaffID)
);
GO

-- =============================================
-- Table: Reservations
-- =============================================
CREATE TABLE Reservations (
    ReservationID INT IDENTITY(1,1) PRIMARY KEY,
    BookID INT NOT NULL,
    MemberID INT NOT NULL,
    ReservationDate DATETIME2 DEFAULT GETDATE(),
    Status NVARCHAR(20) DEFAULT 'Pending' CHECK (Status IN ('Pending', 'Fulfilled', 'Cancelled', 'Expired')),
    ExpiryDate DATETIME2,
    CONSTRAINT FK_Reservations_Books FOREIGN KEY (BookID) REFERENCES Books(BookID) ON DELETE CASCADE,
    CONSTRAINT FK_Reservations_Members FOREIGN KEY (MemberID) REFERENCES Members(MemberID) ON DELETE CASCADE
);
GO

-- =============================================
-- Table: Penalties
-- =============================================
CREATE TABLE Penalties (
    PenaltyID INT IDENTITY(1,1) PRIMARY KEY,
    BorrowingID INT NOT NULL,
    MemberID INT NOT NULL,
    Amount DECIMAL(10,2) NOT NULL CHECK (Amount >= 0),
    Reason NVARCHAR(255),
    IssueDate DATETIME2 DEFAULT GETDATE(),
    PaidDate DATETIME2,
    Status NVARCHAR(20) DEFAULT 'Unpaid' CHECK (Status IN ('Unpaid', 'Paid', 'Waived')),
    CONSTRAINT FK_Penalties_Borrowings FOREIGN KEY (BorrowingID) REFERENCES Borrowings(BorrowingID),
    CONSTRAINT FK_Penalties_Members FOREIGN KEY (MemberID) REFERENCES Members(MemberID) ON DELETE CASCADE
);
GO

-- =============================================
-- Table: ActivityLogs
-- =============================================
CREATE TABLE ActivityLogs (
    LogID INT IDENTITY(1,1) PRIMARY KEY,
    TableName NVARCHAR(100) NOT NULL,
    Action NVARCHAR(50) NOT NULL,
    RecordID INT,
    PerformedBy NVARCHAR(100),
    Details NVARCHAR(MAX),
    Timestamp DATETIME2 DEFAULT GETDATE()
);
GO

-- =============================================
-- INDEXES for Performance
-- =============================================
CREATE INDEX IDX_Books_CategoryID ON Books(CategoryID);
CREATE INDEX IDX_Books_ISBN ON Books(ISBN);
CREATE INDEX IDX_Borrowings_MemberID ON Borrowings(MemberID);
CREATE INDEX IDX_Borrowings_BookID ON Borrowings(BookID);
CREATE INDEX IDX_Borrowings_Status ON Borrowings(Status);
CREATE INDEX IDX_Members_Email ON Members(Email);
CREATE INDEX IDX_Reservations_MemberID ON Reservations(MemberID);
CREATE INDEX IDX_Reservations_BookID ON Reservations(BookID);
GO

-- =============================================
-- VIEW: AvailableBooks
-- =============================================
CREATE VIEW AvailableBooks AS
SELECT 
    b.BookID,
    b.ISBN,
    b.Title,
    b.Author,
    b.Publisher,
    b.PublicationYear,
    c.CategoryName,
    b.AvailableCopies,
    b.TotalCopies,
    b.Price,
    b.ShelfLocation
FROM Books b
INNER JOIN Categories c ON b.CategoryID = c.CategoryID
WHERE b.AvailableCopies > 0;
GO

-- =============================================
-- VIEW: CurrentBorrowings
-- =============================================
CREATE VIEW CurrentBorrowings AS
SELECT 
    br.BorrowingID,
    m.FirstName + ' ' + m.LastName AS MemberName,
    m.Email,
    b.Title AS BookTitle,
    b.ISBN,
    br.BorrowDate,
    br.DueDate,
    DATEDIFF(DAY, br.DueDate, GETDATE()) AS DaysOverdue,
    br.Status
FROM Borrowings br
INNER JOIN Members m ON br.MemberID = m.MemberID
INNER JOIN Books b ON br.BookID = b.BookID
WHERE br.Status IN ('Borrowed', 'Overdue');
GO

-- =============================================
-- VIEW: MemberStatistics
-- =============================================
CREATE VIEW MemberStatistics AS
SELECT 
    m.MemberID,
    m.FirstName + ' ' + m.LastName AS MemberName,
    m.Email,
    m.MembershipStatus,
    COUNT(DISTINCT br.BorrowingID) AS TotalBorrowings,
    SUM(CASE WHEN br.Status = 'Borrowed' THEN 1 ELSE 0 END) AS CurrentBorrowings,
    SUM(CASE WHEN br.Status = 'Overdue' THEN 1 ELSE 0 END) AS OverdueBooks,
    COALESCE(SUM(p.Amount), 0) AS TotalPenalties,
    COALESCE(SUM(CASE WHEN p.Status = 'Unpaid' THEN p.Amount ELSE 0 END), 0) AS UnpaidPenalties
FROM Members m
LEFT JOIN Borrowings br ON m.MemberID = br.MemberID
LEFT JOIN Penalties p ON m.MemberID = p.MemberID
GROUP BY m.MemberID, m.FirstName, m.LastName, m.Email, m.MembershipStatus;
GO

-- =============================================
-- STORED PROCEDURE: UpdateOverdueBooks
-- =============================================
CREATE PROCEDURE UpdateOverdueBooks
AS
BEGIN
    SET NOCOUNT ON;
    
    UPDATE Borrowings
    SET Status = 'Overdue'
    WHERE Status = 'Borrowed' 
      AND DueDate < GETDATE() 
      AND ReturnDate IS NULL;
    
    DECLARE @RowsAffected INT = @@ROWCOUNT;
    
    INSERT INTO ActivityLogs (TableName, Action, Details)
    VALUES ('Borrowings', 'UpdateOverdue', CONCAT(@RowsAffected, ' borrowings marked as overdue'));
    
    SELECT @RowsAffected AS OverdueBooksCount;
END;
GO

-- =============================================
-- STORED PROCEDURE: CalculateOverdueFines
-- =============================================
CREATE PROCEDURE CalculateOverdueFines
    @DailyFineRate DECIMAL(10,2) = 1.00
AS
BEGIN
    SET NOCOUNT ON;
    
    DECLARE @NewFinesCount INT = 0;
    
    -- Insert penalties for overdue books that don't have penalties yet
    INSERT INTO Penalties (BorrowingID, MemberID, Amount, Reason, Status)
    SELECT 
        br.BorrowingID,
        br.MemberID,
        DATEDIFF(DAY, br.DueDate, GETDATE()) * @DailyFineRate AS Amount,
        'Overdue fine - ' + CAST(DATEDIFF(DAY, br.DueDate, GETDATE()) AS NVARCHAR) + ' days late',
        'Unpaid'
    FROM Borrowings br
    WHERE br.Status = 'Overdue'
      AND br.ReturnDate IS NULL
      AND NOT EXISTS (
          SELECT 1 FROM Penalties p 
          WHERE p.BorrowingID = br.BorrowingID 
            AND p.Status = 'Unpaid'
      );
    
    SET @NewFinesCount = @@ROWCOUNT;
    
    -- Update existing unpaid penalties with new amounts
    UPDATE p
    SET p.Amount = DATEDIFF(DAY, br.DueDate, GETDATE()) * @DailyFineRate,
        p.Reason = 'Overdue fine - ' + CAST(DATEDIFF(DAY, br.DueDate, GETDATE()) AS NVARCHAR) + ' days late'
    FROM Penalties p
    INNER JOIN Borrowings br ON p.BorrowingID = br.BorrowingID
    WHERE br.Status = 'Overdue' 
      AND br.ReturnDate IS NULL 
      AND p.Status = 'Unpaid';
    
    INSERT INTO ActivityLogs (TableName, Action, Details)
    VALUES ('Penalties', 'CalculateFines', CONCAT(@NewFinesCount, ' new fines calculated'));
    
    SELECT @NewFinesCount AS NewFinesCreated;
END;
GO

-- =============================================
-- STORED PROCEDURE: GetMemberBorrowings
-- =============================================
CREATE PROCEDURE GetMemberBorrowings
    @MemberID INT
AS
BEGIN
    SET NOCOUNT ON;
    
    SELECT 
        br.BorrowingID,
        b.Title,
        b.ISBN,
        b.Author,
        br.BorrowDate,
        br.DueDate,
        br.ReturnDate,
        br.Status,
        CASE 
            WHEN br.ReturnDate IS NULL AND br.DueDate < GETDATE() 
            THEN DATEDIFF(DAY, br.DueDate, GETDATE())
            ELSE 0 
        END AS DaysOverdue,
        COALESCE(p.Amount, 0) AS PenaltyAmount,
        COALESCE(p.Status, 'None') AS PenaltyStatus
    FROM Borrowings br
    INNER JOIN Books b ON br.BookID = b.BookID
    LEFT JOIN Penalties p ON br.BorrowingID = p.BorrowingID AND p.Status = 'Unpaid'
    WHERE br.MemberID = @MemberID
    ORDER BY br.BorrowDate DESC;
END;
GO

-- =============================================
-- SAMPLE DATA
-- =============================================

-- Insert Categories
INSERT INTO Categories (CategoryName, Description) VALUES
('Fiction', 'Fictional literature including novels and short stories'),
('Science', 'Scientific books covering various disciplines'),
('History', 'Historical books and biographies'),
('Technology', 'Computer science, engineering, and technology'),
('Arts', 'Books about art, music, and culture');
GO

-- Insert Books
INSERT INTO Books (ISBN, Title, Author, Publisher, PublicationYear, CategoryID, TotalCopies, AvailableCopies, Price, ShelfLocation) VALUES
('978-0-06-112008-4', 'To Kill a Mockingbird', 'Harper Lee', 'J.B. Lippincott & Co.', 1960, 1, 3, 2, 15.99, 'A-101'),
('978-0-7432-7356-5', '1984', 'George Orwell', 'Secker & Warburg', 1949, 1, 5, 4, 14.99, 'A-102'),
('978-0-14-303943-3', 'A Brief History of Time', 'Stephen Hawking', 'Bantam Books', 1988, 2, 2, 2, 22.99, 'B-201'),
('978-0-307-58837-1', 'Sapiens', 'Yuval Noah Harari', 'Harper', 2011, 3, 4, 3, 24.99, 'C-301'),
('978-0-201-63361-0', 'The Pragmatic Programmer', 'Andrew Hunt, David Thomas', 'Addison-Wesley', 1999, 4, 3, 3, 49.99, 'D-401'),
('978-0-452-28423-4', 'The Great Gatsby', 'F. Scott Fitzgerald', 'Scribner', 1925, 1, 4, 3, 12.99, 'A-103'),
('978-0-316-76948-0', 'The Catcher in the Rye', 'J.D. Salinger', 'Little, Brown', 1951, 1, 2, 1, 13.99, 'A-104');
GO

-- Insert Members
INSERT INTO Members (FirstName, LastName, Email, Phone, Address, MembershipStatus) VALUES
('Ahmed', 'Hassan', 'ahmed.hassan@email.com', '01012345678', '123 Cairo Street, Cairo', 'Active'),
('Fatima', 'Ali', 'fatima.ali@email.com', '01098765432', '456 Alexandria Ave, Alexandria', 'Active'),
('Mohamed', 'Ibrahim', 'mohamed.ibrahim@email.com', '01123456789', '789 Giza Road, Giza', 'Active');
GO

-- Insert Staff
INSERT INTO Staff (FirstName, LastName, Email, Phone, Position, Salary) VALUES
('Sarah', 'Ahmed', 'sarah.ahmed@library.com', '01011111111', 'Librarian', 5000.00),
('Khaled', 'Mahmoud', 'khaled.mahmoud@library.com', '01022222222', 'Assistant Librarian', 4000.00);
GO

-- Insert Borrowings
DECLARE @BookID1 INT = (SELECT BookID FROM Books WHERE ISBN = '978-0-06-112008-4');
DECLARE @BookID2 INT = (SELECT BookID FROM Books WHERE ISBN = '978-0-7432-7356-5');
DECLARE @BookID3 INT = (SELECT BookID FROM Books WHERE ISBN = '978-0-316-76948-0');
DECLARE @Member1 INT = (SELECT MemberID FROM Members WHERE Email = 'ahmed.hassan@email.com');
DECLARE @Member2 INT = (SELECT MemberID FROM Members WHERE Email = 'fatima.ali@email.com');
DECLARE @Staff1 INT = (SELECT StaffID FROM Staff WHERE Email = 'sarah.ahmed@library.com');

INSERT INTO Borrowings (BookID, MemberID, StaffID, BorrowDate, DueDate, Status) VALUES
(@BookID1, @Member1, @Staff1, DATEADD(DAY, -20, GETDATE()), DATEADD(DAY, -6, GETDATE()), 'Borrowed'),
(@BookID2, @Member2, @Staff1, DATEADD(DAY, -10, GETDATE()), DATEADD(DAY, 4, GETDATE()), 'Borrowed'),
(@BookID3, @Member1, @Staff1, DATEADD(DAY, -35, GETDATE()), DATEADD(DAY, -21, GETDATE()), 'Overdue');

-- Update available copies
UPDATE Books SET AvailableCopies = AvailableCopies - 1 WHERE BookID IN (@BookID1, @BookID2, @BookID3);
GO

-- Insert Reservations
DECLARE @BookID4 INT = (SELECT BookID FROM Books WHERE ISBN = '978-0-452-28423-4');
DECLARE @Member3 INT = (SELECT MemberID FROM Members WHERE Email = 'mohamed.ibrahim@email.com');

INSERT INTO Reservations (BookID, MemberID, Status, ExpiryDate) VALUES
(@BookID4, @Member3, 'Pending', DATEADD(DAY, 7, GETDATE()));
GO

-- Insert Sample Penalty (for overdue book)
DECLARE @BorrowingOverdue INT = (SELECT TOP 1 BorrowingID FROM Borrowings WHERE Status = 'Overdue');
DECLARE @MemberOverdue INT = (SELECT MemberID FROM Borrowings WHERE BorrowingID = @BorrowingOverdue);

IF @BorrowingOverdue IS NOT NULL
BEGIN
    INSERT INTO Penalties (BorrowingID, MemberID, Amount, Reason, Status) VALUES
    (@BorrowingOverdue, @MemberOverdue, 14.00, 'Overdue fine - 14 days late', 'Unpaid');
END
GO

-- Insert Activity Log Sample
INSERT INTO ActivityLogs (TableName, Action, RecordID, PerformedBy, Details) VALUES
('Books', 'INSERT', 1, 'System', 'Initial book data loaded'),
('Members', 'INSERT', 1, 'System', 'Initial member data loaded');
GO

-- =============================================
-- Verification Queries (FIXED)
-- =============================================
GO

-- Declare variables to hold the counts
DECLARE @TotalCategories INT = (SELECT COUNT(*) FROM Categories);
DECLARE @TotalBooks INT = (SELECT COUNT(*) FROM Books);
DECLARE @TotalMembers INT = (SELECT COUNT(*) FROM Members);
DECLARE @TotalStaff INT = (SELECT COUNT(*) FROM Staff);
DECLARE @TotalBorrowings INT = (SELECT COUNT(*) FROM Borrowings);
DECLARE @TotalReservations INT = (SELECT COUNT(*) FROM Reservations);
DECLARE @TotalPenalties INT = (SELECT COUNT(*) FROM Penalties);

PRINT '=== Database Setup Complete ===';
-- Now use the variables in the PRINT statement
PRINT 'Total Categories: ' + CAST(@TotalCategories AS NVARCHAR);
PRINT 'Total Books: ' + CAST(@TotalBooks AS NVARCHAR);
PRINT 'Total Members: ' + CAST(@TotalMembers AS NVARCHAR);
PRINT 'Total Staff: ' + CAST(@TotalStaff AS NVARCHAR);
PRINT 'Total Borrowings: ' + CAST(@TotalBorrowings AS NVARCHAR);
PRINT 'Total Reservations: ' + CAST(@TotalReservations AS NVARCHAR);
PRINT 'Total Penalties: ' + CAST(@TotalPenalties AS NVARCHAR);
GO

----------------------------------------------------------->>


--  ‘€Ì· ﬂÊœ «·«Œ »«— œ«Œ· ﬁ«⁄œ… «·»Ì«‰«  LibraryDB
USE LibraryDB;
GO

PRINT '==================================================';
PRINT 'STEP 1: «·≈Õ’«∆Ì«  «·√Ê·Ì… ··√⁄÷«¡ ﬁ»· «· ÕœÌÀ';
PRINT '==================================================';

-- ⁄—÷ Õ«·… «·√⁄÷«¡ ﬁ»·  ‘€Ì· ≈Ã—«¡«  «· ÕœÌÀ
SELECT * FROM MemberStatistics
ORDER BY MemberID;
GO
/*
-----------------------------------------------------------
-- 1.  ‘€Ì· ≈Ã—«¡  ÕœÌÀ «·ﬂ » «·„ √Œ—…
-----------------------------------------------------------
PRINT '==================================================';
PRINT 'STEP 2:  ‘€Ì· UpdateOverdueBooks';
PRINT '==================================================';

-- «·≈” ⁄«—… «·√Ê·Ï (BookID1) ·Â«  «—ÌŒ «” Õﬁ«ﬁ DueDate ›Ì «·„«÷Ì ÊÂÌ Õ«·Ì« 'Borrowed'.
-- «·≈” ⁄«—… «·À«‰Ì… (BookID2) ·Â«  «—ÌŒ «” Õﬁ«ﬁ DueDate ›Ì «·„” ﬁ»· ÊÂÌ Õ«·Ì« 'Borrowed'.
-- «·≈” ⁄«—… «·À«·À… (BookID3)  „ ≈œŒ«·Â« „ı”»ﬁ« ﬂ‹ 'Overdue'.

EXEC UpdateOverdueBooks;
GO

-- ⁄—÷ ⁄„·Ì«  «·≈” ⁄«—… «·Õ«·Ì… ·· Õﬁﬁ
SELECT * FROM CurrentBorrowings;
GO

-----------------------------------------------------------
-- 2.  ‘€Ì· ≈Ã—«¡ Õ”«» «·€—«„« 
-----------------------------------------------------------
PRINT '==================================================';
PRINT 'STEP 3:  ‘€Ì· CalculateOverdueFines (Rate = 1.00)';
PRINT '==================================================';

-- ”ÌﬁÊ„ Â–« «·≈Ã—«¡ »„« Ì·Ì:
-- 1.  ÕœÌÀ «·€—«„… «·Õ«·Ì… ··«” ⁄«—… —ﬁ„ 3 («· Ì ﬂ«‰  'Overdue' „ı”»ﬁ«) »‰«¡ ⁄·Ï «·√Ì«„ «·›⁄·Ì… ·· √ŒÌ—.
-- 2. ≈œ—«Ã €—«„… ÃœÌœ… ··«” ⁄«—… —ﬁ„ 1 («· Ì √’»Õ  'Overdue' ·· Ê ›Ì «·ŒÿÊ… 2).

EXEC CalculateOverdueFines @DailyFineRate = 1.00;
GO

-----------------------------------------------------------
-- 3. «· Õﬁﬁ „‰  ›«’Ì· «” ⁄«—… ⁄÷Ê „Õœœ
-----------------------------------------------------------
PRINT '==================================================';
PRINT 'STEP 4: «” ⁄—«÷ ”Ã· «” ⁄«—«  «·⁄÷Ê —ﬁ„ 1 (√Õ„œ Õ”‰)';
PRINT '==================================================';

-- «·⁄÷Ê —ﬁ„ 1 (√Õ„œ Õ”‰) ÂÊ „‰ «” ⁄«— «·ﬂ «»Ì‰ «·„ √Œ—Ì‰ (1 Ê 3)
EXEC GetMemberBorrowings @MemberID = 1;
GO

-----------------------------------------------------------
-- 4. «·≈Õ’«∆Ì«  «·‰Â«∆Ì… ··√⁄÷«¡
-----------------------------------------------------------
PRINT '==================================================';
PRINT 'STEP 5: «·≈Õ’«∆Ì«  «·‰Â«∆Ì… ··√⁄÷«¡ »⁄œ «· ÕœÌÀ ÊÕ”«» «·€—«„« ';
PRINT '==================================================';

-- ÌÃ» √‰ ‰—Ï  ÕœÌÀ«  ›Ì Œ«‰«  OverdueBooks Ê UnpaidPenalties
SELECT * FROM MemberStatistics
ORDER BY MemberID;
GO

-- «· Õﬁﬁ «·‰Â«∆Ì „‰ ”Ã· «·√‰‘ÿ…
PRINT '==================================================';
PRINT 'STEP 6: ”Ã·«  «·‰‘«ÿ (Activity Logs)';
PRINT '==================================================';

SELECT TOP 5 * FROM ActivityLogs
ORDER BY Timestamp DESC;
GO*/