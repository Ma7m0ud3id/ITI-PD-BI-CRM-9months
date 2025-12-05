╔═══════════════════════════════════════════════════════════════════════════╗
║     LIBRARY MANAGEMENT SYSTEM - BUILD & RUN INSTRUCTIONS (SQL SERVER)     ║
╚═══════════════════════════════════════════════════════════════════════════╝

This guide walks you through setting up and running the Library Management 
System with Microsoft SQL Server and nanodbc on Windows/Linux.

═══════════════════════════════════════════════════════════════════════════
SECTION 1: PREREQUISITES
═══════════════════════════════════════════════════════════════════════════

1.1 Microsoft SQL Server
   - Download and install SQL Server 2016+ (Express Edition is free):
     https://www.microsoft.com/en-us/sql-server/sql-server-downloads
   
   - Or use SQL Server Developer Edition (free, full-featured):
     https://go.microsoft.com/fwlink/?linkid=866662
   
   - For Linux:
     https://docs.microsoft.com/en-us/sql/linux/sql-server-linux-setup

1.2 SQL Server Management Studio (SSMS) - RECOMMENDED
   - Download SSMS (Windows only):
     https://aka.ms/ssmsfullsetup
   
   - Alternative for command-line: sqlcmd (included with SQL Server)

1.3 Microsoft ODBC Driver for SQL Server
   
   WINDOWS:
   - Download "ODBC Driver 17 for SQL Server" or "ODBC Driver 18":
     https://docs.microsoft.com/en-us/sql/connect/odbc/download-odbc-driver-for-sql-server
   - Run the installer (msodbcsql.msi)
   - Verify installation:
     Open "ODBC Data Sources (64-bit)" from Windows Control Panel
     Go to "Drivers" tab and look for "ODBC Driver 17 for SQL Server"
   
   LINUX (Ubuntu/Debian):
   sudo su
   curl https://packages.microsoft.com/keys/microsoft.asc | apt-key add -
   curl https://packages.microsoft.com/config/ubuntu/$(lsb_release -rs)/prod.list > /etc/apt/sources.list.d/mssql-release.list
   exit
   sudo apt-get update
   sudo ACCEPT_EULA=Y apt-get install -y msodbcsql17
   sudo apt-get install -y unixodbc-dev
   
   LINUX (RHEL/CentOS):
   sudo su
   curl https://packages.microsoft.com/config/rhel/8/prod.repo > /etc/yum.repos.d/mssql-release.repo
   exit
   sudo yum remove unixODBC-utf16 unixODBC-utf16-devel
   sudo ACCEPT_EULA=Y yum install -y msodbcsql17
   sudo yum install -y unixODBC-devel

1.4 C++ Compiler
   
   WINDOWS:
   - Visual Studio 2019/2022 (Community Edition is free):
     https://visualstudio.microsoft.com/downloads/
     Install "Desktop development with C++" workload
   - OR MinGW-w64: https://www.mingw-w64.org/downloads/
   
   LINUX:
   sudo apt-get install -y build-essential g++ cmake

1.5 nanodbc Library
   
   OPTION A: Using vcpkg (RECOMMENDED for Windows/Linux)
   
   1. Install vcpkg:
      git clone https://github.com/Microsoft/vcpkg.git
      cd vcpkg
      # Windows:
      .\bootstrap-vcpkg.bat
      # Linux:
      ./bootstrap-vcpkg.sh
   
   2. Install nanodbc:
      # Windows:
      .\vcpkg install nanodbc:x64-windows
      # Linux:
      ./vcpkg install nanodbc
   
   3. Integrate with your system:
      # Windows:
      .\vcpkg integrate install
      # Linux:
      ./vcpkg integrate install
   
   OPTION B: Build from source
   
   git clone https://github.com/nanodbc/nanodbc.git
   cd nanodbc
   mkdir build && cd build
   cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
   cmake --build . --config Release
   sudo cmake --install .

═══════════════════════════════════════════════════════════════════════════
SECTION 2: DATABASE SETUP
═══════════════════════════════════════════════════════════════════════════

