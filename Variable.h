#ifndef CALCULATOR_VARIABLE_H
#define CALCULATOR_VARIABLE_H

struct Variable
{
    string name;
    double value;
    bool is_constant;

    Variable (string n, double v, bool is_const)
            : name{ n }, value{ v }, is_constant{ is_const }
    { }
};

#endif //CALCULATOR_VARIABLE_H
