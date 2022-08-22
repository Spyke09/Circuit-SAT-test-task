#include "advanced_bs.h"


AdvancedBoolScheme::Tree::Node::Node(const std::string &name_) : name(name_) {}
AdvancedBoolScheme::Tree::Node::Node(const std::string &name_, const std::string &name2_) : name(name_), operationName(name2_) {}

AdvancedBoolScheme::Tree::TreeStackIterator::TreeStackIterator(std::shared_ptr<Node> cur) {
    stack.push(cur);
}

AdvancedBoolScheme::Tree::TreeStackIterator::TreeStackIterator(Tree &tree) {
    stack.push((tree.root));
}

std::shared_ptr<AdvancedBoolScheme::Tree::Node> AdvancedBoolScheme::Tree::TreeStackIterator::next() {
    std::shared_ptr<AdvancedBoolScheme::Tree::Node> cur = stack.top();
    stack.pop();
    if (getOperNumber(cur) == 0) {
        return cur;
    }
    if (cur->first != nullptr) {
        stack.push(cur->first);
    }
    if (cur->second != nullptr) {
        stack.push(cur->second);
    }
    return cur;
}

bool AdvancedBoolScheme::Tree::TreeStackIterator::hasNext() const {
    return !stack.empty();
}

// функция заполнения дерева из переменной BaseBoolScheme::vars
void AdvancedBoolScheme::Tree::fillFromVars(const AdvancedBoolScheme &bs) {
    std::unordered_map<std::string, std::vector<std::string>> map;
    std::copy(bs.vars.begin(), bs.vars.end(), std::inserter(map, map.begin()));
    std::stack<std::shared_ptr<Node>> stack;
    auto curr = bs.vars[bs.vars.size() - 1];
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
int AdvancedBoolScheme::Tree::getOperNumber(std::shared_ptr<Tree::Node> node) {
    int num = 0;
    if (node->first != nullptr) num++;
    if (node->second != nullptr) num++;
    if (node->operationName == "") return 0;
    return num;
}

// функция сравнения двух поддеревьев для определения их одинаковости
// В данной реализации поддеревья одинаковые <=> у каждых соответсвующих узлов одинаковая логическая операция 
// и одинаковое кол-во аргументов. Для листов очевидно иначе, листы одинаковые <=> одинаковые их имена
bool AdvancedBoolScheme::Tree::compareNodes(std::shared_ptr<Tree::Node> first, std::shared_ptr<Tree::Node> second) {
    Tree::TreeStackIterator iter1 = {first};
    Tree::TreeStackIterator iter2 = {second};
    int t = 0;
    while(iter1.hasNext() && iter2.hasNext()) {
        ++t;
        if (t > 30) {
            return 0;
        }
        auto i1 = iter1.next();
        auto i2 = iter2.next();
        int num1 = getOperNumber(i1), num2 = getOperNumber(i2);
        if (num1 != num2) return 0;
        if (i1->operationName != i2->operationName) return 0;
        if (num1 == 0 && i1->name != i2->name) return 0;
    }
    // if (flag) std::cout << first->name << " " << second->name << " " << t << "\n";
    return !(iter1.hasNext() || iter2.hasNext());
}


// функция, которая убирает повторяющиеся гейты
// Упрощение схемы заключается в поиске одинаковых по структуре поддеревьев логической схемы.
// При обнаружении одинаковых поддеревьев имя переменной первого поддерева становится именем корня второго 
// и становится листом. И так далее, пока в дереве есть что упрощать. Поддеревья с кол-вом элементов > 100 не рассматриваются.
void AdvancedBoolScheme::deleteRepetition(Tree &tree) {
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
        bool whileContinue = false;
        for (size_t i = 0; i < v.size(); ++i) {
            auto cur = v[i];
            if (Tree::getOperNumber(cur) == 0) continue;
            for (size_t j = i + 1; j < v.size(); ++j) {
                auto next = v[j];
                if (Tree::getOperNumber(next) == 0) continue;
                if (cur->name != next->name && Tree::compareNodes(cur, next)) {
                    
                    Tree::TreeStackIterator iter1 = {cur};
                    Tree::TreeStackIterator iter2 = {next};
                    std::unordered_map<std::string, std::string> newNodeNames;
                    newNodeNames[next->name] = cur->name;
                    while(iter1.hasNext() && iter2.hasNext()) {
                        auto i1 = iter1.next();
                        auto i2 = iter2.next();
                        int num1 = Tree::getOperNumber(i1);
                        if (num1 == 1 && i1->first->name == i2->first->name) break;
                        if (num1 == 2 && (i1->first->name == i2->first->name && i1->second->name == i2->second->name)) break;
                        if (num1 == 2 && (i1->first->name == i2->second->name && i1->second->name == i2->first->name)) break;
                        if (i1->name != i2->name) {
                            newNodeNames[i2->name] = i1->name;
                        }
                    }
                    
                    std::string oldName = next->name;
                    for (size_t k = 0; k < v.size(); ++k) {
                        auto next2 = v[k];
                        if (newNodeNames.find(next2->name) != newNodeNames.end()) {
                            next2->name = newNodeNames[next2->name];
                        }
                    }
                    whileContinue = true;
                    break;
                }
            }
            if (whileContinue) break;
        }
        if (!whileContinue) break;
        auto timeFinish = std::chrono::high_resolution_clock::now();
        auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(timeFinish - timeStart).count();
        if (deltaTime > 10'000) {
            break;
        } else {
            iter = {tree.root};
            v.clear();
            while (iter.hasNext()) {
                v.push_back(iter.next());
            } 
        }
    }
}

// запись упрощенного дерева в новый файл формата *.bench 
void AdvancedBoolScheme::writeToFile(AdvancedBoolScheme::Tree &t, const std::string &oldFile, const std::string &newFile) {
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
            } else {
                s.push_back(m[cur][1]);
            }
        } else if (m[cur].size() == 3) {
            bool flag1 = m.find(m[cur][1]) == m.end(), flag2 = m.find(m[cur][2]) == m.end();
            if (flag1 && flag2) {
                fout  << cur << " = " << m[cur][0] << "(" << m[cur][1] <<  ", " << m[cur][2] << ")\n";
                s.pop_back();
                m.erase(cur);
            } else {
                if (!flag1) {
                    s.push_back(m[cur][1]);
                }
                if (!flag2) {
                    s.push_back(m[cur][2]);
                }
            }
        }
    }
    fout.close();
}

// функция упрощения текущей булевы схемы, выдающая название нового файла
std::string AdvancedBoolScheme::simplification(std::string fileName) {
    Tree tree;
    tree.fillFromVars(*this);
    // std::cout << tree;
    deleteRepetition(tree);
    // std::cout << tree;
    int pfff = static_cast<int>(fileName.size()) - 1;
    for (; pfff >= 0; --pfff) if (fileName[pfff] == '/' || fileName[pfff] == '\\') break;
    std::string newFile = fileName.substr(0, pfff) + "/simplified/simplified_" + fileName.substr(pfff + 1, fileName.size() - pfff);
    writeToFile(tree, fileName, newFile);
    return newFile;
}

AdvancedBoolScheme::AdvancedBoolScheme(const std::string &fileName) : BaseBoolScheme(fileName) {
    [[maybe_unused]] size_t oldSize = vars.size();
    auto newFileName = simplification(fileName);
    vars.clear();
    inputs.clear();
    parseFile(newFileName);
    // std::cout << "Simplified by " << (oldSize - vars.size()) * 100 / oldSize << "%\n";
}

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