#ifndef SIMPLE_SOLVER_H
#define SIMPLE_SOLVER_H

#include <vector>
#include <string>
#include <chrono>
#include <unordered_map>
#include <iostream>
#include "base_bs.h"
#include "advanced_bs.h"

// класс, решающий задачу выполнимости
struct SimpleSolver {
private:
    std::shared_ptr<BaseBoolScheme> bs; // константная ссылка на класс BaseBoolScheme. bs не будет изменен внутри из-за константности
    long long deltaTime; // время выполнения решения задачи выполнимости
    std::vector<bool> v; // вектор аргументов типа bool для поиска нужных аргументов
    std::vector<std::string> inputs; // входные переменные
    bool answerQ = false; // флаг для понимания был ли получен ответ или нет
public:
    // конструктор от BaseBoolScheme
    SimpleSolver(std::shared_ptr<BaseBoolScheme> bs_);

    // функция, запускающая решение задачи выполнимости
    // обычный перебор. Вектор на каждом шагу, преобразуется так, что в результате будут перебраны все варианты
    // здесь засекается время выполнения кода
    std::vector<bool> solve();

    // функция выдающая ответ в виде словаря где каждой переменной соответсвует ее значение
    std::unordered_map<std::string, bool> getAnswer();

    // функция печати результатов после работы метода solve()
    void printResults();

private:
    // функция, изменяющая вектор на следующую комбинацию аргументов
    bool increaseVectorValue();
};


#endif // SIMPLE_SOLVER_H