2.1 Import Database Schema

   METHOD 1: Using SQL Server Management Studio (SSMS) - EASIEST
   
   1. Open SSMS
   2. Connect to your SQL Server instance:
      - Server name: localhost or localhost\SQLEXPRESS
      - Authentication: Windows Authentication or SQL Server Authentication
   3. Click "File" → "Open" → "File..."
   4. Select "database_sqlserver.sql"
   5. Press F5 or click "Execute" button
   6. Verify success: You should see "Database Setup Complete" message
   7. Refresh "Databases" in Object Explorer to see "LibraryDB"
   
   METHOD 2: Using sqlcmd (Command Line)
   
   # Windows:
   sqlcmd -S localhost -U sa -P YourPassword123 -i database_sqlserver.sql
   
   # Windows with Windows Authentication:
   sqlcmd -S localhost\SQLEXPRESS -E -i database_sqlserver.sql
   
   # Linux:
   sqlcmd -S localhost -U sa -P 'YourPassword123' -i database_sqlserver.sql

2.2 Verify Database

   -- Connect to SQL Server and run:
   USE LibraryDB;
   SELECT COUNT(*) FROM Books;      -- Should return 7
   SELECT COUNT(*) FROM Members;    -- Should return 3
   SELECT COUNT(*) FROM Staff;      -- Should return 2
   SELECT * FROM AvailableBooks;    -- Should show available books

2.3 Enable TCP/IP (if connection fails)

   WINDOWS:
   1. Open "SQL Server Configuration Manager"
   2. Expand "SQL Server Network Configuration"
   3. Click "Protocols for [YOUR_INSTANCE]"
   4. Right-click "TCP/IP" → Enable
   5. Restart SQL Server service
   6. Verify port 1433 is open (default)
   
   LINUX:
   sudo /opt/mssql/bin/mssql-conf set network.tcpport 1433
   sudo systemctl restart mssql-server

═══════════════════════════════════════════════════════════════════════════
SECTION 3: BUILD THE PROJECT
═══════════════════════════════════════════════════════════════════════════

3.1 Organize Project Files

   Create a project directory structure:
   
   LibraryManagementSystem/
   ├── database_sqlserver.sql
   ├── DBManager.h
   ├── DBManager.cpp
   ├── Book.h
   ├── Book.cpp
   ├── Category.h
   ├── Category.cpp
   ├── Member.h
   ├── Member.cpp
   ├── Staff.h
   ├── Staff.cpp
   ├── Borrowing.h
   ├── Borrowing.cpp
   ├── Reservation.h
   ├── Reservation.cpp
   ├── main.cpp
   └── README_run_steps.txt

3.2 Modify Connection String in main.cpp

   Open main.cpp and find the connection string section (around line 330):
   
   // OPTION 1: SQL Server Authentication (username/password)
   string connStr = "Driver={ODBC Driver 17 for SQL Server};"
                    "Server=localhost;"           // or "localhost\\SQLEXPRESS"
                    "Database=LibraryDB;"
                    "UID=sa;"                     // your SQL Server username
                    "PWD=YourPassword123;";       // your SQL Server password
   
   // OPTION 2: Windows Authentication (no password needed)
   string connStr = "Driver={ODBC Driver 17 for SQL Server};"
                    "Server=localhost\\SQLEXPRESS;"
                    "Database=LibraryDB;"
                    "Trusted_Connection=yes;";
   
   // OPTION 3: DSN-based (after creating a System DSN)
   string connStr = "DSN=LibraryDSN;UID=sa;PWD=YourPassword123;";
   
   Choose one and modify with your actual server name and credentials.

