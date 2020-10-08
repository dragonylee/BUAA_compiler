#include "parser.h"

Parser::Parser()
{
    tokens=Lexer().get_all_tokens();
    tokens_num=tokens.size();
    cur=0;
    get_nowtoken();
}

int Parser::get_nowtoken()
{
    if(cur>0 && cur<=tokens_num) print_token(tokens[cur-1]);
    if(cur>=tokens_num)
    {
        nowtoken.type=0;
        return -1;
    }
    nowtoken=tokens[cur++];
    return 1;
}

// 符号表相关函数
/*
语法分析时涉及到的符号表插入操作有：
1、常量定义
2、变量定义无初始化
3、变量定义及初始化
4、有返回值函数定义
5、无返回值函数定义
*/
bool Parser::insert_identifier(int type,std::string word)
{
    table.push_back(identifier(type,word));
    return true;    // 这里没做查重处理
}

int Parser::get_identifier_type(std::string word)
{
    for(identifier id:table)
    {
        if(id.word==word)
            return id.type;
    }
    return -1;
}

// 各个子处理程序
/*
尽管每个子程序处理方法不尽相同，但是都有一些共性：
1、在处理过程中可能要判断nowtoken是否已经读完
2、需要进行打印、error等操作
3、对于必须出现的，用if-statement-else-error这种格式；
    对于可能出现一次的（也就是[]里面的），用if这种格式；
    对于可能出现多次的（也就是{}里面的），用while这种格式
*/

// <加法运算符> ::= +|-
void Parser::jiafayunsuanfu()
{
    if(nowtoken.type!=PLUS && nowtoken.type!=MINU)
    {
        error();
        get_nowtoken();
    }
    else
    {
        get_nowtoken();
        grammer g(JIAFAYUNSUANFU);
        //
    }
}

// <乘法运算符> ::= *|/
void Parser::chengfayunsuanfu()
{
    if(nowtoken.type!=MULT && nowtoken.type!=DIV)
    {
        error();
        get_nowtoken();
    }
    else
    {
        get_nowtoken();
        grammer g(CHENGFAYUNSUANFU);
    }
}

// <关系运算符> ::= <|<=|>|>=|!=|==
void Parser::guanxiyunsuanfu()
{
    if(!(nowtoken.type==LSS || nowtoken.type==LEQ || nowtoken.type==GRE || nowtoken.type==GEQ || nowtoken.type==NEQ || nowtoken.type==EQL))
    {
        error();
        get_nowtoken();
    }
    else
    {
        get_nowtoken();
        grammer g(GUANXIYUNSUANFU);
    }
}

// <字符> ::= CHARCON
void Parser::zifu()
{
    if(nowtoken.type!=CHARCON)
    {
        error();
        get_nowtoken();
    }
    else
    {
        get_nowtoken();
        grammer g(ZIFU);
        //print_grammer(g);
    }
}

// <字符串> ::= STRCON
void Parser::zifuchuan()
{
    if(nowtoken.type!=STRCON)
    {
        error();
        get_nowtoken();
    }
    else
    {
        get_nowtoken();
        grammer g(ZIFUCHUAN);
        print_grammer(g);
    }
}

// <程序> ::= [<常量说明>][<变量说明>]{<有返回值函数定义>|<无返回值函数定义>}<主函数>
// <常量说明>的首符号集是{CONSTTK}
// <变量说明>的首符号集是{INTTK,CHARTK}
// <有返回值函数定义>的首符号集是{INTTK,CHARTK}
// <无返回值函数定义>的首符号集是{VOIDTK}
// <主函数>的首符号集是{VOIDTK}
/*
然后处理步骤如下：
如果当前token的type是CONSTTK，进入<常量说明>；
如果当前是{INTTK,CHARTK}，且后两个的token的type不是LPARENT，进入<变量说明>（注意这个和后面的<有返回值函数定义>可能会有冲突，后面要注意）；
（可能若干次）
{
    如果当前是{INTTK,CHARTK}，且后两个的token的type是LPARENT，进入<有返回值函数定义>；
    如果当前是VOIDTK，且后一个的token的type不是MAINTK，进入<无返回值函数定义>；
}
进入<主函数>；
*/
void Parser::chengxu()
{
    if(nowtoken.type==CONSTTK)
    {
        changliangshuoming();   // 常量说明
    }
    if((nowtoken.type==INTTK || nowtoken.type==CHARTK) &&
       (cur+1<tokens_num && tokens[cur+1].type!=LPARENT))
    {
        bianliangshuoming();    // 变量说明
    }
    while(((nowtoken.type==INTTK || nowtoken.type==CHARTK) && (cur+1<tokens_num && tokens[cur+1].type==LPARENT))
        || (nowtoken.type==VOIDTK && cur<tokens_num && tokens[cur].type!=MAINTK))
    {
        if((nowtoken.type==INTTK || nowtoken.type==CHARTK) && (cur+1<tokens_num && tokens[cur+1].type==LPARENT))
        {
            youfanhuizhihanshudingyi(); // 有返回值函数定义
        }
        else if(nowtoken.type==VOIDTK && cur<tokens_num && tokens[cur].type!=MAINTK)
        {
            wufanhuizhihanshudingyi();  // 无返回值函数定义
        }
    }
    zhuhanshu();    // 主函数

    grammer g(CHENGXU);
    print_grammer(g);
}

