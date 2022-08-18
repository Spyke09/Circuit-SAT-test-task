#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <stack>
#include <exception>
#include <chrono>
#include <memory>
#include <utility> 

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

// Собсвенный класс исключений
struct MyException : std::exception {
private:
    std::string error;
public:
    MyException(const std::string &error_) : error(error_) {}

    const char* what() const noexcept {
        return error.c_str();
    }
};

// Базовый класс, содержащий конкретную булеву схему
struct BaseBoolScheme {
protected:
    //
    std::vector<std::string> inputs;  // здесь хранятся входные переменные, которые INPUT(...)
    std::string output; // переменная OUTPUT(...)
    std::vector<std::pair<std::string, std::vector<std::string>>> vars; // информация о строках вида "var1 = func(var2, var3)" 

    // функция парсинга аргументов функции
    // для функции func(var2, var3) выдаст вектор {"func", "var1", "var2"}
    virtual std::vector<std::string> parseFuncArgs(const std::string &st) const noexcept{
        std::vector<std::string> result;
        int lbracket = st.find('('), rbracket = st.find(')');
        std::string name = st.substr(0, lbracket);
        result.push_back(name);
        std::string args = st.substr(lbracket + 1, rbracket - lbracket - 1);
        bool flag = false;
        for (auto& i : args) {
            if (i == ',' || i == ' ') {
                flag = false;
            } else if (flag == false) {
                result.push_back("");
                result[result.size() - 1].push_back(i);
                flag = true;
            } else {
                result[result.size() - 1].push_back(i);
            }
        }
        return result;
    }

    virtual std::pair<std::string, std::string> parseVars(const std::string &s) {
        int i = 0;
        for ( ; s[i] != ' '; ++i);
        auto name = s.substr(0, i);
        for ( ; s[i] == ' '; ++i);
        ++i;
        for ( ; s[i] == ' '; ++i);
        auto func = s.substr(i, s.size() - i);
        return {name, func};
    }

    // функция парсинга файла, здесь формируются переменные vars, inputs, output
    virtual void parseFile(const std::string &filename) {
        std::ifstream fin;
        std::string buff;
        fin = std::ifstream(filename);
        if (!fin) {
            throw MyException("File not found");
        }
        bool endinput = false, endoutput = false;
        while(std::getline (fin, buff)) {
            if (buff[0] == '#' || buff == "") {
                continue;
            } 
            else if (!endinput && buff.size() > 5 && buff.substr(0, 5) == "INPUT") {
                inputs.push_back(buff.substr(6, buff.find(')') - 6));
            }
            else if (!endoutput && buff.size() > 6 && buff.substr(0, 6) == "OUTPUT") {
                endinput = true;
                output = buff.substr(7, buff.find(')') - 7);
            } else {
                endoutput = true;
                auto p = parseVars(buff);
                std::vector<std::string> funcargs = parseFuncArgs(p.second);
                vars.push_back({p.first, funcargs});
            }
        }
        fin.close();
    }

public:

    // конструктор, аргумент - имя файла формата *.bench
    BaseBoolScheme(const std::string &fileName) {
        parseFile(fileName);
    }

    // функция вычисления значения функции по функции и аргументам, которые в виде строк
    virtual bool calculateBooleanFunction(std::vector<std::string> expr, std::unordered_map<std::string, bool> &valuesMap) const {
        if (expr[0] == "NOT") {
            return !valuesMap[expr[1]];
        } else if (expr[0] == "AND") {
            return valuesMap[expr[1]] && valuesMap[expr[2]];
        } else if (expr[0] == "OR") {
            return valuesMap[expr[1]] || valuesMap[expr[2]];
        } else if (expr[0] == "XOR") {
            return valuesMap[expr[1]] ^ valuesMap[expr[2]];
        } else if (expr[0] == "NAND") {
            return !(valuesMap[expr[1]] && valuesMap[expr[2]]);
        } else if (expr[0] == "NOR") {
            return !(valuesMap[expr[1]] || valuesMap[expr[2]]);
        } else if (expr[0] == "NXOR") {
            return !(valuesMap[expr[1]] ^ valuesMap[expr[2]]);
        } else if (expr[0] == "DFF" || expr[0] == "BUFF") {
            return valuesMap[expr[1]];
        } else {
            throw MyException("Failed to calculate function " + expr[0]);
        }
        
    }

