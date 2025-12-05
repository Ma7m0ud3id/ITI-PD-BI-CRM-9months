// FILE: Category.h
#ifndef CATEGORY_H
#define CATEGORY_H

#include <string>

class Category {
public:
    int categoryId;
    std::string categoryName;
    std::string description;
    
    Category() : categoryId(0) {}
    
    void display() const;
};

#endif // CATEGORY_H