// <常量说明> ::= const<常量定义>;{const<常量定义>;}
void Parser::changliangshuoming()
{
    if(nowtoken.type!=CONSTTK)
    {
        error();
    }
    while(nowtoken.type==CONSTTK)
    {
        get_nowtoken();
        changliangdingyi(); // 常量定义
        if(nowtoken.type!=SEMICN)
        {
            error();
        }
        get_nowtoken();
    }
    grammer g(CHANGLIANGSHUOMING);
    print_grammer(g);
}

// <常量定义> ::= int<标识符>＝<整数>{,<标识符>＝<整数>}|char<标识符>＝<字符>{,<标识符>＝<字符>}
// 两个选择的首符号集不相交，且都是只出现一次
void Parser::changliangdingyi()
{
    if(nowtoken.type==INTTK)
    {
        get_nowtoken();
        std::string wordtemp=nowtoken.word;
        if(biaoshifu()==true)   // --------------------符号表操作
        {
            insert_identifier(ZHENGXINGCHANGLIANG,wordtemp);
        }
        if(nowtoken.type!=ASSIGN)
        {
            error();
        }
        get_nowtoken();
        zhengshu();
        while(nowtoken.type==COMMA)
        {
            get_nowtoken();
            std::string wordtemp=nowtoken.word;
            if(biaoshifu()==true)   // --------------------符号表操作
            {
                insert_identifier(ZHENGXINGCHANGLIANG,wordtemp);
            }
            if(nowtoken.type!=ASSIGN)
            {
                error();
            }
            get_nowtoken();
            zhengshu();
        }
    }
    else if(nowtoken.type==CHARTK)
    {
        get_nowtoken();
        std::string wordtemp=nowtoken.word;
        if(biaoshifu()==true)   // --------------------符号表操作
        {
            insert_identifier(ZIFUCHANGLIANG,wordtemp);
        }
        if(nowtoken.type!=ASSIGN)
        {
            error();
        }
        get_nowtoken();
        zifu();
        while(nowtoken.type==COMMA)
        {
            get_nowtoken();
            std::string wordtemp=nowtoken.word;
            if(biaoshifu()==true)   // --------------------符号表操作
            {
                insert_identifier(ZIFUCHANGLIANG,wordtemp);
            }
            if(nowtoken.type!=ASSIGN)
            {
                error();
            }
            get_nowtoken();
            zifu();
        }
    }
    else
    {
        error();
        return;
    }
    grammer g(CHANGLIANGDINGYI);
    print_grammer(g);
}

// <无符号整数> ::= <数字>{<数字>}
// <无符号整数> ::= INTCON
void Parser::wufuhaozhengshu()
{
    if(nowtoken.type!=INTCON)
    {
        error();
        get_nowtoken();
    }
    else
    {
        get_nowtoken();
        grammer g(WUFUHAOZHENGSHU);
        print_grammer(g);
    }
}

// <整数> ::= [+|-]<无符号整数>
void Parser::zhengshu()
{
    if(nowtoken.type==PLUS || nowtoken.type==MINU)
    {
        jiafayunsuanfu();
    }
    wufuhaozhengshu();

    grammer g(ZHENGSHU);
    print_grammer(g);
}

// <标识符> ::= IDENFR
bool Parser::biaoshifu()
{
    bool ret=0;
    if(nowtoken.type!=IDENFR)
    {
        error();
        get_nowtoken();
    }
    else
    {
        get_nowtoken();
        grammer g(BIAOSHIFU);
        //print_grammer(g);
        ret=1;
    }
    return ret;
}

// <声明头部> ::= int<标识符>|char<标识符>
void Parser::shengmingtoubu()
{
    if(nowtoken.type==INTTK || nowtoken.type==CHARTK)
    {
        get_nowtoken();
        biaoshifu();
    }
    else
    {
        error();
    }
    grammer g(SHENGMINGTOUBU);
    print_grammer(g);
}

// <常量> ::= <整数>|<字符>
// 字符的首符号集为{CHARCON}
// 整数的首符号集为{PLUS,MINU,INTCON}
void Parser::changliang()
{
    if(nowtoken.type==INTCON || nowtoken.type==PLUS || nowtoken.type==MINU)
    {
        zhengshu();
    }
    else if(nowtoken.type==CHARCON)
    {
        zifu();
    }
    else
    {
        error();
        return;
    }

    grammer g(CHANGLIANG);
    print_grammer(g);
}

