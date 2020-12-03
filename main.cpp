#include "std_lib_facilities.h"
#include "Token.h"
#include "Token_stream.h"
#include "Variable.h"
#include "Symbol_table.h"
#include "constants.h"

using namespace std;

void print_help()
{
    cout << "CLI Calculator\n\n"
    << "Available commands:\n"
    << "1) 'quit' - exit program\n"
    << "2) 'help' - print manual\n"
    << "3) put ';' or '\\n' at the end of statement to calculate it\n";
}

Symbol_table variables;

Token_stream ts;

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
                if (d == 0) error("divide by zero");
                left /= d;
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
            Token tt = ts.get();
            if (tt.kind != '=')
                cin.putback(tt.kind);
            else if (tt.kind == '=')
                return variables.set(t.name, expression());
        }
        default:
            ts.putback(t);
            return expression();
    }
}


void clean_up_mess ()
{
    ts.ignore (print);
}


void calculate ()
{
    while (true)
        try
        {
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
                cout << result << statement() << endl;
            }
        }
        catch (runtime_error& e)
        {
            cerr << e.what() << endl;
            clean_up_mess();
        }
}


int main ()
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
