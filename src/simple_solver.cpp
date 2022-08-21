#include <memory>
#include "simple_solver.h"

// конструктор от BaseBoolScheme
SimpleSolver::SimpleSolver(std::shared_ptr<BaseBoolScheme> bs_) 
    : bs(bs_), deltaTime(-1), v(std::vector<bool>(bs_->getInputsSize(), 0)), inputs(bs->getInputs()) {}

// функция, запускающая решение задачи выполнимости
// обычный перебор. Вектор на каждом шагу, преобразуется так, что в результате будут перебраны все варианты
// здесь засекается время выполнения кода
std::vector<bool> SimpleSolver::solve() {
    auto timeStart = std::chrono::high_resolution_clock::now();
    while(true) {
        bool t = bs->calculate(v);
        if (t) {
            answerQ = true;
            break;
        }
        if (!increaseVectorValue()) {
            answerQ = false;
            break;
        }
    }
    auto timeFinish = std::chrono::high_resolution_clock::now();
    deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(timeFinish - timeStart).count();
    return v;
}

// функция выдающая ответ в виде словаря где каждой переменной соответсвует ее значение
std::unordered_map<std::string, bool> SimpleSolver::getAnswer() {
    std::unordered_map<std::string, bool> answer;
    for (size_t i = 0; i < v.size(); ++i) {
        answer.insert({inputs[i], v[i]});
    }
    return answer;
}

// функция печати результатов после работы метода solve()
void SimpleSolver::printResults() {
    std::cout << deltaTime << "\n";
    // std::cout << "Time: " << deltaTime << " microseconds\n";
    // if (!answerQ) {
    //     std::cout << "No solution\n";
    // } else {
    //     std::cout << "Answer: ";
    //     for (size_t i = 0; i < v.size(); ++i) {
    //         std::cout << "'" << inputs[i] << "' = " << v[i] << ((i < v.size() - 1) ? ", " : ".");
    //     }
    //     std::cout << "\n";
    // }
    
}

// функция, изменяющая вектор на следующую комбинацию аргументов
bool SimpleSolver::increaseVectorValue() {
    size_t i = 0;
    while(v[i] != 0) {
        v[i] = 0;
        i++;
    }
    if(i < v.size()) {
        v[i] = 1;
        return true;
    }
    return false;
}