// <变量说明> ::= <变量定义>;{<变量定义>;}
/*
处理步骤：
如果当前是{INTTK,CHARTK}，且后两个的token的type不是LPARENT，进入bianliangdingyi，
否则error
（可能若干次）如果当前是{INTTK,CHARTK}，且后两个的token的type不是LPARENT，进入bianliangdingyi，
*/
void Parser::bianliangshuoming()
{
    if((nowtoken.type==INTTK || nowtoken.type==CHARTK) &&
       (cur+1<tokens_num && tokens[cur+1].type!=LPARENT))   // cur+1必须有，否则一定错
    {
        bianliangdingyi();
        if(nowtoken.type!=SEMICN)
        {
            error();
        }
        get_nowtoken();
    }
    else
    {
        error();
        return;
    }
    while((nowtoken.type==INTTK || nowtoken.type==CHARTK) &&
       (cur+1<tokens_num && tokens[cur+1].type!=LPARENT))
    {
        bianliangdingyi();
        if(nowtoken.type!=SEMICN)
        {
            error();
        }
        get_nowtoken();
    }
    grammer g(BIANLIANGSHUOMING);
    print_grammer(g);
}

// <变量定义> ::= <变量定义无初始化>|<变量定义及初始化>
/*
处理过程：
如果以下这个条件：
    该条件为真当且仅当以下三个条件中至少有一个为真：
        tokens[cur+1]为ASSIGN
        tokens[cur+1]为LBRACK 且 tokens[cur+4]为ASSIGN
        tokens[cur+1]为LBRACK 且 tokens[cur+4]为LBRACK 且 token[cur+7]为ASSIGN
为真，那么进入bianliangdingyijichushihua（<变量定义及初始化>）
否则进入bianliangdingyiwuchushihua（<变量定义无初始化>）
*/
void Parser::bianliangdingyi()
{
    if((cur+1<tokens_num && tokens[cur+1].type==ASSIGN) ||
       (cur+4<tokens_num && tokens[cur+1].type==LBRACK && tokens[cur+4].type==ASSIGN) ||
       (cur+7<tokens_num && tokens[cur+1].type==LBRACK && tokens[cur+4].type==LBRACK && tokens[cur+7].type==ASSIGN))
    {
        bianliangdingyijichushihua();
    }
    else
    {
        bianliangdingyiwuchushihua();
    }

    grammer g(BIANLIANGDINGYI);
    print_grammer(g);
}

// <变量定义无初始化> ::= <类型标识符>(<标识符>|<标识符>'['<无符号整数>']'|<标识符>'['<无符号整数>']''['<无符号整数>']'){,(<标识符>|<标识符>'['<无符号整数>']'|<标识符>'['<无符号整数>']''['<无符号整数>']')}
void Parser::bianliangdingyiwuchushihua()
{
    int typetemp;
    std::string wordtemp;

    typetemp=nowtoken.type; // --------------------符号表操作
    leixingbiaoshifu();
    wordtemp=nowtoken.word;
    if(!biaoshifu()) typetemp=-1;

    int lnum=0;
    while(nowtoken.type==LBRACK)
    {
        lnum++;
        get_nowtoken();
        wufuhaozhengshu();
        if(nowtoken.type!=RBRACK)
        {
            error();
        }
        get_nowtoken();
    }
    if(typetemp==INTTK)     // --------------------符号表操作
    {
        if(lnum==0) insert_identifier(ZHENGXINGBIANLIANG,wordtemp);
        else if(lnum==1) insert_identifier(YIWEIZHENGXING,wordtemp);
        else if(lnum==2) insert_identifier(ERWEIZHENGXING,wordtemp);
    }
    else if(typetemp==CHARTK)   // --------------------符号表操作
    {
        if(lnum==0) insert_identifier(ZIFUBIANLIANG,wordtemp);
        else if(lnum==1) insert_identifier(YIWEIZIFU,wordtemp);
        else if(lnum==2) insert_identifier(ERWEIZIFU,wordtemp);
    }

    while(nowtoken.type==COMMA)
    {
        get_nowtoken();

        wordtemp=nowtoken.word;     // --------------------符号表操作
        if(!biaoshifu()) typetemp=-1;

        lnum=0;
        while(nowtoken.type==LBRACK)
        {
            lnum++;
            get_nowtoken();
            wufuhaozhengshu();
            if(nowtoken.type!=RBRACK)
            {
                error();
            }
            get_nowtoken();
        }
        if(typetemp==INTTK)     // --------------------符号表操作
        {
            if(lnum==0) insert_identifier(ZHENGXINGBIANLIANG,wordtemp);
            else if(lnum==1) insert_identifier(YIWEIZHENGXING,wordtemp);
            else if(lnum==2) insert_identifier(ERWEIZHENGXING,wordtemp);
        }
        else if(typetemp==CHARTK)   // --------------------符号表操作
        {
            if(lnum==0) insert_identifier(ZIFUBIANLIANG,wordtemp);
            else if(lnum==1) insert_identifier(YIWEIZIFU,wordtemp);
            else if(lnum==2) insert_identifier(ERWEIZIFU,wordtemp);
        }
    }

    grammer g(BIANLIANGDINGYIWUCHUSHIHUA);
    print_grammer(g);
}

