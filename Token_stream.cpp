#include "Token_stream.h"
#include "constants.h"
#include "std_lib_facilities.h"
#include <iostream>

using namespace std;

void Token_stream::putback (Token t)
{
    if (full)
        error("putback() into a full buffer");

    buffer = t;
    full = true;
}

Token Token_stream::get ()
{
    if (full)
    {
        full = false;
        return buffer;
    }

    char ch = cin.get(); // get sole symbol including spaces. cin by the default skips spaces

    while (isspace(ch))
    {
        if (ch == '\n') return Token { print };
        ch = cin.get(); // skip the rest space symbols
    }

    switch (ch)
    {
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
            if (ch == '.') // handle case when only '.' entered
            {
                char next_symbol = cin.get();
                if (!isdigit(next_symbol))
                {
                    cin.putback(next_symbol); // e.g. if symbol was ';' and if don't put it back => ts.ignore works bad
                    error("Bad token");
                }
                cin.putback(next_symbol); // putting digit back for further reading!!!
            }

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
                while (cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_'))
                    s += ch;
                cin.putback(ch);

                if (s == declkey) return Token{ let };
                if (s == constkey) return Token{ constant };
                if (s == helpkey) return Token { help };
                if (s == quitkey) return Token { quit };

                return Token{ name, s };
            }
            error("Bad token");
    }
}

void Token_stream::ignore (char a, char b)
{
    if (full && (a == buffer.kind || b == buffer.kind))
    {
        full = false;
        return;
    }
    full = false;

    char ch;
    while ((ch = cin.get()))
        if (ch == a || ch == b) return;
}