3.3 Build on WINDOWS with Visual Studio

   METHOD A: Using Visual Studio IDE
   
   1. Open "Visual Studio"
   2. Create new project: "Console App" (C++)
   3. Add all .h and .cpp files to the project
   4. Right-click project → Properties:
      - C/C++ → General → Additional Include Directories:
        Add: C:\vcpkg\installed\x64-windows\include
      - Linker → General → Additional Library Directories:
        Add: C:\vcpkg\installed\x64-windows\lib
      - Linker → Input → Additional Dependencies:
        Add: nanodbc.lib odbc32.lib
   5. Build → Build Solution (F7)
   6. Run (Ctrl+F5)
   
   METHOD B: Using Developer Command Prompt (cl.exe)
   
   1. Open "Developer Command Prompt for VS 2022"
   2. Navigate to project directory:
      cd C:\path\to\LibraryManagementSystem
   
   3. Compile (assuming vcpkg is at C:\vcpkg):
      
      cl /EHsc /std:c++17 ^
         /I"C:\vcpkg\installed\x64-windows\include" ^
         main.cpp DBManager.cpp Book.cpp Category.cpp Member.cpp ^
         Staff.cpp Borrowing.cpp Reservation.cpp ^
         /link ^
         /LIBPATH:"C:\vcpkg\installed\x64-windows\lib" ^
         nanodbc.lib odbc32.lib ^
         /OUT:LibrarySystem.exe
   
   4. Run:
      LibrarySystem.exe

3.4 Build on WINDOWS with MinGW (g++)

   1. Open Command Prompt or PowerShell
   2. Navigate to project directory
   3. Compile:
      
      g++ -std=c++17 -o LibrarySystem.exe ^
          main.cpp DBManager.cpp Book.cpp Category.cpp Member.cpp ^
          Staff.cpp Borrowing.cpp Reservation.cpp ^
          -I"C:\vcpkg\installed\x64-mingw-static\include" ^
          -L"C:\vcpkg\installed\x64-mingw-static\lib" ^
          -lnanodbc -lodbc32
   
   4. Run:
      LibrarySystem.exe

3.5 Build on LINUX

   1. Open Terminal
   2. Navigate to project directory:
      cd /path/to/LibraryManagementSystem
   
   3. Compile (if vcpkg installed system-wide):
      
      g++ -std=c++17 -o LibrarySystem \
          main.cpp DBManager.cpp Book.cpp Category.cpp Member.cpp \
          Staff.cpp Borrowing.cpp Reservation.cpp \
          -I/usr/local/include \
          -L/usr/local/lib \
          -lnanodbc -lodbc
   
   4. Or with vcpkg paths:
      
      g++ -std=c++17 -o LibrarySystem \
          main.cpp DBManager.cpp Book.cpp Category.cpp Member.cpp \
          Staff.cpp Borrowing.cpp Reservation.cpp \
          -I$HOME/vcpkg/installed/x64-linux/include \
          -L$HOME/vcpkg/installed/x64-linux/lib \
          -lnanodbc -lodbc
   
   5. Run:
      ./LibrarySystem

3.6 Using CMake (Cross-Platform) - ADVANCED

   Create CMakeLists.txt in project root:
   
   cmake_minimum_required(VERSION 3.15)
   project(LibraryManagementSystem)
   
   set(CMAKE_CXX_STANDARD 17)
   set(CMAKE_CXX_STANDARD_REQUIRED ON)
   
   # Find packages
   find_package(nanodbc CONFIG REQUIRED)
   find_package(ODBC REQUIRED)
   
   # Add executable
   add_executable(LibrarySystem
       main.cpp
       DBManager.cpp
       Book.cpp
       Category.cpp
       Member.cpp
       Staff.cpp
       Borrowing.cpp
       Reservation.cpp
   )
   
   # Link libraries
   target_link_libraries(LibrarySystem PRIVATE nanodbc ${ODBC_LIBRARIES})
   
   Build:
   mkdir build && cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=[path-to-vcpkg]/scripts/buildsystems/vcpkg.cmake
   cmake --build . --config Release
   
   Run:
   ./Release/LibrarySystem (Windows)
   ./LibrarySystem (Linux)

═══════════════════════════════════════════════════════════════════════════
SECTION 4: RUN THE APPLICATION
═══════════════════════════════════════════════════════════════════════════

4.1 First Run

   1. Execute the compiled program
   2. You should see:
      ╔═════════════════════════════════════════════╗
      ║   LIBRARY MANAGEMENT SYSTEM - SQL SERVER   ║
      ╚═════════════════════════════════════════════╝
      
      Connecting to SQL Server...
      ✓ Connected successfully!
   
   3. If connection fails, check:
      - SQL Server is running (check Services on Windows)
      - Database LibraryDB exists
      - Connection string is correct
      - TCP/IP is enabled (see Section 2.3)
      - Firewall allows port 1433