// <变量定义及初始化> ::= <类型标识符><标识符>=<常量>|<类型标识符><标识符>'['<无符号整数>']'='{'<常量>{,<常量>}'}'|<类型标识符><标识符>'['<无符号整数>']''['<无符号整数>']'='{''{'<常量>{,<常量>}'}'{,'{'<常量>{,<常量>}'}'}'}'
void Parser::bianliangdingyijichushihua()
{
    int typetemp;
    std::string wordtemp;

    typetemp=nowtoken.type; // --------------------符号表操作
    leixingbiaoshifu();
    wordtemp=nowtoken.word;
    if(!biaoshifu()) typetemp=-1;

    if(nowtoken.type==ASSIGN)   // 变量初始化
    {
        get_nowtoken();
        changliang();
        // --------------------符号表操作
        if(typetemp==INTTK) insert_identifier(ZHENGXINGBIANLIANG,wordtemp);
        else if(typetemp==CHARTK) insert_identifier(ZIFUBIANLIANG,wordtemp);
    }
    else if(nowtoken.type==LBRACK)
    {
        get_nowtoken();
        wufuhaozhengshu();
        if(nowtoken.type!=RBRACK)
        {
            error();
        }
        get_nowtoken();
        if(nowtoken.type==ASSIGN)   // 一维数组初始化
        {
            // '{' <常量> {,<常量>} '}'
            get_nowtoken();
            if(nowtoken.type!=LBRACE)
            {
                error();
            }
            get_nowtoken();
            changliang();
            while(nowtoken.type==COMMA)
            {
                get_nowtoken();
                changliang();
            }
            if(nowtoken.type!=RBRACE)
            {
                error();
            }
            get_nowtoken();
            // --------------------符号表操作
            if(typetemp==INTTK) insert_identifier(YIWEIZHENGXING,wordtemp);
            else if(typetemp==CHARTK) insert_identifier(YIWEIZIFU,wordtemp);
        }
        else if(nowtoken.type==LBRACK)
        {
            get_nowtoken();
            wufuhaozhengshu();
            if(nowtoken.type!=RBRACK)
            {
                error();
            }
            get_nowtoken();
            if(nowtoken.type==ASSIGN)   // 二维数组初始化
            {
                // '{' '{' <常量>{,<常量>} '}' {,'{'<常量>{,<常量>} '}' }  '}'
                get_nowtoken();
                if(nowtoken.type!=LBRACE)
                {
                    error();
                }
                get_nowtoken();
                if(nowtoken.type!=LBRACE)
                {
                    error();
                }
                get_nowtoken();
                changliang();
                while(nowtoken.type==COMMA)
                {
                    get_nowtoken();
                    changliang();
                }
                if(nowtoken.type!=RBRACE)
                {
                    error();
                }
                get_nowtoken();
                while(nowtoken.type==COMMA)
                {
                    // { ,'{'<常量>{,<常量>} '}' }
                    get_nowtoken();
                    if(nowtoken.type!=LBRACE)
                    {
                        error();
                    }
                    get_nowtoken();
                    changliang();
                    while(nowtoken.type==COMMA)
                    {
                        get_nowtoken();
                        changliang();
                    }
                    if(nowtoken.type!=RBRACE)
                    {
                        error();
                    }
                    get_nowtoken();
                }
                if(nowtoken.type!=RBRACE)
                {
                    error();
                }
                get_nowtoken();

                // --------------------符号表操作
                if(typetemp==INTTK) insert_identifier(ERWEIZHENGXING,wordtemp);
                else if(typetemp==CHARTK) insert_identifier(ERWEIZIFU,wordtemp);
            }
            else
            {
                error();
            }
        }
        else
        {
            error();
        }
    }
    else
    {
        error();
    }

    grammer g(BIANLIANGDINGYIJICHUSHIHUA);
    print_grammer(g);
}

// <类型标识符> ::= int|char
void Parser::leixingbiaoshifu()
{
    if(!(nowtoken.word=="int" || nowtoken.word=="char"))
    {
        error();
        get_nowtoken();
    }
    else
    {
        get_nowtoken();
        grammer g(LEIXINGBIAOSHIFU);
        //print_grammer(g);
    }
}

