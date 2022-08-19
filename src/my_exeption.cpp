#include "my_exeption.h"

MyException::MyException(const std::string &error_) : error(error_) {}

const char* MyException::what() const noexcept {
        return error.c_str();
}