4.2 Example Usage Workflow

   EXAMPLE 1: View All Books
   
   Menu Choice: 1
   Output:
   === ALL BOOKS ===
   ID   ISBN              Title                Author        Category   Available
   ----------------------------------------------------------------------------------
   1    978-0-06-112008-4 To Kill a Mockingbird Harper Lee   Fiction    2/3
   2    978-0-7432-7356-5 1984                 George Orwell Fiction    4/5
   ...
   Total: 7 books
   
   EXAMPLE 2: Search for a Book
   
   Menu Choice: 3
   Enter book title to search: Gatsby
   
   Output:
   === SEARCH RESULTS ===
   ========================================
   Book ID: 6
   ISBN: 978-0-452-28423-4
   Title: The Great Gatsby
   Author: F. Scott Fitzgerald
   ...
   Found: 1 books
   
   EXAMPLE 3: Add New Book
   
   Menu Choice: 4
   Available Categories:
   1. Fiction
   2. Science
   3. History
   4. Technology
   5. Arts
   
   ISBN: 978-0-123-45678-9
   Title: Clean Code
   Author: Robert C. Martin
   Publisher: Prentice Hall
   Publication Year: 2008
   Category ID: 4
   Total Copies: 5
   Price: 39.99
   Shelf Location: D-402
   
   ✓ Book added successfully!
   
   EXAMPLE 4: Create a Borrowing
   
   Menu Choice: 9
   Book ID: 1
   Member ID: 1
   Staff ID: 1
   Due Date (YYYY-MM-DD): 2025-12-01
   
   ✓ Borrowing created successfully!
   
   EXAMPLE 5: Return a Book
   
   Menu Choice: 12
   Borrowing ID: 1
   
   ✓ Book returned successfully!
   
   EXAMPLE 6: Update Overdue Books (runs stored procedure)
   
   Menu Choice: 15
   
   === UPDATE OVERDUE BOOKS ===
   ✓ Updated 1 overdue books.
   
   EXAMPLE 7: Calculate Overdue Fines
   
   Menu Choice: 16
   Daily Fine Rate (default 1.00): 2.50
   
   === CALCULATE OVERDUE FINES ===
   ✓ Calculated fines for 1 borrowings.

4.3 Test Connection

   Menu Choice: 17
   
   Output:
   === TEST CONNECTION ===
   ✓ Connection is active and working!
   
   This verifies the database connection is alive.

4.4 View Logs

   All operations are logged to: library_db.log
   
   Example log entries:
   [Mon Nov 17 14:30:15 2025] DBManager initialized
   [Mon Nov 17 14:30:16 2025] Connected to database successfully
   [Mon Nov 17 14:30:22 2025] Retrieved 7 books
   [Mon Nov 17 14:31:05 2025] Book created: Clean Code (ISBN: 978-0-123-45678-9)

═══════════════════════════════════════════════════════════════════════════
SECTION 5: TROUBLESHOOTING
═══════════════════════════════════════════════════════════════════════════

PROBLEM 1: "Failed to connect to database"
SOLUTION:
   - Verify SQL Server is running:
     Windows: Open Services.msc, look for "SQL Server (MSSQLSERVER)" or "SQL Server (SQLEXPRESS)"
     Linux: sudo systemctl status mssql-server
   - Check server name in connection string:
     Default instance: localhost
     Named instance: localhost\SQLEXPRESS
   - Test with sqlcmd:
     sqlcmd -S localhost -U sa -P YourPassword123
     If this fails, SQL Server has auth/network issues

PROBLEM 2: "Login failed for user 'sa'"
SOLUTION:
   - Enable SQL Server Authentication:
     1. Open SSMS → Connect to server
     2. Right-click server → Properties → Security
     3. Select "SQL Server and Windows Authentication mode"
     4. Restart SQL Server service
   - Reset 'sa' password:
     ALTER LOGIN sa WITH PASSWORD = 'NewPassword123';
     ALTER LOGIN sa ENABLE;

