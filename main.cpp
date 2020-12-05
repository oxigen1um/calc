#include "std_lib_facilities.h"
#include "Token.h"
#include "Token_stream.h"
#include "Variable.h"
#include "Symbol_table.h"
#include "constants.h"

using namespace std;

ostream& syncerr = *cerr.tie(); // synchronize cerr with cout and save it as link for further purposes

Symbol_table variables;
Token_stream ts;

void print_help()
{
    vector<Variable> v = variables.get_variables();

    cout << "CLI Calculator\n\n"
    << "Available commands:\n"
    << "1) 'quit' - exit program\n"
    << "2) 'help' - print manual\n"
    << "3) use 'let' and 'const' to define variable or constant respectively\n"
    << "4) put ';' or '\\n' at the end of statement to calculate it\n\n"
    << "Table of variables and constants:\n"
    << "Name | Type | Value\n"
    << "______________________________________\n";
    for (const Variable& x : v)
    {
        string type = (x.is_constant) ? "constant" : "variable";
        cout << setw(5) << x.name << " | " << setw(5) <<  type << " | " << setw(5) << x.value <<  '\n';
        cout << "--------------------------------------\n";
    }

}


double expression (); // function declaration in order to use it before it's definition

double primary ()
{
    Token t = ts.get();
    switch (t.kind)
    {
        case '(':
        {
            double d = expression();
            t = ts.get();
            if (t.kind != ')')
                error("'(' expected");
            return d;
        }

        case '-':
            return -primary();
        case '+':
            return +primary();

        case number:
            return t.value;

        case name:
            return variables.get(t.name);

        default:
            error("primary expected");
    }
}


double term ()
{
    double left = primary();
    while (true)
    {
        Token t = ts.get();

        switch (t.kind)
        {
            case '*':
                left *= primary();
                break;

            case '/':
            {
                double d = primary();
                if (d == 0) error("/: divide by zero");
                left /= d;
                break;
            }

            case '%': // get the rest of division
            {
                double d = primary();
                if (d == 0) error("%: divide by zero");
                left = fmod(left, d);
                t = ts.get();
                break;
            }


            default:
                ts.putback(t);
                return left;
        }
    }
}


double expression ()
{
    double left = term();
    while (true)
    {
        Token t = ts.get();

        switch (t.kind)
        {
            case '+':
                left += term();
                break;

            case '-':
                left -= term();
                break;

            default:
                ts.putback(t);
                return left;
        }
    }
}


double declaration (bool is_const)
{
    Token t = ts.get();
    if (t.kind != name)
        error("name expected in declaration");

    string var = t.name;
    if (variables.is_declared(var))
        error(var, " declared twice");

    t = ts.get();
    if (t.kind != '=')
        error("'=' missing in declaration of ", var);

    return variables.define (var, expression(), is_const);
}


double statement ()
{
    Token t = ts.get();
    switch (t.kind)
    {
        case let:
            return declaration(false);
        case constant:
            return declaration(true);
        case name:
        {
            Token tt = ts.get(); // get next symbol and check it

            if (tt.kind == '=')
                return variables.set(t.name, expression());
            cin.putback(tt.kind);
        }
        default:
            ts.putback(t);
            return expression();
    }
}


void clean_up_mess ()
{
    ts.ignore (print, '\n');
}


void calculate ()
{
    while (cin)
        try {
            cout << prompt;
            Token t = ts.get();

            while (t.kind == print) // 'eat' all ';' symbols
                t = ts.get();

            if (t.kind == quit) return;

            if (t.kind == help)
                print_help();
            else
            {
                ts.putback(t);
                double value = statement();
                cout << result << value << endl;
            }

        }
        catch (exception& e)
        {
            syncerr << e.what() << endl;
            clean_up_mess();
        }
}


int main ()
{
    try
    {
        variables.define ("pi", 3.141592653589793, true);
        variables.define ("e",  2.718281828459045, true);

        calculate();
    }
    catch (exception& e)
    {
        cerr << "exception: " << e.what() << endl;
        return 1;
    }
    catch (...)
    {
        cerr << "Oops, unknown exception" << endl;
        return 2;
    }
}

