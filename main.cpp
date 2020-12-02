#include "std_lib_facilities.h"

using namespace std;

struct Token
{
    char kind;
    double value;
    string name;

    Token (char ch)
            : kind{ ch }, value{ 0 }
    { }

    Token (char ch, double val)
            : kind{ ch }, value{ val }
    { }

    Token (char ch, string s)
        : kind{ ch }, name{ s }
    { }
};


class Token_stream
{
    bool full{ false };
    Token buffer{ '\0' };

public:
    Token_stream () { }

    Token get ();
    void putback (Token t);

    void ignore (char); // ignore input up to the defined symbol
};


void Token_stream::putback (Token t)
{
    if (full)
        error("putback() into a full buffer");

    buffer = t;
    full = true;
}


constexpr char quit = 'q';
constexpr char print = ';';
constexpr char number = '8';
constexpr char name = 'n';
constexpr char let = 'L';
constexpr char constant = 'C';

const string prompt = "> ";
const string result = "= ";
const string declkey = "let";
const string constkey = "const";


Token Token_stream::get ()
{
    if (full)
    {
        full = false;
        return buffer;
    }

    char ch;
    cin >> ch;

    switch (ch)
    {
        case quit:
        case print:
        case '(':
        case ')':
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '=':
            return Token{ ch };

        case '.':
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        {
            cin.putback(ch);
            double val;
            cin >> val;
            return Token{ number, val };
        }

        default:
            if (isalpha(ch))
            {
                string s;
                s += ch;
                while (cin.get(ch) &&
                       (isalpha(ch) || isdigit(ch) || ch == '_'))
                    s += ch;
                cin.putback(ch);

                if (s == declkey) return Token{ let };
                if (s == constkey) return Token{ constant };

                return Token{ name, s };
            }
            error("Bad token");
    }
}


void Token_stream::ignore (char c)
{
    if (full && c == buffer.kind)
    {
        full = false;
        return;
    }
    full = false;

    char ch;
    while (cin >> ch)
        if (ch == c) return;
}


struct Variable
{
    string name;
    double value;
    bool is_constant;

    Variable (string n, double v, bool is_const = false)
            : name{ n }, value{ v }, is_constant{ is_const }
    { }
};

vector<Variable> var_table;

double get_value (string s)
{
    for (int i = 0; i < var_table.size(); ++i)
        if (var_table[i].name == s)
            return var_table[i].value;

    error("get: undefined name ", s);
}

double set_value (string s, double d)
{
    for (int i = 0; i <= var_table.size(); ++i)
    {
        if (var_table[i].name == s && !var_table[i].is_constant)
        {
            var_table[i].value = d;
            return d;
        }
        else
        {
            error("set: unable to change constant ", s);
        }
    }

    error("set: undefined name ", s);
}

bool is_declared (string s)
{
    for (int i = 0; i < var_table.size(); ++i)
        if (var_table[i].name == s) return true;
    return false;
}

double define_name (string var, double val, bool is_const)
{
    if (is_declared(var))
        error(var, "define_name: declared twice");

    var_table.push_back (Variable{ var, val, is_const});

    return val;
}


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
            return get_value(t.name);

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
    if (is_declared(var))
        error(var, " declared twice");

    t = ts.get();
    if (t.kind != '=')
        error("'=' missing in declaration of ", var);

    return define_name (var, expression(), is_const);
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
                return set_value(t.name, expression());
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

            ts.putback(t);
            cout << result << statement() << endl;
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
    define_name ("pi", 3.141592653589793, true);
    define_name ("e",  2.718281828459045, true);

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