// <有返回值函数定义> ::= <声明头部>'('<参数表>')' '{'<复合语句>'}'
void Parser::youfanhuizhihanshudingyi()
{
    std::string wordtemp=tokens[cur].word;

    shengmingtoubu();
    if(nowtoken.type!=LPARENT)
    {
        error();
    }
    get_nowtoken();
    canshubiao();
    if(nowtoken.type!=RPARENT)
    {
        error();
    }
    get_nowtoken();
    if(nowtoken.type!=LBRACE)
    {
        error();
    }
    get_nowtoken();
    fuheyuju();
    if(nowtoken.type!=RBRACE)
    {
        error();
    }
    get_nowtoken();

    // --------------------符号表操作
    insert_identifier(YOUFANHUIZHIHANSHU,wordtemp);

    grammer g(YOUFANHUIZHIHANSHUDINGYI);
    print_grammer(g);
}

// <无返回值函数定义> ::= void<标识符>'('<参数表>')''{'<复合语句>'}'
void Parser::wufanhuizhihanshudingyi()
{
    if(nowtoken.type!=VOIDTK)
    {
        error();
    }
    get_nowtoken();

    std::string wordtemp=nowtoken.word;
    biaoshifu();

    if(nowtoken.type!=LPARENT)
    {
        error();
    }
    get_nowtoken();
    canshubiao();
    if(nowtoken.type!=RPARENT)
    {
        error();
    }
    get_nowtoken();
    if(nowtoken.type!=LBRACE)
    {
        error();
    }
    get_nowtoken();
    fuheyuju();
    if(nowtoken.type!=RBRACE)
    {
        error();
    }
    get_nowtoken();

    // --------------------符号表操作
    insert_identifier(WUFANHUIZHIHANSHU,wordtemp);

    grammer g(WUFANHUIZHIHANSHUDINGYI);
    print_grammer(g);
}

// <复合语句> ::=[<常量说明>][<变量说明>]<语句列>
// <常量说明>的首符号集是{CONSTTK}
// <变量说明>的首符号集是{INTTK,CHARTK}
// <语句列>的首符号集有好多好多元素，但是和前两个都没有交集，所以直接处理
void Parser::fuheyuju()
{
    if(nowtoken.type==CONSTTK)
    {
        changliangshuoming();
    }
    if(nowtoken.type==INTTK || nowtoken.type==CHARTK)
    {
        bianliangshuoming();
    }
    yujulie();

    grammer g(FUHEYUJU);
    print_grammer(g);
}

// <参数表> ::= <类型标识符><标识符>{,<类型标识符><标识符>} | <空>
/*
因为可以为空，所以如果首个token不是类型标识符，也可以成立
*/
void Parser::canshubiao()
{
    if(nowtoken.type==INTTK || nowtoken.type==CHARTK)
    {
        leixingbiaoshifu();
        biaoshifu();
        while(nowtoken.type==COMMA)
        {
            get_nowtoken();
            leixingbiaoshifu();
            biaoshifu();
        }
    }

    grammer g(CANSHUBIAO);
    print_grammer(g);
}

// <主函数> ::= void main'('')' '{'<复合语句>'}'
void Parser::zhuhanshu()
{
    if(nowtoken.type!=VOIDTK)
    {
        error();
    }
    get_nowtoken();
    if(nowtoken.type!=MAINTK)
    {
        error();
    }
    get_nowtoken();
    if(nowtoken.type!=LPARENT)
    {
        error();
    }
    get_nowtoken();
    if(nowtoken.type!=RPARENT)
    {
        error();
    }
    get_nowtoken();
    if(nowtoken.type!=LBRACE)
    {
        error();
    }
    get_nowtoken();
    fuheyuju();
    if(nowtoken.type!=RBRACE)
    {
        error();
    }
    get_nowtoken();

    grammer g(ZHUHANSHU);
    print_grammer(g);
}

// <表达式> ::= [+|-]<项>{<加法运算符><项>}
void Parser::biaodashi()
{
    if(nowtoken.type==PLUS || nowtoken.type==MINU)
    {
        jiafayunsuanfu();
    }
    xiang();
    while(nowtoken.type==PLUS || nowtoken.type==MINU)
    {
        jiafayunsuanfu();
        xiang();
    }

    grammer g(BIAODASHI);
    print_grammer(g);
}

// <项> ::= <因子>{<乘法运算符><因子>}
void Parser::xiang()
{
    yinzi();
    while(nowtoken.type==MULT || nowtoken.type==DIV)
    {
        chengfayunsuanfu();
        yinzi();
    }

    grammer g(XIANG);
    print_grammer(g);
}

