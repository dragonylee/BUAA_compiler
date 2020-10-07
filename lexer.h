#ifndef __LEXER_H__
#define __LEXER_H__

#include "head.h"
#include "error.h"

class Lexer
{
private:
    int nowline;
    char nowc;
    void getnowc();
    void ungetnowc();
public:
    Lexer();
    ~Lexer();
    // 读取一个token，返回token的类别码，如果读完了返回-1，如果无法识别返回0
    int get_token(token &nowtoken);
    std::vector<token> get_all_tokens();
};

#endif // __LEXER_H__
