#ifndef ADVANCED_BS_H
#define ADVANCED_BS_H

#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>
#include <chrono>
#include <memory>
#include <stack>
#include "base_bs.h"
#include "my_exeption.h"

// класс с механизмом упрощения схемы, наследник класса BaseBoolScheme
struct AdvancedBoolScheme final : BaseBoolScheme {
private:
    // дерево для хранения узлов с операциями и аргументами
    struct Tree;

    void deleteRepetition(Tree &tree);
    
    struct Tree {
    private:
        struct Node {
            std::string name; // имя узла, т.е. конкретной переменной
            std::string operationName; // имя операции, если переменная не лист, иначе пустая строка
            std::shared_ptr<Node> first; // аргумент функции, также являющийся узлом (или nullptr, если лист)
            std::shared_ptr<Node> second; // тоже самое, если функция бинарная, иначе nullptr

            Node(const std::string &name_);
            Node(const std::string &name_, const std::string &name2_);
        };

        std::shared_ptr<Node> root;

        friend std::ostream& operator<<(std::ostream &out, const Tree &tree);
        friend void AdvancedBoolScheme::deleteRepetition(Tree &tree);

    public:
        // итератор, исользующий стек, далее все понятно, поэтому без коментариев
        struct TreeStackIterator {
            std::stack<std::shared_ptr<Node>> stack;
            TreeStackIterator(std::shared_ptr<Node> cur);
            TreeStackIterator(Tree &tree);
            std::shared_ptr<Node> next();
            bool hasNext() const;
        };

        // функция заполнения дерева из переменной BaseBoolScheme::vars
        void fillFromVars(const std::vector<std::pair<std::string, std::vector<std::string>>> &v);

        // функция, выдающая количество аргументов данного узла, как функции, или ноль, если это лист
        static int getOperNumber(std::shared_ptr<Tree::Node> node);

        // функция сравнения двух поддеревьев для определения их одинаковости
        // В данной реализации поддеревья одинаковые <=> у каждых соответсвующих узлов одинаковая логическая операция 
        // и одинаковое кол-во аргументов. Для листов очевидно иначе, листы одинаковые <=> одинаковые их имена
        static bool compareNodes(std::shared_ptr<Tree::Node> first, std::shared_ptr<Tree::Node> second);
        
    };


    // запись упрощенного дерева в новый файл формата *.bench 
    void writeToFile(Tree &t, const std::string &oldFile, const std::string &newFile);

    // функция упрощения текущей булевы схемы, выдающая название нового файла
    std::string simplification(std::string fileName);

public:

    // конструктор, который создает и упрощает булеву схему, (а затем выдает сколько процентов было урощено)
    AdvancedBoolScheme(const std::string &fileName);

    friend std::ostream& operator<<(std::ostream &out, Tree &tree);
};


#endif // ADVANCED_BS_H