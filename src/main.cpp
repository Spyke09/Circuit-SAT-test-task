#include <iostream>
#include "advanced_bs.h"
#include "simple_solver.h"

/*
    Здесь реализованы:
    1) Базовый класс, который умеет считывать данные с файла и получать значение 
    схемы из конкретного вектора аргументов
    2) Простой "решатель" задачи выполнимости, использующий перебор
    3) Продвинутый класс, наследующий базовый, который умеет упрощать схему при своей инициализации.
    Упрощение схемы заключается в поиске одинаковых по структуре поддеревьев логической схемы.
    В данной реализации поддеревья одинаковые <=> у каждых соответсвующих узлов одинаковая логическая операция 
    и одинаковое кол-во аргументов.
    При обнаружении одинаковых поддеревьев имя переменной первого поддерева становится именем корня второго 
    и становится листом. И так далее, пока в дереве есть что упрощать. Поддеревья с кол-вом элементов > 100 не рассматриваются.
    Вся программа работает как скриптовый файл.
    1) Первый аргумент - имя файла в формате .bench.
    2) Второй -s, для того, чтобы был выведен ответ.
    
*/

int main(int argc, char *argv[]) {
    if (argc == 2) { // solve
        std::string fileName = argv[1];
        auto bs = std::make_shared<BaseBoolScheme>(fileName);
        SimpleSolver solver(bs);
        solver.solve();
        solver.printResults();
    } else if (argc == 3 && static_cast<std::string>(argv[2]) == "-s") { // -s == simplify
        std::string fileName = argv[1];
        auto bs = std::make_shared<AdvancedBoolScheme>(fileName);
    } else if (argc == 3 && static_cast<std::string>(argv[2]) == "-ss") { // -ss == simplify and solve
        std::string fileName = argv[1];
        auto bs = std::make_shared<AdvancedBoolScheme>(fileName);
        SimpleSolver solver(bs);
        solver.solve();
        solver.printResults();
    } else {
        std::cout << "Invalid arguments\n";
        return EINVAL;
    }
}
