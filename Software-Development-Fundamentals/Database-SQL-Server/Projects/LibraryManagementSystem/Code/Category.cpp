// FILE: Category.cpp
#include "Category.h"
#include <iostream>

void Category::display() const {
    std::cout << "Category ID: " << categoryId << "\n";
    std::cout << "Name: " << categoryName << "\n";
    std::cout << "Description: " << description << "\n";
    std::cout << "---\n";
}