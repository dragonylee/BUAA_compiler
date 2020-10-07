#include "head.h"
#include "lexer.h"

Lexer::Lexer()
{
    nowline=1;
    nowc=' ';
}

Lexer::~Lexer()
{
    //
}

void Lexer::getnowc() {nowc=getchar();}
void Lexer::ungetnowc() {ungetc(nowc,stdin);}

int Lexer::get_token(token &nowtoken)
{
    std::string nows;
    // 跳过空白字符，同时处理行号
    while(!nowc || is_space(nowc) || is_newline(nowc))
    {
        if(is_newline(nowc)) nowline++;
        getnowc();
    }
    if(nowc==-1)
    {
        nowtoken.type=0;
        return -1;
    }
    // 首字母为letter，可能是标识符或者保留字
    if(is_letter(nowc))
    {
        while(is_letter(nowc) || is_digit(nowc))
        {
            nows+=nowc;
            getnowc();
        }
        int x=is_reserverd_word(nows);
        if(x>0)
        {
            nowtoken=token(x,nows,nowline);
            return x;
        }
        else
        {
            nowtoken=token(IDENFR,nows,nowline);
            return IDENFR;
        }
    }
    // 首字母为数字，可能是整型常量
    else if(is_digit(nowc))
    {
        while(is_digit(nowc))
        {
            nows+=nowc;
            getnowc();
        }
        nowtoken=token(INTCON,nows,nowline);
        return INTCON;
    }
    // 首字母为 '\'' ，可能是字符常量
    else if(nowc=='\'')
    {
        getnowc();
        if(!is_valid_char(nowc))
        {
            error();
        }
        nows+=nowc;
        getnowc();
        if(nowc!='\'')
        {
            error();
        }
        getnowc();
        nowtoken=token(CHARCON,nows,nowline);
        return CHARCON;
    }
    // 首字母为 '\"'，可能是字符串
    else if(nowc=='\"')
    {
        getnowc();
        while(is_valid_str(nowc) && nowc!='\"')
        {
            nows+=nowc;
            getnowc();
        }
        if(nows.length()==0)
        {
            error();
        }
        if(nowc!='\"')
        {
            error();
        }
        getnowc();
        nowtoken=token(STRCON,nows,nowline);
        return STRCON;
    }
    // 这些是仅凭一个符号就可以唯一标识的，包括：
    // 加、减、乘、除、冒号、分号、逗号、六个括号
    else if(is_plus(nowc) || is_minu(nowc) || is_mult(nowc) || is_div(nowc)
            || is_colon(nowc) || is_semicn(nowc) || is_comma(nowc) || is_lparent(nowc)
            || is_rparent(nowc) || is_lbrack(nowc) || is_rbrack(nowc)
            || is_lbrace(nowc) || is_rbrace(nowc))
    {
        nows+=nowc;
        int x=0;
        if(is_plus(nowc)) x=PLUS;
        else if(is_minu(nowc)) x=MINU;
        else if(is_mult(nowc)) x=MULT;
        else if(is_div(nowc)) x=DIV;
        else if(is_colon(nowc)) x=COLON;
        else if(is_semicn(nowc)) x=SEMICN;
        else if(is_comma(nowc)) x=COMMA;
        else if(is_lparent(nowc)) x=LPARENT;
        else if(is_rparent(nowc)) x=RPARENT;
        else if(is_lbrack(nowc)) x=LBRACK;
        else if(is_rbrack(nowc)) x=RBRACK;
        else if(is_lbrace(nowc)) x=LBRACE;
        else if(is_rbrace(nowc)) x=RBRACE;
        getnowc();
        nowtoken=token(x,nows,nowline);
        return x;
    }
    // 首字母为 '<'，可能是小于或者小于等于
    else if(is_lss(nowc))
    {
        nows+=nowc;
        int x=LSS;
        getnowc();
        if(is_eql(nowc))
        {
            nows+=nowc;
            x=LEQ;
            getnowc();
        }
        nowtoken=token(x,nows,nowline);
        return x;
    }
    // 首字母为 '>'，可能是大于或者大于等于
    else if(is_gre(nowc))
    {
        nows+=nowc;
        int x=GRE;
        getnowc();
        if(is_eql(nowc))
        {
            nows+=nowc;
            x=GEQ;
            getnowc();
        }
        nowtoken=token(x,nows,nowline);
        return x;
    }
    // 首字母为 '!'，可能是不等于
    else if(is_neq(nowc))
    {
        nows+=nowc;
        getnowc();
        if(!is_eql(nowc))
        {
            error();
        }
        nows+=nowc;
        getnowc();
        nowtoken=token(NEQ,nows,nowline);
        return NEQ;
    }
    // 首字母为 '='，可能是赋值或者双等号
    else if(is_eql(nowc))
    {
        nows+=nowc;
        int x=ASSIGN;
        getnowc();
        if(is_eql(nowc))
        {
            nows+=nowc;
            x=EQL;
            getnowc();
        }
        nowtoken=token(x,nows,nowline);
        return x;
    }
    else
    {
        error();
        return 0;
    }
}

std::vector<token> Lexer::get_all_tokens()
{
    std::vector<token> ret;
    token t;
    int r;
    while((r=get_token(t))!=-1)
    {
        if(r>0) ret.push_back(t);
    }
    return ret;
}
