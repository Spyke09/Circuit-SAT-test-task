#ifndef BASE_BS_H
#define BASE_BS_H

#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>
#include "my_exeption.h"

// Базовый класс, содержащий конкретную булеву схему
struct BaseBoolScheme {
protected:
    //
    std::vector<std::string> inputs;  // здесь хранятся входные переменные, которые INPUT(...)
    std::string output; // переменная OUTPUT(...)
    std::vector<std::pair<std::string, std::vector<std::string>>> vars; // информация о строках вида "var1 = func(var2, var3)" 

    // функция парсинга аргументов функции
    // для функции func(var2, var3) выдаст вектор {"func", "var1", "var2"}
    virtual std::vector<std::string> parseFuncArgs(const std::string &st) const noexcept;

    virtual std::pair<std::string, std::string> parseVars(const std::string &s);

    // функция парсинга файла, здесь формируются переменные vars, inputs, output
    virtual void parseFile(const std::string &filename);

public:

    // конструктор, аргумент - имя файла формата *.bench
    BaseBoolScheme(const std::string &fileName);

    // функция вычисления значения функции по функции и аргументам, которые в виде строк
    virtual bool calculateBooleanFunction(std::vector<std::string> expr, std::unordered_map<std::string, bool> &valuesMap) const;

    // функция вычисления булевы схемы по заданному вектору аргументов типа bool
    virtual bool calculate(std::vector<bool> numValues) const;

    // функция выдающая копию переменной inputs
    virtual std::vector<std::string> getInputs() const;

    // функция выдающая копию переменной outputs
    virtual std::string getOutput() const;

    // функция выдающая количество входных переменных
    virtual size_t getInputsSize() const;

    // деструктор
    virtual ~BaseBoolScheme();
};


#endif // BASE_BS_H