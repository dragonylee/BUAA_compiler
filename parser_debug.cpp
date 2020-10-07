#include "parser.h"

void Parser::print_all_tokens()
{
    for(token t:tokens)
    {
        print_token(t);
    }
}