    // функция вычисления булевы схемы по заданному вектору аргументов типа bool
    virtual bool calculate(std::vector<bool> numValues) const {
        std::unordered_map<std::string, bool> valuesMap;
        for (size_t i = 0; i < numValues.size(); ++i) {
            valuesMap.insert({inputs[i], numValues[i]});
        }
        for (const auto &i : vars) {
            bool boolArgsQ = true;
            for (size_t t = 1; t < i.second.size(); ++t) {
                boolArgsQ &= (valuesMap.find(i.second[t]) != valuesMap.end());
            }
            if (boolArgsQ) {
                valuesMap.insert({i.first, calculateBooleanFunction(i.second, valuesMap)});
            } else {
                throw MyException("No arguments for " + i.first);
            }
        }
        if (valuesMap.find(output) == valuesMap.end()) {
            throw  MyException("Output is not calculated");
        }
        return valuesMap[output];
    }

    // функция выдающая копию переменной inputs
    virtual std::vector<std::string> getInputs() const {
        return inputs;
    }

    // функция выдающая копию переменной outputs
    virtual std::string getOutput() const{
        return output;
    }

    // функция выдающая количество входных переменных
    virtual size_t getInputsSize() const {
        return inputs.size();
    }
    virtual ~BaseBoolScheme() {}
};

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
    SimpleSolver(std::shared_ptr<BaseBoolScheme> &&bs_) : bs(bs_), deltaTime(-1), v(std::vector<bool>(bs_->getInputsSize(), 0)), inputs(bs->getInputs()) {}

    // функция, запускающая решение задачи выполнимости
    // обычный перебор. Вектор на каждом шагу, преобразуется так, что в результате будут перебраны все варианты
    // здесь засекается время выполнения кода
    std::vector<bool> solve() {
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
        deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(timeFinish - timeStart).count();
        return v;
    }

    // функция выдающая ответ в виде словаря где каждой переменной соответсвует ее значение
    std::unordered_map<std::string, bool> getAnswer() {
        std::unordered_map<std::string, bool> answer;
        for (size_t i = 0; i < v.size(); ++i) {
            answer.insert({inputs[i], v[i]});
        }
        return answer;
    }

    // функция печати результатов после работы метода solve()
    void printResults() {
        std::cout << "Time: " << deltaTime << " ms\n";
        if (!answerQ) {
            std::cout << "No solution\n";
        } else {
            std::cout << "Answer: ";
            for (size_t i = 0; i < v.size(); ++i) {
                std::cout << "'" << inputs[i] << "' = " << v[i] << ((i < v.size() - 1) ? ", " : ".");
            }
            std::cout << "\n";
        }
        
    }

private:
    // функция, изменяющая вектор на следующую комбинацию аргументов
    bool increaseVectorValue() {
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
};

// класс с механизмом упрощения схемы, наследник класса BaseBoolScheme
struct AdvancedBoolScheme final : BaseBoolScheme {
private:
    // дерево для хранения узлов с операциями и аргументами
    struct Tree {
    private:
        struct Node {
            std::string name; // имя узла, т.е. конкретной переменной
            std::string operationName; // имя операции, если переменная не лист, иначе пустая строка
            std::shared_ptr<Node> first; // аргумент функции, также являющийся узлом (или nullptr, если лист)
            std::shared_ptr<Node> second; // тоже самое, если функция бинарная, иначе nullptr

            Node(const std::string &name_) : name(name_) {}
            Node(const std::string &name_, const std::string &name2_) : name(name_), operationName(name2_) {}
        };

        std::shared_ptr<Node> root;
        int size = 0;

    public:
        // итератор, исользующий стек, далее все понятно, поэтому без коментариев
        struct TreeStackIterator {
            std::stack<std::shared_ptr<Node>> stack;

            TreeStackIterator(std::shared_ptr<Node> cur) {
                stack.push(cur);
            }

            TreeStackIterator(Tree &tree) {
                stack.push((tree.root));
            }

            std::shared_ptr<Node> next() {
                std::shared_ptr<Node> cur = stack.top();
                stack.pop();
                if (cur->first != nullptr) {
                    stack.push(cur->first);
                }
                if (cur->second != nullptr) {
                    stack.push(cur->second);
                }
                return cur;
            }

            bool hasNext() const {
                return !stack.empty();
            }
        };