// <因子> ::= <标识符>|<标识符>'['<表达式>']'|<标识符>'['<表达式>']''['<表达式>']'|'('<表达式>')'|<整数>|<字符>|<有返回值函数调用语句>
/*
一共有如下若干种选择：
<标识符>
<标识符>'['<表达式>']'
<标识符>'['<表达式>']''['<表达式>']'
'('<表达式>')'
<整数>
<字符>
<有返回值函数调用语句>

处理过程如下：
如果nowtoken的type是IDENFR，且tokens[cur].type==LBRACK，那么先进入biaodashi，然后，
    - 如果后面跟着的两个token，第一个是RBRACK，第二个是LBRACK，再次进入biaodashi；
    - 如果只有一个RBRACK，结束
如果nowtoken的type是IDENFR，且tokens[cur].type==LPARENT，那么进入youfanhuizhihanshudiaoyongyuju
如果nowtoken的type是IDENFR，结束；
如果nowtoken的type是LPARENT，进入biaodashi，然后处理RPARENT；
如果nowtoken的type是PLUS、MINU或INTCON，进入zhengshu；
如果nowtoken的type是CHARCON，进入字符
*/
void Parser::yinzi()
{
    if(nowtoken.type==IDENFR)
    {
        if(cur<tokens_num && tokens[cur].type==LPARENT)
        {
            youfanhuizhihanshudiaoyongyuju();
        }
        else
        {
            biaoshifu();
            if(nowtoken.type==LBRACK)
            {
                get_nowtoken();
                biaodashi();
                if(nowtoken.type!=RBRACK)
                {
                    error();
                }
                get_nowtoken();
                if(nowtoken.type==LBRACK)
                {
                    get_nowtoken();
                    biaodashi();
                    if(nowtoken.type!=RBRACK)
                    {
                        error();
                    }
                    get_nowtoken();
                }
            }
        }
    }
    else if(nowtoken.type==LPARENT)
    {
        get_nowtoken();
        biaodashi();
        if(nowtoken.type!=RPARENT)
        {
            error();
        }
        get_nowtoken();
    }
    else if(nowtoken.type==PLUS || nowtoken.type==MINU || nowtoken.type==INTCON)
    {
        zhengshu();
    }
    else if(nowtoken.type==CHARCON)
    {
        zifu();
    }
    else
    {
        error();
    }

    grammer g(YINZI);
    print_grammer(g);
}

// <语句> ::= <循环语句>|<条件语句>|<有返回值函数调用语句>;|<无返回值函数调用语句>;|<赋值语句>;|<读语句>;|<写语句>;|<情况语句>|<空>;|<返回语句>;|'{'<语句列>'}'
/*
一共有如下若干种可能，并且它们的首符号集分别为：
<循环语句>  {WHILETK,FORTK}
<条件语句>  {IFTK}
<有返回值函数调用语句>;   {IDENFR}
<无返回值函数调用语句>;   {IDENFR}
<赋值语句>;     {IDENFR}
<读语句>;      {SCANFTK}
<写语句>;      {PRINTFTK}
<情况语句>      {SWITCHTK}
<空>;            {SEMICN}
<返回语句>;     {RETURNTK}
'{'<语句列>'}'     {LBRACE}

可以看出，只有三个是有冲突的，它们三个分别是：
<有返回值函数调用语句>;
<无返回值函数调用语句>;
<赋值语句>;
赋值语句满足tokens[cur].type==ASSIGN || tokens[cur].type==LBRACK，可以先判断这个;
然后根据当前的IDENFR，在table里面查找，根据标识符类别来选择进入有返回值还是无返回值函数调用语句；
其它的就根据首字符判断即可
*/
void Parser::yuju()
{
    if(nowtoken.type==WHILETK || nowtoken.type==FORTK)
    {
        xunhuanyuju();
    }
    else if(nowtoken.type==IFTK)
    {
        tiaojianyuju();
    }
    else if(nowtoken.type==IDENFR)
    {
        if(cur<tokens_num && (tokens[cur].type==ASSIGN || tokens[cur].type==LBRACK))
        {
            fuzhiyuju();
            if(nowtoken.type!=SEMICN)
            {
                error();
            }
            get_nowtoken();
        }
        else
        {
            if(get_identifier_type(nowtoken.word)==WUFANHUIZHIHANSHU)
            {
                wufanhuizhihanshudiaoyongyuju();
            }
            else if(get_identifier_type(nowtoken.word)==YOUFANHUIZHIHANSHU)
            {
                youfanhuizhihanshudiaoyongyuju();
            }
            if(nowtoken.type!=SEMICN)
            {
                error();
            }
            get_nowtoken();
        }
    }
    else if(nowtoken.type==SCANFTK)
    {
        duyuju();
        if(nowtoken.type!=SEMICN)
        {
            error();
        }
        get_nowtoken();
    }
    else if(nowtoken.type==PRINTFTK)
    {
        xieyuju();
        if(nowtoken.type!=SEMICN)
        {
            error();
        }
        get_nowtoken();
    }
    else if(nowtoken.type==SWITCHTK)
    {
        qingkuangyuju();
    }
    else if(nowtoken.type==SEMICN)
    {
        get_nowtoken();
    }
    else if(nowtoken.type==RETURNTK)
    {
        fanhuiyuju();
        if(nowtoken.type!=SEMICN)
        {
            error();
        }
        get_nowtoken();
    }
    else if(nowtoken.type==LBRACE)
    {
        get_nowtoken();
        yujulie();
        if(nowtoken.type!=RBRACE)
        {
            error();
        }
        get_nowtoken();
    }
    else
    {
        error();
    }

    grammer g(YUJU);
    print_grammer(g);
}

