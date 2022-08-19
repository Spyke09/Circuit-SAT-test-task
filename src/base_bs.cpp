#include "base_bs.h"


// функция парсинга аргументов функции
// для функции func(var2, var3) выдаст вектор {"func", "var1", "var2"}
std::vector<std::string> BaseBoolScheme::parseFuncArgs(const std::string &st) const noexcept{
    std::vector<std::string> result;
    size_t lbracket = st.find('('), rbracket = st.find(')');
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

std::pair<std::string, std::string> BaseBoolScheme::parseVars(const std::string &s) {
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
void BaseBoolScheme::parseFile(const std::string &filename) {
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

// конструктор, аргумент - имя файла формата *.bench
BaseBoolScheme::BaseBoolScheme(const std::string &fileName) {
    parseFile(fileName);
}

// функция вычисления значения функции по функции и аргументам, которые в виде строк
bool BaseBoolScheme::calculateBooleanFunction(std::vector<std::string> expr, std::unordered_map<std::string, bool> &valuesMap) const {
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
bool BaseBoolScheme::calculate(std::vector<bool> numValues) const {
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
std::vector<std::string> BaseBoolScheme::getInputs() const {
    return inputs;
}

// функция выдающая копию переменной outputs
std::string BaseBoolScheme::getOutput() const{
    return output;
}

// функция выдающая количество входных переменных
size_t BaseBoolScheme::getInputsSize() const {
    return inputs.size();
}

BaseBoolScheme::~BaseBoolScheme() {}