        // функция заполнения дерева из переменной BaseBoolScheme::vars
        void fillFromVars(const std::vector<std::pair<std::string, std::vector<std::string>>> &v) {
            std::unordered_map<std::string, std::vector<std::string>> map;
            std::copy(v.begin(), v.end(), std::inserter(map, map.begin()));
            std::stack<std::shared_ptr<Node>> stack;
            auto curr = v[v.size() - 1];
            root = std::make_shared<Node>(curr.first);
            stack.push(root);
            while (!stack.empty()) {
                std::shared_ptr<Node> node = stack.top();
                stack.pop();
                if (map.find(node->name) != map.end()) {
                    std::vector<std::string> args = map[node->name];
                    node->operationName = args[0];
                    node->first = std::make_shared<Node>(args[1]);
                    stack.push(node->first);
                    if (args.size() == 3) {
                        node->second = std::make_shared<Node>(args[2]);
                        stack.push(node->second);
                    }
                    map.erase(node->name);
                }
            }
        }

        // функция, выдающая количество аргументов данного узла, как функции, или ноль, если это лист
        static int getOperNumber(std::shared_ptr<Tree::Node> node) {
            int num = 0;
            if (node->first != nullptr) num++;
            if (node->second != nullptr) num++;
            if (node->operationName == "") return 0;
            return num;
        }

        // функция сравнения двух поддеревьев для определения их одинаковости
        // В данной реализации поддеревья одинаковые <=> у каждых соответсвующих узлов одинаковая логическая операция 
        // и одинаковое кол-во аргументов. Для листов очевидно иначе, листы одинаковые <=> одинаковые их имена
        static bool compareNodes(std::shared_ptr<Tree::Node> first, std::shared_ptr<Tree::Node> second) {
            Tree::TreeStackIterator iter1 = {first};
            Tree::TreeStackIterator iter2 = {second};
            int t = 0;
            while(iter1.hasNext() && iter2.hasNext()) {
                ++t;
                if (t > 100) {
                    return 0;
                }
                auto i1 = iter1.next();
                auto i2 = iter2.next();
                int num1 = getOperNumber(i1), num2 = getOperNumber(i2);
                if (num1 != num2) return 0;
                if (i1->operationName != i2->operationName) return 0;
                if (num1 == 0 && i1->name != i2->name) return 0;
                
            }
            return 1;
        }

        friend std::ostream& operator<<(std::ostream &out, const Tree &tree);
        friend void deleteRepetition(Tree &tree);
    };

    // функция, которая убирает повторяющиеся гейты
    // Упрощение схемы заключается в поиске одинаковых по структуре поддеревьев логической схемы.
    // При обнаружении одинаковых поддеревьев имя переменной первого поддерева становится именем корня второго 
    // и становится листом. И так далее, пока в дереве есть что упрощать. Поддеревья с кол-вом элементов > 100 не рассматриваются.
    friend void deleteRepetition(Tree &tree) {
        Tree::TreeStackIterator iter = {tree.root};
        std::vector<std::shared_ptr<Tree::Node>> v;
        while (iter.hasNext()) {
            v.push_back(iter.next());
        }
        auto timeStart = std::chrono::high_resolution_clock::now();
        if (v.size() > 10'000) {
            return;
        }
        while(true) {
            bool b = false;
            for (size_t i = 0; i < v.size(); ++i) {
                auto cur = v[i];
                if (Tree::getOperNumber(cur) == 0) continue;
                for (size_t j = i + 1; j < v.size(); ++j) {
                    auto next = v[j];
                    if (Tree::getOperNumber(next) == 0) continue;
                    if (Tree::getOperNumber(cur) != 0 && Tree::getOperNumber(next) != 0 && Tree::compareNodes(cur, next)) {
                        
                        std::string oldName = next->name;
                        for (size_t k = 0; k < v.size(); ++k) {
                            auto next2 = v[k];
                            if (k == i) continue;
                            if (next2 != nullptr && (next2->name == oldName)) {
                                next2->name = cur->name;
                                next2->operationName = "";
                            }
                        }
                        // std::cout << (*cur)->name << " " << (*next)->name << "\n";

                        iter = {tree.root};
                        v.clear();
                        while (iter.hasNext()) {
                            v.push_back(iter.next());
                        } 
                        b = true;
                        break;
                    }
                }
                if (b) break;
            }
            if (!b) break;
            auto timeFinish = std::chrono::high_resolution_clock::now();
            auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(timeFinish - timeStart).count();
            if (deltaTime > 10'000) {
                break;
            }
        }
    }