// <赋值语句> ::= <标识符>＝<表达式>|<标识符>'['<表达式>']'=<表达式>|<标识符>'['<表达式>']''['<表达式>']'=<表达式>
// 这个就根据标识符后面有没有LBRACK来判断选择哪个就行了
void Parser::fuzhiyuju()
{
    biaoshifu();
    if(nowtoken.type==LBRACK)
    {
        get_nowtoken();
        biaodashi();
        if(nowtoken.type!=RBRACK)
        {
            error();
        }
        get_nowtoken();
        if(nowtoken.type==LBRACK)
        {
            get_nowtoken();
            biaodashi();
            if(nowtoken.type!=RBRACK)
            {
                error();
            }
            get_nowtoken();
        }
    }
    if(nowtoken.type!=ASSIGN)
    {
        error();
    }
    get_nowtoken();
    biaodashi();

    grammer g(FUZHIYUJU);
    print_grammer(g);
}

// <条件语句> ::= if '('<条件>')'<语句>［else<语句>］
void Parser::tiaojianyuju()
{
    if(nowtoken.type!=IFTK)
    {
        error();
    }
    get_nowtoken();
    if(nowtoken.type!=LPARENT)
    {
        error();
    }
    get_nowtoken();
    tiaojian();
    if(nowtoken.type!=RPARENT)
    {
        error();
    }
    get_nowtoken();
    yuju();
    if(nowtoken.type==ELSETK)
    {
        get_nowtoken();
        yuju();
    }

    grammer g(TIAOJIANYUJU);
    print_grammer(g);
}

// <条件> ::= <表达式><关系运算符><表达式>
void Parser::tiaojian()
{
    biaodashi();
    guanxiyunsuanfu();
    biaodashi();

    grammer g(TIAOJIAN);
    print_grammer(g);
}

// <循环语句> ::= while'('<条件>')'<语句>| for'('<标识符>＝<表达式>;<条件>;<标识符>＝<标识符>(+|-)<步长>')'<语句>
void Parser::xunhuanyuju()
{
    if(nowtoken.type==WHILETK)
    {
        get_nowtoken();
        if(nowtoken.type!=LPARENT)
        {
            error();
        }
        get_nowtoken();
        tiaojian();
        if(nowtoken.type!=RPARENT)
        {
            error();
        }
        get_nowtoken();
        yuju();
    }
    else if(nowtoken.type==FORTK)
    {
        // for'('<标识符>＝<表达式>;<条件>;<标识符>＝<标识符>(+|-)<步长>')'<语句>
        get_nowtoken();
        if(nowtoken.type!=LPARENT)
        {
            error();
        }
        get_nowtoken();
        biaoshifu();
        if(nowtoken.type!=ASSIGN)
        {
            error();
        }
        get_nowtoken();
        biaodashi();
        if(nowtoken.type!=SEMICN)
        {
            error();
        }
        get_nowtoken();
        tiaojian();
        if(nowtoken.type!=SEMICN)
        {
            error();
        }
        get_nowtoken();
        biaoshifu();
        if(nowtoken.type!=ASSIGN)
        {
            error();
        }
        get_nowtoken();
        biaoshifu();
        if(nowtoken.type!=PLUS && nowtoken.type!=MINU)
        {
            error();
        }
        get_nowtoken();
        buchang();
        if(nowtoken.type!=RPARENT)
        {
            error();
        }
        get_nowtoken();
        yuju();
    }
    else
    {
        error();
    }

    grammer g(XUNHUANYUJU);
    print_grammer(g);
}

// <步长> ::= <无符号整数>
void Parser::buchang()
{
    wufuhaozhengshu();

    grammer g(BUCHANG);
    print_grammer(g);
}

// <情况语句> ::= switch '('<表达式>')' '{'<情况表><缺省>'}'
void Parser::qingkuangyuju()
{
    if(nowtoken.type!=SWITCHTK)
    {
        error();
    }
    get_nowtoken();
    if(nowtoken.type!=LPARENT)
    {
        error();
    }
    get_nowtoken();
    biaodashi();
    if(nowtoken.type!=RPARENT)
    {
        error();
    }
    get_nowtoken();
    if(nowtoken.type!=LBRACE)
    {
        error();
    }
    get_nowtoken();
    qingkuangbiao();
    quesheng();
    if(nowtoken.type!=RBRACE)
    {
        error();
    }
    get_nowtoken();

    grammer g(QINGKUANGYUJU);
    print_grammer(g);
}

// <情况表> ::= <情况子语句>{<情况子语句>}
void Parser::qingkuangbiao()
{
    qingkuangziyuju();
    while(nowtoken.type==CASETK)
    {
        qingkuangziyuju();
    }

    grammer g(QINGKUANGBIAO);
    print_grammer(g);
}

