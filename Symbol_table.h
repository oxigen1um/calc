#ifndef CALCULATOR_SYMBOL_TABLE_H
#define CALCULATOR_SYMBOL_TABLE_H
#include "std_lib_facilities.h"
#include "Variable.h"


class Symbol_table {
public:

    double get(string s);
    double set(string s, double d);
    bool is_declared(string s);
    double define(string var, double val, bool is_const);

private:
    vector<Variable> var_table;
};


#endif //CALCULATOR_SYMBOL_TABLE_H