PROBLEM 3: "TCP Provider: No connection could be made"
SOLUTION:
   - Enable TCP/IP (see Section 2.3)
   - Check firewall:
     Windows Firewall → Allow an app → Add SQL Server (sqlservr.exe)
     Or open port 1433:
     netsh advfirewall firewall add rule name="SQL Server" dir=in action=allow protocol=TCP localport=1433
   - For Linux:
     sudo ufw allow 1433/tcp

PROBLEM 4: "Database 'LibraryDB' does not exist"
SOLUTION:
   - Re-run database_sqlserver.sql script
   - Verify with: SELECT name FROM sys.databases;

PROBLEM 5: Linker error "unresolved external symbol" (nanodbc)
SOLUTION:
   - Ensure nanodbc.lib is in library path
   - Check vcpkg integration: vcpkg integrate install
   - For manual builds, verify paths:
     /LIBPATH:"C:\vcpkg\installed\x64-windows\lib"

PROBLEM 6: "Driver not found" or "Data source name not found"
SOLUTION:
   - Install ODBC Driver 17 for SQL Server (see Section 1.3)
   - Verify driver name:
     Windows: Open "ODBC Data Sources (64-bit)" → Drivers tab
     Linux: odbcinst -q -d
   - Update driver name in connection string:
     Change "ODBC Driver 17" to "ODBC Driver 18" if needed

PROBLEM 7: "Cannot bind parameter" errors
SOLUTION:
   - Ensure data types match between C++ and SQL Server:
     INT in SQL = int in C++
     NVARCHAR = std::string
     DECIMAL = double
   - Check parameter order in prepared statements

PROBLEM 8: Program crashes on execute
SOLUTION:
   - Check connection is valid before each operation:
     if (!db.isConnected()) { ... }
   - Review library_db.log for detailed error messages
   - Wrap database calls in try-catch blocks

PROBLEM 9: nanodbc compile errors
SOLUTION:
   - Ensure C++17 standard: /std:c++17 or -std=c++17
   - Update nanodbc to latest version
   - Check include paths are correct

PROBLEM 10: Port 1433 blocked
SOLUTION:
   - Change SQL Server port:
     SQL Server Configuration Manager → TCP/IP Properties → IP Addresses → IPAll → TCP Port
   - Update connection string:
     Server=localhost,1435 (if changed to port 1435)

═══════════════════════════════════════════════════════════════════════════
SECTION 6: ADVANCED FEATURES
═══════════════════════════════════════════════════════════════════════════

6.1 Using Views

   The database includes pre-built views for common queries:
   
   - AvailableBooks: Shows only books with available copies
   - CurrentBorrowings: Shows active borrowings with overdue calculation
   - MemberStatistics: Shows member borrowing history and penalties
   
   Access in SQL:
   SELECT * FROM AvailableBooks;
   
   Already integrated in C++ code via getAvailableBooks() and getCurrentBorrowings()

6.2 Stored Procedures

   Three stored procedures are included:
   
   1. UpdateOverdueBooks: Marks borrowed books past due date as overdue
      EXEC UpdateOverdueBooks;
   
   2. CalculateOverdueFines: Creates/updates penalty records for overdue books
      EXEC CalculateOverdueFines @DailyFineRate = 1.50;
   
   3. GetMemberBorrowings: Retrieves all borrowings for a specific member
      EXEC GetMemberBorrowings @MemberID = 1;
   
   Called in C++ via executeUpdateOverdueBooks() and executeCalculateOverdueFines()

6.3 Activity Logging

   All database operations are logged to ActivityLogs table:
   
   SELECT * FROM ActivityLogs ORDER BY Timestamp DESC;
   
   Plus application-level logging to library_db.log file

6.4 Transaction Management

   Critical operations use transactions (see DBManager.cpp):
   - createBorrowing(): Updates book availability atomically
   - returnBook(): Returns book and updates availability together
   
   If any step fails, entire transaction rolls back

═══════════════════════════════════════════════════════════════════════════
SECTION 7: MIGRATING TO OTHER DATABASES (FUTURE)
═══════════════════════════════════════════════════════════════════════════

If you later want to migrate this project to MySQL or PostgreSQL:

7.1 SQL Syntax Changes Needed

   SQL Server → MySQL:
   - IDENTITY(1,1) → AUTO_INCREMENT
   - NVARCHAR → VARCHAR with UTF8MB4 charset
   - GETDATE() → NOW() or CURRENT_TIMESTAMP
   - DATEDIFF(DAY, x, y) → DATEDIFF(y, x) (reversed order!)
   - DATEADD(DAY, n, date) → DATE_ADD(date, INTERVAL n DAY)
   - CONVERT(VARCHAR, date, 23) → DATE_FORMAT(date, '%Y-%m-%d')
   - BEGIN TRANSACTION / COMMIT → START TRANSACTION / COMMIT
   - TOP n → LIMIT n
   - LEN() → LENGTH()
   - + (string concat) → CONCAT()
   
   SQL Server → PostgreSQL:
   - IDENTITY(1,1) → SERIAL or GENERATED ALWAYS AS IDENTITY
   - NVARCHAR → TEXT or VARCHAR
   - GETDATE() → NOW() or CURRENT_TIMESTAMP
   - DATEDIFF → age() function or date arithmetic
   - DATEADD → date + INTERVAL
   - CONVERT → CAST() or TO_CHAR()
   - String concat: CONCAT() or ||

7.2 C++ Code Changes

   - Replace nanodbc connection string with MySQL or PostgreSQL format
   - MySQL: "Driver={MySQL ODBC 8.0 Unicode Driver};Server=...;Database=...;"
   - PostgreSQL: "Driver={PostgreSQL Unicode};Server=...;Database=...;"
   - Or switch from nanodbc to native drivers (MySQL Connector/C++, libpqxx)
   - Minimal changes needed in DBManager.cpp (mostly SQL queries)

7.3 ODBC Driver Installation

   MySQL:
   - Download MySQL Connector/ODBC from https://dev.mysql.com/downloads/connector/odbc/
   
   PostgreSQL:
   - Download psqlODBC from https://www.postgresql.org/ftp/odbc/versions/

═══════════════════════════════════════════════════════════════════════════
SECTION 8: QUICK REFERENCE
═══════════════════════════════════════════════════════════════════════════

8.1 Essential Commands

   Start SQL Server (Windows):
   net start MSSQLSERVER
   
   Stop SQL Server (Windows):
   net stop MSSQLSERVER
   
   Start SQL Server (Linux):
   sudo systemctl start mssql-server
   
   Check SQL Server status (Linux):
   sudo systemctl status mssql-server
   
   Connect with sqlcmd:
   sqlcmd -S localhost -U sa -P YourPassword123
   
   List databases:
   SELECT name FROM sys.databases;
   
   Switch to LibraryDB:
   USE LibraryDB;

8.2 Connection String Templates

   # SQL Authentication (username/password)
   Driver={ODBC Driver 17 for SQL Server};Server=localhost;Database=LibraryDB;UID=sa;PWD=Pass123;
   
   # Windows Authentication
   Driver={ODBC Driver 17 for SQL Server};Server=localhost\SQLEXPRESS;Database=LibraryDB;Trusted_Connection=yes;
   
   # Remote server
   Driver={ODBC Driver 17 for SQL Server};Server=192.168.1.100;Database=LibraryDB;UID=sa;PWD=Pass123;
   
   # With encryption
   Driver={ODBC Driver 17 for SQL Server};Server=localhost;Database=LibraryDB;UID=sa;PWD=Pass123;Encrypt=yes;TrustServerCertificate=yes;

8.3 vcpkg Quick Commands

   Install nanodbc:
   vcpkg install nanodbc
   
   List installed packages:
   vcpkg list
   
   Update packages:
   vcpkg update
   vcpkg upgrade
   
   Remove package:
   vcpkg remove nanodbc

═══════════════════════════════════════════════════════════════════════════
CONGRATULATIONS!
═══════════════════════════════════════════════════════════════════════════

You now have a fully functional Library Management System running on 
Microsoft SQL Server with C++ and nanodbc!

For questions or issues:
1. Check library_db.log for detailed error messages
2. Review SQL Server error logs (SQL Server Management Studio → Management → SQL Server Logs)
3. Verify all prerequisites are correctly installed
4. Test connection with sqlcmd before testing the C++ application

Happy coding! 📚💻