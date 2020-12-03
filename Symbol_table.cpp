#include "Symbol_table.h"

double Symbol_table::get(string s) const {
    for (int i = 0; i < var_table.size(); ++i)
        if (var_table[i].name == s)
            return var_table[i].value;

    error("get: undefined name ", s);
}

double Symbol_table::set(string s, double d)
{
    for (int i = 0; i <= var_table.size(); ++i)
    {
        if (var_table[i].name == s && !var_table[i].is_constant)
        {
            var_table[i].value = d;
            return d;
        }
        else if (var_table[i].name == s && var_table[i].is_constant)
        {
            error("set: unable to change constant ", s);
        }
    }

    error("set: undefined name ", s);
}

bool Symbol_table::is_declared (string s)
{
    for (int i = 0; i < var_table.size(); ++i)
        if (var_table[i].name == s) return true;
    return false;
}

double Symbol_table::define (string var, double val, bool is_const)
{
    if (is_declared(var))
        error(var, "define_name: declared twice");

    var_table.push_back (Variable{ var, val, is_const});

    return val;
}