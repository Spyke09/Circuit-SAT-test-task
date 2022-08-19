#ifndef ME_EXEPTION_H
#define ME_EXEPTION_H

#include <exception>
#include <string>

// Собсвенный класс исключений
struct MyException : std::exception {
private:
    std::string error;
public:
    MyException(const std::string &error_);
    const char* what() const noexcept;
};


#endif