    // запись упрощенного дерева в новый файл формата *.bench 
    void writeToFile(Tree &t, const std::string &oldFile, const std::string &newFile) {
        std::unordered_map<std::string, std::vector<std::string>> m;
        Tree::TreeStackIterator iter = {t};
        while(iter.hasNext()) {
            auto i = iter.next();
            auto j = i->first, k = i->second;
            int num = Tree::getOperNumber(i);
            if (num == 0) {
                continue;
            } else if (num == 1) {
                m.insert({i->name, {i->operationName, j->name}});
            } else if (num == 2) {
                m.insert({i->name, {i->operationName, j->name, k->name}});
            }
        }
        
        std::ifstream fin = std::ifstream(oldFile);
        std::ofstream fout = std::ofstream(newFile);
        std::string buff;
        
        if (!fin) {
            throw MyException("File not found");
        }
        while(std::getline (fin, buff)) {
            if (buff[0] == '#' || buff == "") {
                fout << buff << "\n";
            } 
            else if (buff.size() > 5 && buff.substr(0, 5) == "INPUT") {
                fout << buff << "\n";
            } else if (buff.size() > 6 && buff.substr(0, 6) == "OUTPUT") {
                fout << buff << "\n";
                fout << "\n";
                break;
            }
        }
        fin.close();

        std::vector<std::string> s;
        s.push_back(output);

        while(!s.empty()) {
            std::string cur = s[s.size() - 1];
            if (m.find(cur) == m.end()) {
                s.pop_back();
                continue;
            }
            if (m[cur].size() == 2) {
                if (m.find(m[cur][1]) == m.end()) {
                    fout << cur << " = " << m[cur][0] << "(" << m[cur][1] << ")\n";
                    s.pop_back();
                    m.erase(cur);
                } else if (m.find(m[cur][1]) != m.end()) {
                    s.push_back(m[cur][1]);
                }
            } else if (m[cur].size() == 3) {
                if (m.find(m[cur][1]) == m.end() && m.find(m[cur][2]) == m.end()) {
                    fout  << cur << " = " << m[cur][0] << "(" << m[cur][1] <<  ", " << m[cur][2] << ")\n";
                    s.pop_back();
                    m.erase(cur);
                } else {
                    if (m.find(m[cur][1]) != m.end()) {
                        s.push_back(m[cur][1]);
                    }
                    if (m.find(m[cur][2]) != m.end()) {
                        s.push_back(m[cur][2]);
                    }
                }
            }
        }
        fout.close();
    }

    // функция упрощения текущей булевы схемы, выдающая название нового файла
    std::string simplification(std::string fileName) {
        Tree tree;
        tree.fillFromVars(vars);
        // std::cout << tree;
        deleteRepetition(tree);
        // std::cout << tree;
        int pfff = fileName.size() - 1;
        for (; pfff >= 0; --pfff) if (fileName[pfff] == '/') break;
        std::string newFile = fileName.substr(0, pfff) + "/simplified/simplified_" + fileName.substr(pfff + 1, fileName.size() - pfff);
        writeToFile(tree, fileName, newFile);
        return newFile;
    }

public:

    // конструктор, который создает и упрощает булеву схему, а затем выдает сколько процентов было урощено
    AdvancedBoolScheme(const std::string &fileName) : BaseBoolScheme(fileName) {
        [[maybe_unused]] size_t oldSize = vars.size();
        auto newFileName = simplification(fileName);
        vars.clear();
        inputs.clear();
        parseFile(newFileName);
        // std::cout << "Simplified by " << (oldSize - vars.size()) * 100 / oldSize << "%\n";
    }

    friend std::ostream& operator<<(std::ostream &out, Tree &tree);
};

    // оператор вывода дерева в поток
    std::ostream& operator<<(std::ostream &out, AdvancedBoolScheme::Tree &tree) {
        AdvancedBoolScheme::Tree::TreeStackIterator iter(tree);
        while(iter.hasNext()) {
            auto cur = iter.next();
            int num = AdvancedBoolScheme::Tree::getOperNumber(cur);
            auto f = cur->first;
            auto s = cur->second;
            if (num == 0) {
                out << cur->name << ", ";
            } else if (num == 1) {
                
                out << cur->name << " = " << cur->operationName << "(" <<  f->name << "), ";
            } else {
                
                out << cur->name << " = " << cur->operationName << "(" <<  f->name << ", ";
                out << s->name << "), ";
            }
            
        }
        out << "\n";
        return out;
    }

int main(int argc, char *argv[]) {
    if (argc != 3 && argc != 2) {
        std::cout << "There should be one or two arguments here\n";
        return 0;
    }

    std::string fileName = argv[1];
    auto bs = std::make_shared<AdvancedBoolScheme>(fileName);

    if (argc == 3 && static_cast<std::string>(argv[2]) == "-s") {
        SimpleSolver solver(bs);
        solver.solve();
        solver.printResults();
    }
}


