#ifndef CALCULATOR_TOKEN_STREAM_H
#define CALCULATOR_TOKEN_STREAM_H

#include "Token.h"

class Token_stream
{
    bool full{ false };
    Token buffer{ '\0' };

public:
    Token_stream () { }

    Token get ();
    void putback (Token t);

    void ignore (char a, char b); // ignore input up to the defined symbol
};

#endif //CALCULATOR_TOKEN_STREAM_H
