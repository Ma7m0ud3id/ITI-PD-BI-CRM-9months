# 📚 Library Management System

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![C++](https://img.shields.io/badge/C%2B%2B-17-brightgreen)
![SQL Server](https://img.shields.io/badge/SQL%20Server-2016%2B-red)
![Status](https://img.shields.io/badge/status-active-success)

A comprehensive **Library Management System** built with **C++** and **Microsoft SQL Server**, designed to automate and streamline library operations including book inventory management, member registration, borrowing/returning transactions, reservations, and automated penalty calculations.

---

## 📖 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Project Structure](#project-structure)
- [Technologies Used](#technologies-used)
- [Prerequisites](#prerequisites)
- [Setup Instructions](#setup-instructions)
- [Database Configuration](#database-configuration)
- [Compilation and Execution](#compilation-and-execution)
- [Usage](#usage)
- [Screenshots](#screenshots)
- [ERD (Entity-Relationship Diagram)](#erd-entity-relationship-diagram)
- [API Documentation](#api-documentation)
- [Testing](#testing)
- [Future Enhancements](#future-enhancements)
- [Contributing](#contributing)
- [License](#license)
- [Author](#author)
- [Acknowledgments](#acknowledgments)

---

## 🎯 Overview

The **Library Management System** is a database-driven application that facilitates efficient management of library resources. Built with modern C++ and enterprise-grade SQL Server, this system ensures data integrity through ACID-compliant transactions, implements security best practices with prepared statements, and provides comprehensive audit logging for all operations.

### Purpose

This system is designed for:
- **Libraries** seeking to digitize their operations
- **Educational institutions** managing book lending services
- **Developers** learning database connectivity with C++
- **Students** requiring a comprehensive project reference

### Key Highlights

✅ **8 Interconnected Database Tables** with normalized schema  
✅ **Transaction-Safe Operations** ensuring data consistency  
✅ **Automated Penalty Calculation** for overdue books  
✅ **Prepared Statements** preventing SQL injection  
✅ **Comprehensive Logging** for audit trail  
✅ **Stored Procedures & Views** for optimized queries  

---

## ✨ Features

### 📚 Book Management
- ➕ Add new books with complete bibliographic information (ISBN, title, author, publisher, year)
- 📋 View all books with availability status
- 🔍 Search books by title, author, or ISBN
- ✏️ Update book details and availability
- 🗑️ Delete books from inventory
- 📊 Track total and available copies in real-time

### 👥 Member Management
- 📝 Register new members with contact information
- 👁️ View complete member directory
- 🔄 Update membership status (Active/Suspended/Expired)
- 📈 View member borrowing history and statistics
- 💰 Track penalties and payment status

### 👨‍💼 Staff Management
- ➕ Register library staff members
- 📋 Maintain staff directory with positions and salaries
- 📊 Track staff processing of transactions

### 📤 Borrowing & Returns
- ✅ Process book borrowings with automatic inventory adjustment
- 🔙 Return books with automatic availability update
- 📅 Set custom due dates
- ⚠️ Automatic overdue detection
- 🔄 Transaction rollback on failures ensuring data integrity

### 📅 Reservations
- 📌 Reserve unavailable books
- ⏰ Set automatic expiration dates
- ✉️ Track reservation status (Pending/Fulfilled/Cancelled/Expired)
- 🔔 Notification-ready architecture

### 💰 Penalty Management
- 🤖 Automatic penalty calculation based on overdue days
- ⚙️ Configurable daily fine rate
- 💳 Track payment status (Unpaid/Paid/Waived)
- 📊 Link penalties to specific borrowing transactions

### 🗄️ Database Features
- 🔍 **3 Optimized Views**: AvailableBooks, CurrentBorrowings, MemberStatistics
- ⚡ **3 Stored Procedures**: UpdateOverdueBooks, CalculateOverdueFines, GetMemberBorrowings
- 🔐 **Comprehensive Constraints**: PRIMARY KEY, FOREIGN KEY, CHECK, UNIQUE
- 📊 **Strategic Indexes** for query performance optimization

### 🔐 Security & Logging
- 🛡️ SQL injection prevention through prepared statements
- 📝 Dual logging system (application file + database table)
- ⏱️ Timestamped audit trail for all operations
- 🔒 Support for Windows Authentication and SQL Authentication

---

## 📁 Project Structure

```
LibraryManagementSystem/
│
├── 📄 README.md                      # Project documentation
├── 📄 LICENSE                        # Project license
├── 📄 .gitignore                     # Git ignore file
│
├── 📂 src/                           # Source code files
│   ├── 📄 main.cpp                   # Application entry point
│   ├── 📄 DBManager.h                # Database manager header
│   ├── 📄 DBManager.cpp              # Database manager implementation
│   ├── 📄 Book.h                     # Book entity header
│   ├── 📄 Book.cpp                   # Book entity implementation
│   ├── 📄 Category.h                 # Category entity header
│   ├── 📄 Category.cpp               # Category entity implementation
│   ├── 📄 Member.h                   # Member entity header
│   ├── 📄 Member.cpp                 # Member entity implementation
│   ├── 📄 Staff.h                    # Staff entity header
│   ├── 📄 Staff.cpp                  # Staff entity implementation
│   ├── 📄 Borrowing.h                # Borrowing entity header
│   ├── 📄 Borrowing.cpp              # Borrowing entity implementation
│   ├── 📄 Reservation.h              # Reservation entity header
│   └── 📄 Reservation.cpp            # Reservation entity implementation
│

```

---

## 🛠️ Technologies Used

### Backend
- **Language**: C++ (C++17 Standard)
- **Database Connectivity**: nanodbc (ODBC wrapper library)
- **Compiler**: MSVC (Visual Studio) / GCC (Linux)
- **Build System**: Command-line tools / CMake

### Database
- **DBMS**: Microsoft SQL Server 2016+
- **Query Language**: T-SQL (Transact-SQL)
- **Driver**: Microsoft ODBC Driver 17/18 for SQL Server
- **Features Used**: 
  - Stored Procedures
  - Views
  - Triggers (optional)
  - Constraints (PK, FK, CHECK, UNIQUE)
  - Transactions (BEGIN, COMMIT, ROLLBACK)
  - Indexes

### Development Tools
- **IDE**: Visual Studio 2022 / Visual Studio Code
- **Version Control**: Git & GitHub
- **Database Management**: SQL Server Management Studio (SSMS)
- **Package Manager**: vcpkg (for nanodbc installation)

### Libraries & Dependencies
- **nanodbc**: Modern C++ ODBC wrapper ([GitHub](https://github.com/nanodbc/nanodbc))
- **ODBC API**: Standard database connectivity interface
- **STL (Standard Template Library)**: Containers, algorithms, iterators

### Design Patterns
- **RAII** (Resource Acquisition Is Initialization)
- **Repository Pattern** (DBManager class)
- **Entity Pattern** (Book, Member, Staff classes)
- **Singleton Pattern** (DBManager instance)

---

## 📋 Prerequisites

Before setting up the project, ensure you have the following installed:

### Required Software

1. **Microsoft SQL Server 2016 or later**
   - [Download SQL Server Express](https://www.microsoft.com/en-us/sql-server/sql-server-downloads) (Free)
   - Or SQL Server Developer Edition (Full-featured, free)

2. **SQL Server Management Studio (SSMS)**
   - [Download SSMS](https://aka.ms/ssmsfullsetup) (Windows only)
   - Alternative: [Azure Data Studio](https://docs.microsoft.com/sql/azure-data-studio/download) (Cross-platform)

3. **Microsoft ODBC Driver 17 or 18 for SQL Server**
   - [Download ODBC Driver](https://docs.microsoft.com/en-us/sql/connect/odbc/download-odbc-driver-for-sql-server)

4. **C++ Compiler**
   - **Windows**: Visual Studio 2019/2022 with "Desktop development with C++"
   - **Linux**: GCC 7.0+ (`sudo apt-get install build-essential g++`)

5. **vcpkg** (Package Manager for C++)
   - [vcpkg Installation Guide](https://github.com/microsoft/vcpkg#quick-start-windows)

6. **Git**
   - [Download Git](https://git-scm.com/downloads)

### System Requirements
- **OS**: Windows 10/11 or Linux (Ubuntu 20.04+)
- **RAM**: 4GB minimum (8GB recommended)
- **Disk Space**: 2GB for SQL Server + 500MB for development tools
- **Processor**: x64 architecture

---