// <情况子语句> ::= case<常量>:<语句>
void Parser::qingkuangziyuju()
{
    if(nowtoken.type!=CASETK)
    {
        error();
    }
    get_nowtoken();
    changliang();
    if(nowtoken.type!=COLON)
    {
        error();
    }
    get_nowtoken();
    yuju();

    grammer g(QINGKUANGZIYUJU);
    print_grammer(g);
}

// <缺省> ::= default:<语句>
void Parser::quesheng()
{
    if(nowtoken.type!=DEFAULTTK)
    {
        error();
    }
    get_nowtoken();
    if(nowtoken.type!=COLON)
    {
        error();
    }
    get_nowtoken();
    yuju();

    grammer g(QUESHENG);
    print_grammer(g);
}

// <有返回值函数调用语句> ::= <标识符>'('<值参数表>')'
void Parser::youfanhuizhihanshudiaoyongyuju()
{
    biaoshifu();
    if(nowtoken.type!=LPARENT)
    {
        error();
    }
    get_nowtoken();
    zhicanshubiao();
    if(nowtoken.type!=RPARENT)
    {
        error();
    }
    get_nowtoken();

    grammer g(YOUFANHUIZHIHANSHUDIAOYONGYUJU);
    print_grammer(g);
}

// <无返回值函数调用语句> ::= <标识符>'('<值参数表>')'
void Parser::wufanhuizhihanshudiaoyongyuju()
{
    biaoshifu();
    if(nowtoken.type!=LPARENT)
    {
        error();
    }
    get_nowtoken();
    zhicanshubiao();
    if(nowtoken.type!=RPARENT)
    {
        error();
    }
    get_nowtoken();

    grammer g(WUFANHUIZHIHANSHUDIAOYONGYUJU);
    print_grammer(g);
}

// <值参数表> ::= <表达式>{,<表达式>}|<空>
// 值参数表只在函数调用语句里面出现，所以可以根据当前token的type是否为RPARENT来判断是否为空
void Parser::zhicanshubiao()
{
    if(nowtoken.type!=RPARENT)
    {
        biaodashi();
        while(nowtoken.type==COMMA)
        {
            get_nowtoken();
            biaodashi();
        }
    }

    grammer g(ZHICANSHUBIAO);
    print_grammer(g);
}

// <语句列> ::= {<语句>}
/*
<语句>的首符号集为{WHILETK,FORTK,IDENFR,SCANFTK,PRINTFTK,SWITCHTK,SEMICN,RETURNTK,LBRACE,IFTK}
所以当出现上述的时候循环进入<语句>
*/
void Parser::yujulie()
{
    while(nowtoken.type==WHILETK || nowtoken.type==FORTK || nowtoken.type==IDENFR ||
          nowtoken.type==SCANFTK || nowtoken.type==PRINTFTK || nowtoken.type==SWITCHTK ||
          nowtoken.type==SEMICN || nowtoken.type==RETURNTK || nowtoken.type==LBRACE ||
          nowtoken.type==IFTK)
    {
        yuju();
    }

    grammer g(YUJULIE);
    print_grammer(g);
}

// <读语句> ::= scanf '('<标识符>')'
void Parser::duyuju()
{
    if(nowtoken.type!=SCANFTK)
    {
        error();
    }
    get_nowtoken();
    if(nowtoken.type!=LPARENT)
    {
        error();
    }
    get_nowtoken();
    biaoshifu();
    if(nowtoken.type!=RPARENT)
    {
        error();
    }
    get_nowtoken();

    grammer g(DUYUJU);
    print_grammer(g);
}

// <写语句> ::= printf'('<字符串>,<表达式>')'| printf'('<字符串>')'| printf'('<表达式>')'
// <写语句> ::= printf'(' (<字符串>,<表达式>| <字符串>|<表达式>) ')'
void Parser::xieyuju()
{
    if(nowtoken.type!=SCANFTK)
    {
        error();
    }
    get_nowtoken();
    if(nowtoken.type!=LPARENT)
    {
        error();
    }
    get_nowtoken();

    if(nowtoken.type==STRCON)
    {
        zifuchuan();
        if(nowtoken.type==COMMA)
        {
            get_nowtoken();
            biaodashi();
        }
    }
    else
    {
        biaodashi();
    }

    if(nowtoken.type!=RPARENT)
    {
        error();
    }
    get_nowtoken();

    grammer g(XIEYUJU);
    print_grammer(g);
}

// <返回语句> ::= return['('<表达式>')']
void Parser::fanhuiyuju()
{
    if(nowtoken.type!=RETURNTK)
    {
        error();
    }
    get_nowtoken();

    if(nowtoken.type==LPARENT)
    {
        get_nowtoken();
        biaodashi();
        if(nowtoken.type!=RPARENT)
        {
            error();
        }
        get_nowtoken();
    }

    grammer g(FANHUIYUJU);
    print_grammer(g);
}







