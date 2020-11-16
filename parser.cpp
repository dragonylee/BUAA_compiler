#include "parser.h"
#include "mid.h"
#include "head.h"

int label_cnt=0;
int uselessi;
std::string useless;

// 预先加载所有string
std::map<std::string,std::string> strmap;

void reverse(std::string& r1,std::string& r2,OP& op)
{
    if(op==OP::BEQ_) op=OP::BNE_;
    else if(op==OP::BNE_) op=OP::BEQ_;
    else if(op==OP::BLE_) op=OP::BLT_,swap(r1,r2);
    else if(op==OP::BLT_) op=OP::BLE_,swap(r1,r2);
}

std::string next_label()
{
    label_cnt++;
    std::string s=LABEL_HEAD;
    return s+std::to_string(label_cnt);
}

std::string first_r()
{
    std::string r=R_HEAD;
    r+="1";
    return r;
}

std::string next_r(std::string r)
{
    r.replace(0,sizeof(R_HEAD)/sizeof(char)-1,"");
    r=std::to_string(string_to_int(r)+1);
    std::string s=R_HEAD;
    return s+r;
}

std::string Parser::base_addr()
{
    if(nowWord=="global") return "$zero";
    else return R_SP;
}

Parser::Parser()
{
    tokens=Lexer().get_all_tokens();
    tokens_num=tokens.size();
    cur=0;
    nowWord="global";
    get_nowtoken();

    // 预处理所有string
    extern int global_offset;
    add_instruct(OP::SETSTR_,".data");
    int num=0;
    for(token t:tokens) if(t.type==STRCON && !strmap.count(t.word))
    {
        num++;
        strmap[t.word]="str"+std::to_string(num);
        add_instruct(OP::SETSTR_,"str"+std::to_string(num)+": .asciiz \""+t.word+"\"");
        global_offset+=t.word.length()*2+2;
    }
    // 添加一个换行专用
    add_instruct(OP::SETSTR_,"newline: .asciiz \"\\n\"");
    global_offset+=4;
    if(global_offset%4!=0) global_offset=global_offset/4*4+4;

    add_instruct(OP::SETSTR_,".text");
}

int Parser::get_nowtoken()
{
    lasttoken=nowtoken;
    if(cur>0 && cur<=tokens_num) print_token(tokens[cur-1]);
    if(cur>=tokens_num)
    {
        nowtoken.type=0;
        return -1;
    }
    nowtoken=tokens[cur++];
    return 1;
}

void Parser::run_until(int token_type)
{
    while(cur<tokens_num && nowtoken.type!=token_type)
    {
        get_nowtoken();
    }
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

/*
关于错误处理：
（1）如果出现缺失，比如缺失分号、右括号，则默认是缺失而不是写错，也就是遇到这种错误不往后读一个token
（2）遇到变量已经定义，报错之后用新的覆盖原来的
（3）有些错误输出是当前token的行数，有些是上一个token的行数
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
void Parser::zifu(int &num)
{
    if(nowtoken.type!=CHARCON)
    {
        error(nowtoken.line,error_a);
        get_nowtoken();
    }
    else
    {
        if(!is_valid_char(nowtoken.word[0]))
        {
            error(nowtoken.line,error_a);
        }
        num=(int)nowtoken.word[0];
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
        error(nowtoken.line,error_a);
        get_nowtoken();
    }
    else
    {
        if(nowtoken.word.length()==0)
        {
            // 字符串长度不可以为0
            error(nowtoken.line,error_a);
        }
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
    // 完成变量说明后要跳转到主函数
    add_instruct(OP::JUMP_,"main");
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
            add_instruct(OP::RETURN_);  // 以防万一（有些void函数可以不写return）
        }
    }
    zhuhanshu();    // 主函数
    add_instruct(OP::EXIT_);

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
            error(lasttoken.line,error_k);
        }
        else get_nowtoken();
    }
    grammer g(CHANGLIANGSHUOMING);
    print_grammer(g);
}

// <常量定义> ::= int<标识符>＝<整数>{,<标识符>＝<整数>}|char<标识符>＝<字符>{,<标识符>＝<字符>}
// 两个选择的首符号集不相交，且都是只出现一次
// 有符号表添加操作
void Parser::changliangdingyi()
{
    if(nowtoken.type==INTTK)
    {
        int num;
        get_nowtoken();
        if(nowtoken.type!=IDENFR)
        {
            error();
        }
        if(!add_to_symbolTable(nowWord,nowtoken.word,objType::CONST,varType::INT,0))
        {
            error(nowtoken.line,error_b);
        }
        std::string word=nowtoken.word;
        get_nowtoken();
        if(nowtoken.type!=ASSIGN)
        {
            error();
        }
        get_nowtoken();
        zhengshu(num);
        add_instruct(OP::ASSIGN_,word,std::to_string(num));
        while(nowtoken.type==COMMA)
        {
            get_nowtoken();
            if(nowtoken.type!=IDENFR)
            {
                error();
            }
            if(!add_to_symbolTable(nowWord,nowtoken.word,objType::CONST,varType::INT,0))
            {
                error(nowtoken.line,error_b);
            }
            std::string word=nowtoken.word;
            get_nowtoken();
            if(nowtoken.type!=ASSIGN)
            {
                error();
            }
            get_nowtoken();
            zhengshu(num);
            add_instruct(OP::ASSIGN_,word,std::to_string(num));
        }
    }
    else if(nowtoken.type==CHARTK)
    {
        int num,offset;
        get_nowtoken();
        if(nowtoken.type!=IDENFR)
        {
            error();
        }
        if(!add_to_symbolTable(nowWord,nowtoken.word,objType::CONST,varType::CHAR,0))
        {
            error(nowtoken.line,error_b);
        }
        std::string word=nowtoken.word;
        get_nowtoken();
        if(nowtoken.type!=ASSIGN)
        {
            error();
        }
        get_nowtoken();
        zifu(num);
        add_instruct(OP::ASSIGN_,word,std::to_string(num));
        while(nowtoken.type==COMMA)
        {
            get_nowtoken();
            if(nowtoken.type!=IDENFR)
            {
                error();
            }
            if(!add_to_symbolTable(nowWord,nowtoken.word,objType::CONST,varType::CHAR,0))
            {
                error(nowtoken.line,error_b);
            }
            std::string word=nowtoken.word;
            get_nowtoken();
            if(nowtoken.type!=ASSIGN)
            {
                error();
            }
            get_nowtoken();
            zifu(num);
            add_instruct(OP::ASSIGN_,word,std::to_string(num));
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
void Parser::wufuhaozhengshu(int &num)
{
    if(nowtoken.type!=INTCON)
    {
        error();
        get_nowtoken();
    }
    else
    {
        num=string_to_int(nowtoken.word);
        get_nowtoken();
        grammer g(WUFUHAOZHENGSHU);
        print_grammer(g);
    }
}

// <整数> ::= [+|-]<无符号整数>
void Parser::zhengshu(int &num)
{
    int flag=1;
    if(nowtoken.type==MINU) flag=-1;
    if(nowtoken.type==PLUS || nowtoken.type==MINU)
    {
        jiafayunsuanfu();
    }
    wufuhaozhengshu(num);
    num=num*flag;

    grammer g(ZHENGSHU);
    print_grammer(g);
}

// <声明头部> ::= int<标识符>|char<标识符>
// 声明头部只能从有返回值函数定义推出
void Parser::shengmingtoubu()
{
    if(nowtoken.type==INTTK || nowtoken.type==CHARTK)
    {
        varType vartype=nowtoken.type==INTTK?varType::INT:varType::CHAR;
        get_nowtoken();
        if(nowtoken.type!=IDENFR)
        {
            error();
        }
        if(!add_to_symbolTable(nowWord,nowtoken.word,objType::FUNCTION,vartype,0))
        {
            error(nowtoken.line,error_b);
        }
        else
        {
            // 修改nowWord为当前函数名字
            nowWord=nowtoken.word;
            add_instruct(OP::SETLABEL_,nowtoken.word);
            get_nowtoken();
        }
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
int Parser::changliang(int &num)
{
    int ret=CHARTK;
    if(nowtoken.type==INTCON || nowtoken.type==PLUS || nowtoken.type==MINU)
    {
        ret=INTTK;
        zhengshu(num);
    }
    else if(nowtoken.type==CHARCON)
    {
        zifu(num);
    }
    else
    {
        error();
    }

    grammer g(CHANGLIANG);
    print_grammer(g);
    return ret;
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
            error(lasttoken.line,error_k);
        }
        else get_nowtoken();
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
            error(lasttoken.line,error_k);
        }
        else get_nowtoken();
    }
    grammer g(BIANLIANGSHUOMING);
    print_grammer(g);
}

// <变量定义> ::= <变量定义无初始化>|<变量定义及初始化>
/*
处理过程：
如果后面有等号，那么进入bianliangdingyijichushihua（<变量定义及初始化>）
否则进入bianliangdingyiwuchushihua（<变量定义无初始化>）
*/
void Parser::bianliangdingyi()
{
    int flag=0;
    int i=cur+1;
    while(i<tokens_num)
    {
        if(tokens[i].type==ASSIGN) {flag=1;break;}
        if(tokens[i].type==INTCON || tokens[i].type==LBRACK || tokens[i].type==RBRACK)
            i++;
        else break;
    }
    if(flag)
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
    int tmp=0;
    varType vartype;
    if(nowtoken.type==INTTK) vartype=varType::INT;
    else vartype=varType::CHAR;
    leixingbiaoshifu();

    while(!tmp || nowtoken.type==COMMA)
    {
        tmp++;
        std::string word="_";

        if(nowtoken.type==COMMA) get_nowtoken();

        if(nowtoken.type!=IDENFR)
        {
            error();
        }
        else word=nowtoken.word;
        get_nowtoken();

        int lnum=0,num;
        std::vector<int> dd;
        while(nowtoken.type==LBRACK)
        {
            lnum++;
            get_nowtoken();
            if(nowtoken.type!=INTCON)
            {
                // 数组元素下标只能是整型
                error(nowtoken.line,error_i);
            }
            wufuhaozhengshu(num);
            dd.push_back(num);
            if(nowtoken.type!=RBRACK)
            {
                // 没有右中括号当做缺失处理，也就是不往后读一个token
                error(nowtoken.line,error_m);
            }
            else get_nowtoken();
        }
        if(!add_to_symbolTable(nowWord,word,lnum>0?objType::ARRAY:objType::VARIABLE,vartype,lnum,dd))
        {
            error(nowtoken.line,error_b);
        }
    }

    grammer g(BIANLIANGDINGYIWUCHUSHIHUA);
    print_grammer(g);
}

// <变量定义及初始化> ::= <类型标识符><标识符>=<常量>|<类型标识符><标识符>'['<无符号整数>']'='{'<常量>{,<常量>}'}'|<类型标识符><标识符>'['<无符号整数>']''['<无符号整数>']'='{''{'<常量>{,<常量>}'}'{,'{'<常量>{,<常量>}'}'}'}'
void Parser::bianliangdingyijichushihua()
{
    varType vartype;
    std::string word="_";
    int type=nowtoken.type;

    if(nowtoken.type==INTTK) vartype=varType::INT;
    else vartype=varType::CHAR;
    leixingbiaoshifu();
    if(nowtoken.type!=IDENFR)
    {
        error();
    }
    else word=nowtoken.word;
    get_nowtoken();

    std::vector<int> dd;

    if(nowtoken.type==ASSIGN)   // 变量初始化
    {
        int num;
        get_nowtoken();
        if(changliang(num)!=type)
        {
            error(nowtoken.line,error_o);
        }
        if(!add_to_symbolTable(nowWord,word,objType::VARIABLE,vartype,0))
        {
            error(nowtoken.line,error_b);
        }
        add_instruct(OP::ASSIGN_,word,std::to_string(num));
    }
    else if(nowtoken.type==LBRACK)
    {
        get_nowtoken();
        int x1=0;
        if(nowtoken.type==INTCON)
        {
            x1=string_to_int(nowtoken.word);
        }
        else
        {
            // 数组元素下标只能是整型
            error(nowtoken.line,error_i);
        }
        wufuhaozhengshu(x1);
        dd.push_back(x1);
        if(nowtoken.type!=RBRACK)
        {
            error(nowtoken.line,error_m);
        }
        else get_nowtoken();
        if(nowtoken.type==ASSIGN)   // 一维数组初始化
        {
            // '{' <常量> {,<常量>} '}'
            int num;
            std::vector<int> tmp;
            get_nowtoken();
            if(nowtoken.type!=LBRACE)
            {
                error();
            }
            get_nowtoken();
            int tot1=1;
            if(changliang(num)!=type)
            {
                error(nowtoken.line,error_o);
            }
            tmp.push_back(num);
            while(nowtoken.type==COMMA)
            {
                get_nowtoken();
                if(changliang(num)!=type)
                {
                    error(nowtoken.line,error_o);
                }
                tmp.push_back(num);
                tot1++;
            }
            if(tot1!=x1)
            {
                // 初始化数组元素个数不匹配
                error(nowtoken.line,error_n);
            }
            if(nowtoken.type!=RBRACE)
            {
                error();
            }
            else get_nowtoken();
            if(!add_to_symbolTable(nowWord,word,objType::ARRAY,vartype,1,dd))
            {
                error(nowtoken.line,error_b);
            }
            for(int i=0;i<tmp.size();i++)
            {
                add_instruct(OP::SARRAY_,std::to_string(tmp[i]),word,std::to_string(i));
            }
        }
        else if(nowtoken.type==LBRACK)
        {
            get_nowtoken();
            int x2=0;
            if(nowtoken.type==INTCON)
            {
                x2=string_to_int(nowtoken.word);
            }
            else
            {
                // 数组元素下标只能是整型
                error(nowtoken.line,error_i);
            }
            wufuhaozhengshu(x2);
            dd.push_back(x2);
            if(nowtoken.type!=RBRACK)
            {
                error(nowtoken.line,error_m);
            }
            else get_nowtoken();
            if(nowtoken.type==ASSIGN)   // 二维数组初始化
            {
                // '{' '{' <常量>{,<常量>} '}' {,'{'<常量>{,<常量>} '}' }  '}'
                get_nowtoken();
                int tot1=0,tot2=0;
                int num;
                std::vector<int> tmp;
                if(nowtoken.type!=LBRACE)
                {
                    error();
                }
                get_nowtoken();
                if(nowtoken.type!=LBRACE)
                {
                    error();
                }
                else tot1++;
                get_nowtoken();
                if(changliang(num)!=type)
                {
                    error(nowtoken.line,error_o);
                }
                tot2++;
                tmp.push_back(num);
                while(nowtoken.type==COMMA)
                {
                    get_nowtoken();
                    if(changliang(num)!=type)
                    {
                        error(nowtoken.line,error_o);
                    }
                    tmp.push_back(num);
                    tot2++;
                }
                if(tot2!=x2)
                {
                    error(nowtoken.line,error_n);
                }
                if(nowtoken.type!=RBRACE)
                {
                    error();
                    //run_until(RBRACE);
                }
                else get_nowtoken();
                while(nowtoken.type==COMMA)
                {
                    // { ,'{'<常量>{,<常量>} '}' }
                    get_nowtoken();
                    if(nowtoken.type!=LBRACE)
                    {
                        error();
                    }
                    else tot1++;
                    get_nowtoken();
                    if(changliang(num)!=type)
                    {
                        error(nowtoken.line,error_o);
                    }
                    tmp.push_back(num);
                    tot2=1;
                    while(nowtoken.type==COMMA)
                    {
                        get_nowtoken();
                        if(changliang(num)!=type)
                        {
                            error(nowtoken.line,error_o);
                        }
                        tmp.push_back(num);
                        tot2++;
                    }
                    if(tot2!=x2)
                    {
                        error(nowtoken.line,error_n);
                    }
                    if(nowtoken.type!=RBRACE)
                    {
                        error();
                        //run_until(RBRACE);
                    }
                    else get_nowtoken();
                }
                if(tot1!=x1)
                {
                    error(nowtoken.line,error_n);
                }
                if(nowtoken.type!=RBRACE)
                {
                    error();
                }
                get_nowtoken();
                if(!add_to_symbolTable(nowWord,word,objType::ARRAY,vartype,2,dd))
                {
                    error(nowtoken.line,error_b);
                }
                for(int i=0;i<tmp.size();i++)
                {
                    add_instruct(OP::SARRAY_,std::to_string(tmp[i]),word,std::to_string(i));
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
    if(!(nowtoken.type==INTTK || nowtoken.type==CHARTK))
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
    shengmingtoubu();
    if(nowtoken.type!=LPARENT)
    {
        error();
    }
    get_nowtoken();
    int num=canshubiao();
    modify_globalSymbolTable(nowWord,num);  // 修改当前函数参数个数
    if(nowtoken.type!=RPARENT)
    {
        error(nowtoken.line,error_l);
    }
    else get_nowtoken();
    if(nowtoken.type!=LBRACE)
    {
        error();
    }

    modify_isret(nowWord,0);

    get_nowtoken();
    fuheyuju();
    if(nowtoken.type!=RBRACE)
    {
        error();
        //run_until(RBRACE);
    }

    symbolEntry se;
    get_symbolEntry(nowWord,nowWord,se);
    if(se.is_ret==0)
    {
        // 有返回值函数，没有返回东西
        error(nowtoken.line,error_h);
    }

    get_nowtoken();

    // 有返回值函数定义完之后，应当将当前范围变成全局
    nowWord="global";

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

    if(nowtoken.type!=IDENFR) error();
    if(!add_to_symbolTable(nowWord,nowtoken.word,objType::FUNCTION,varType::VOID,0))
    {
        error(nowtoken.line,error_b);
    }
    // 将nowWord置为当前函数
    nowWord=nowtoken.word;
    add_instruct(OP::SETLABEL_,nowtoken.word);
    get_nowtoken();

    if(nowtoken.type!=LPARENT)
    {
        error();
    }
    else get_nowtoken();
    int num=canshubiao();
    modify_globalSymbolTable(nowWord,num);
    if(nowtoken.type!=RPARENT)
    {
        error(nowtoken.line,error_l);
    }
    else get_nowtoken();

    modify_isret(nowWord,0);

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

    symbolEntry se;
    get_symbolEntry(nowWord,nowWord,se);
    if(se.is_ret==1)
    {
        // 无返回值函数，有返回东西
        error(lasttoken.line,error_g);
    }

    get_nowtoken();

    // 无返回值函数定义完之后，应当将当前范围变成全局
    nowWord="global";

    grammer g(WUFANHUIZHIHANSHUDINGYI);
    print_grammer(g);
}

// <复合语句> ::=[<常量说明>][<变量说明>]<语句列>
// <常量说明>的首符号集是{CONSTTK}
// <变量说明>的首符号集是{INTTK,CHARTK}
// <语句列>的首符号集有好多好多元素，但是和前两个都没有交集，所以直接处理
// 如果有return，返回1
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
返回参数个数
*/
int Parser::canshubiao()
{
    symbolEntry se;
    int tot=0;
    if(nowtoken.type==INTTK || nowtoken.type==CHARTK)
    {
        varType vartype=nowtoken.type==INTTK?varType::INT:varType::CHAR;
        leixingbiaoshifu();
        if(nowtoken.type!=IDENFR) error();
        else
        {
            if(!add_to_symbolTable(nowWord,nowtoken.word,objType::VARIABLE,vartype,0))
            {
                error(nowtoken.line,error_b);
            }
            modify_add_parameter(nowWord,vartype==varType::INT?INTTK:CHARTK);
            modify_add_parameter_name(nowWord,nowtoken.word);
            get_nowtoken();
            tot++;
        }
        while(nowtoken.type==COMMA)
        {
            get_nowtoken();
            varType vartype=nowtoken.type==INTTK?varType::INT:varType::CHAR;
            leixingbiaoshifu();
            if(nowtoken.type!=IDENFR) error();
            else
            {
                if(!add_to_symbolTable(nowWord,nowtoken.word,objType::VARIABLE,vartype,0))
                {
                    error(nowtoken.line,error_b);
                }
                modify_add_parameter(nowWord,vartype==varType::INT?INTTK:CHARTK);
                modify_add_parameter_name(nowWord,nowtoken.word);
                get_nowtoken();
                tot++;
            }
        }
    }

    grammer g(CANSHUBIAO);
    print_grammer(g);
    return tot;
}

// <主函数> ::= void main'('')' '{'<复合语句>'}'
void Parser::zhuhanshu()
{
    if(nowtoken.type!=VOIDTK)
    {
        error();
    }
    else get_nowtoken();
    if(nowtoken.type!=MAINTK)
    {
        error();
    }
    else
    {
        add_instruct(OP::SETLABEL_,nowtoken.word);
        if(!add_to_symbolTable(nowWord,nowtoken.word,objType::FUNCTION,varType::VOID,0))
        {
            error(nowtoken.line,error_b);
        }
        // 修改为主函数
        nowWord=nowtoken.word;
        get_nowtoken();
    }
    if(nowtoken.type!=LPARENT)
    {
        error();
    }
    else get_nowtoken();
    if(nowtoken.type!=RPARENT)
    {
        error(nowtoken.line,error_l);
    }
    else get_nowtoken();
    if(nowtoken.type!=LBRACE)
    {
        error();
    }
    else get_nowtoken();
    fuheyuju();

    symbolEntry se;
    get_symbolEntry(nowWord,nowWord,se);
    if(se.is_ret==1)
    {
        error(lasttoken.line,error_g);
    }

    if(nowtoken.type!=RBRACE)
    {
        error();
    }
    else get_nowtoken();

    // 离开主函数之后，修改为全局
    nowWord="global";

    grammer g(ZHUHANSHU);
    print_grammer(g);
}

// <表达式> ::= [+|-]<项>{<加法运算符><项>}
/*
表达式为CHARTK当且仅当以下三种情况：
（1）只有一个标识符变量，或者一个char数组；
（2）char类型函数调用；
（3）CHARCON；
其他情况都是INTTK
*/
// 返回表达式类别, CHARTK / INTTK
int Parser::biaodashi(std::string r_up)
{
    int ret=CHARTK,flag=0;
    std::string t1=next_r(r_up);
    if(nowtoken.type==MINU) flag=1;
    if(nowtoken.type==PLUS || nowtoken.type==MINU)
    {
        jiafayunsuanfu();
        ret=INTTK;
    }
    if(xiang(t1)==INTTK) ret=INTTK;
    if(flag)
    {
        add_instruct(OP::SUB_,r_up,R_ZERO,t1);
    }
    else add_instruct(OP::ASSIGN_,r_up,t1);
    while(nowtoken.type==PLUS || nowtoken.type==MINU)
    {
        flag=nowtoken.type==PLUS?0:1;
        jiafayunsuanfu();
        xiang(t1);
        if(flag) add_instruct(OP::SUB_,r_up,r_up,t1);
        else add_instruct(OP::ADD_,r_up,r_up,t1);
        ret=INTTK;
    }

    grammer g(BIAODASHI);
    print_grammer(g);
    return ret;
}

// <项> ::= <因子>{<乘法运算符><因子>}
int Parser::xiang(std::string r_up)
{
    int ret=CHARTK;
    std::string t1=next_r(r_up);
    if(yinzi(t1)==INTTK) ret=INTTK;
    add_instruct(OP::ASSIGN_,r_up,t1);
    while(nowtoken.type==MULT || nowtoken.type==DIV)
    {
        int flag=nowtoken.type==MULT?0:1;
        chengfayunsuanfu();
        yinzi(t1);
        if(flag) add_instruct(OP::DIV_,r_up,r_up,t1);
        else add_instruct(OP::MULT_,r_up,r_up,t1);
        ret=INTTK;
    }

    grammer g(XIANG);
    print_grammer(g);
    return ret;
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
int Parser::yinzi(std::string r_up)
{
    int ret=CHARTK;
    if(nowtoken.type==IDENFR)
    {
        if(cur<tokens_num && tokens[cur].type==LPARENT)
        {
            symbolEntry se;
            if(get_symbolEntry("",nowtoken.word,se))
            {
                if(se.vartype==varType::INT) ret=INTTK;
            }
            youfanhuizhihanshudiaoyongyuju(r_up);
        }
        else
        {
            symbolEntry se;
            if(get_symbolEntry(nowWord,nowtoken.word,se))
            {
                if(se.vartype==varType::INT) ret=INTTK;
            }
            std::string word=nowtoken.word;
            biaoshifu();
            if(nowtoken.type==LBRACK)
            {
                get_nowtoken();
                std::string t1=next_r(r_up);
                if(biaodashi(t1)!=INTTK)
                {
                    error(nowtoken.line,error_i);
                }
                if(nowtoken.type!=RBRACK)
                {
                    error(nowtoken.line,error_m);
                }
                else get_nowtoken();
                if(nowtoken.type==LBRACK)
                {
                    get_nowtoken();
                    std::string t2=next_r(t1);
                    if(biaodashi(t2)!=INTTK)
                    {
                        error(nowtoken.line,error_i);
                    }
                    if(nowtoken.type!=RBRACK)
                    {
                        error(nowtoken.line,error_m);
                    }
                    else get_nowtoken();
                    symbolEntry se;
                    get_symbolEntry(nowWord,word,se);
                    // r_up = word[t1][t2]
                    add_instruct(OP::MULT_,t1,t1,std::to_string(se.dd[1]));
                    add_instruct(OP::ADD_,t1,t1,t2);
                    add_instruct(OP::LARRAY_,r_up,word,t1);
                }
                else
                {
                    // 一维数组 r_up=word[t1]
                    add_instruct(OP::LARRAY_,r_up,word,t1);
                }
            }
            else
            {
                add_instruct(OP::ASSIGN_,r_up,word);
            }
        }
    }
    else if(nowtoken.type==LPARENT)
    {
        ret=INTTK;
        get_nowtoken();
        biaodashi(r_up);
        if(nowtoken.type!=RPARENT)
        {
            // 默认缺失
            error(nowtoken.line,error_l);
        }
        else get_nowtoken();
    }
    else if(nowtoken.type==PLUS || nowtoken.type==MINU || nowtoken.type==INTCON)
    {
        ret=INTTK;
        int num;
        zhengshu(num);
        add_instruct(OP::ASSIGN_,r_up,std::to_string(num));
    }
    else if(nowtoken.type==CHARCON)
    {
        int num;
        zifu(num);
        add_instruct(OP::ASSIGN_,r_up,std::to_string(num));
    }
    else
    {
        error();
    }

    grammer g(YINZI);
    print_grammer(g);
    return ret;
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
                error(lasttoken.line,error_k);
            }
            else get_nowtoken();
        }
        else
        {
            symbolEntry se;
            get_symbolEntry(nowWord,nowtoken.word,se);
            if(se.vartype==varType::VOID)
            {
                wufanhuizhihanshudiaoyongyuju();
            }
            else
            {
                std::string r=first_r();
                youfanhuizhihanshudiaoyongyuju(r);
            }

            if(nowtoken.type!=SEMICN)
            {
                error(lasttoken.line,error_k);
            }
            else get_nowtoken();
        }
    }
    else if(nowtoken.type==SCANFTK)
    {
        duyuju();
        if(nowtoken.type!=SEMICN)
        {
            error(lasttoken.line,error_k);
        }
        else get_nowtoken();
    }
    else if(nowtoken.type==PRINTFTK)
    {
        xieyuju();
        if(nowtoken.type!=SEMICN)
        {
            error(lasttoken.line,error_k);
        }
        else get_nowtoken();
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
            error(lasttoken.line,error_k);
        }
        else get_nowtoken();
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
    std::string tmp=nowtoken.word;
    biaoshifu();
    symbolEntry se;
    if(get_symbolEntry(nowWord,tmp,se))
    {
        if(se.objtype==objType::CONST)
        {
            // 不能改变常量的值
            error(nowtoken.line,error_j);
        }
    }
    int type=0;
    std::string t1=first_r();
    std::string t2=next_r(t1),t3=next_r(t2);
    if(nowtoken.type==LBRACK)
    {
        type=1;
        get_nowtoken();
        if(biaodashi(t1)!=INTTK)
        {
            error(nowtoken.line,error_i);
        }
        if(nowtoken.type!=RBRACK)
        {
            error(nowtoken.line,error_m);
        }
        else get_nowtoken();
        if(nowtoken.type==LBRACK)
        {
            get_nowtoken();
            if(biaodashi(t2)!=INTTK)
            {
                error(nowtoken.line,error_i);
            }
            if(nowtoken.type!=RBRACK)
            {
                error(nowtoken.line,error_m);
            }
            else get_nowtoken();
            // tmp[t2][t3]
            get_symbolEntry(nowWord,tmp,se);
            add_instruct(OP::MULT_,t1,t1,std::to_string(se.dd[1]));
            add_instruct(OP::ADD_,t1,t1,t2);
            // now become tmp[t2]
        }
    }
    if(nowtoken.type!=ASSIGN)
    {
        error();
    }
    get_nowtoken();
    biaodashi(t3);
    if(!type)
    {
        add_instruct(OP::ASSIGN_,tmp,t3);
    }
    else
    {
        add_instruct(OP::SARRAY_,t3,tmp,t1);
    }

    grammer g(FUZHIYUJU);
    print_grammer(g);
}

// <条件语句> ::= if '('<条件>')'<语句>［else<语句>］
// branch if r1 op r2
void Parser::tiaojianyuju()
{
    std::string r1=first_r(),r2=next_r(r1);
    OP op;
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
    tiaojian(r1,r2,op);
    if(nowtoken.type!=RPARENT)
    {
        error(nowtoken.line,error_l);
    }
    else get_nowtoken();

    std::string lb1=next_label(),lb2=next_label(),lb3=next_label();
    add_instruct(op,r1,r2,lb1);
    add_instruct(OP::JUMP_,lb2);
    add_instruct(OP::SETLABEL_,lb1);
    yuju();
    add_instruct(OP::JUMP_,lb3);
    add_instruct(OP::SETLABEL_,lb2);

    if(nowtoken.type==ELSETK)
    {
        get_nowtoken();
        yuju();
    }

    add_instruct(OP::SETLABEL_,lb3);

    grammer g(TIAOJIANYUJU);
    print_grammer(g);
}

// <条件> ::= <表达式><关系运算符><表达式>
// 只能是整型之间比
void Parser::tiaojian(std::string& r1,std::string& r2,OP& op)
{
    int x=biaodashi(r1);
    int type=nowtoken.type;
    guanxiyunsuanfu();
    int y=biaodashi(r2);
    if(x==CHARTK || y==CHARTK)
    {
        // 条件左右只能是整型
        error(lasttoken.line,error_f);
    }
    if(type==LSS) op=OP::BLT_;
    else if(type==LEQ) op=OP::BLE_;
    else if(type==GRE) op=OP::BLT_,swap(r1,r2);
    else if(type==GEQ) op=OP::BLE_,swap(r1,r2);
    else if(type==EQL) op=OP::BEQ_;
    else if(type==NEQ) op=OP::BNE_;

    grammer g(TIAOJIAN);
    print_grammer(g);
}

// <循环语句> ::= while'('<条件>')'<语句>| for'('<标识符>＝<表达式>;<条件>;<标识符>＝<标识符>(+|-)<步长>')'<语句>
void Parser::xunhuanyuju()
{
    std::string r1=first_r(),r2=next_r(r1);
    OP op;
    if(nowtoken.type==WHILETK)
    {
        get_nowtoken();
        if(nowtoken.type!=LPARENT)
        {
            error();
        }
        get_nowtoken();

        std::string lb1=next_label(),lb2=next_label();
        add_instruct(OP::SETLABEL_,lb1);
        tiaojian(r1,r2,op);
        reverse(r1,r2,op);
        add_instruct(op,r1,r2,lb2);

        if(nowtoken.type!=RPARENT)
        {
            error(nowtoken.line,error_l);
        }
        else get_nowtoken();
        yuju();

        add_instruct(OP::JUMP_,lb1);
        add_instruct(OP::SETLABEL_,lb2);
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
        std::string tmp=nowtoken.word;
        symbolEntry se;
        if(get_symbolEntry(nowWord,tmp,se))
        {
            if(se.objtype==objType::CONST)
            {
                // 不能改变常量的值
                error(nowtoken.line,error_j);
            }
        }
        biaoshifu();
        if(nowtoken.type!=ASSIGN)
        {
            error();
        }
        get_nowtoken();
        biaodashi(r1);
        if(nowtoken.type!=SEMICN)
        {
            error(lasttoken.line,error_k);
        }
        else get_nowtoken();

        add_instruct(OP::ASSIGN_,tmp,r1);
        std::string lb1=next_label(),lb2=next_label();
        add_instruct(OP::SETLABEL_,lb1);
        tiaojian(r1,r2,op);
        reverse(r1,r2,op);
        add_instruct(op,r1,r2,lb2);

        if(nowtoken.type!=SEMICN)
        {
            error(lasttoken.line,error_k);
        }
        else get_nowtoken();
        biaoshifu();
        if(nowtoken.type!=ASSIGN)
        {
            error();
        }
        get_nowtoken();
        std::string tmp2=nowtoken.word;
        biaoshifu();
        int flag=0,num;
        if(nowtoken.type==PLUS) flag=1;
        if(nowtoken.type!=PLUS && nowtoken.type!=MINU)
        {
            error();
        }
        get_nowtoken();
        buchang(num);
        if(nowtoken.type!=RPARENT)
        {
            error(nowtoken.line,error_l);
        }
        else get_nowtoken();
        yuju();

        if(flag) add_instruct(OP::ADD_,tmp2,tmp2,std::to_string(num));
        else add_instruct(OP::SUB_,tmp2,tmp2,std::to_string(num));
        add_instruct(OP::JUMP_,lb1);
        add_instruct(OP::SETLABEL_,lb2);
    }
    else
    {
        error();
    }

    grammer g(XUNHUANYUJU);
    print_grammer(g);
}

// <步长> ::= <无符号整数>
void Parser::buchang(int& num)
{
    wufuhaozhengshu(num);

    grammer g(BUCHANG);
    print_grammer(g);
}

// <情况语句> ::= switch '('<表达式>')' '{'<情况表><缺省>'}'
void Parser::qingkuangyuju()
{
    std::string r1=first_r();
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
    int x=biaodashi(r1);
    if(nowtoken.type!=RPARENT)
    {
        error(nowtoken.line,error_l);
    }
    else get_nowtoken();
    if(nowtoken.type!=LBRACE)
    {
        error();
    }
    get_nowtoken();

    std::string lb1=next_label();
    qingkuangbiao(x,lb1,r1);
    quesheng();
    add_instruct(OP::SETLABEL_,lb1);

    if(nowtoken.type!=RBRACE)
    {
        error();
    }
    get_nowtoken();

    grammer g(QINGKUANGYUJU);
    print_grammer(g);
}

// <情况表> ::= <情况子语句>{<情况子语句>}
void Parser::qingkuangbiao(int type,std::string lb1,std::string r1)
{
    qingkuangziyuju(type,lb1,r1);
    while(nowtoken.type==CASETK)
    {
        qingkuangziyuju(type,lb1,r1);
    }

    grammer g(QINGKUANGBIAO);
    print_grammer(g);
}

// <情况子语句> ::= case<常量>:<语句>
void Parser::qingkuangziyuju(int type,std::string lb1,std::string r1)
{
    if(nowtoken.type!=CASETK)
    {
        error();
    }
    get_nowtoken();
    int num;
    if(changliang(num)!=type)
    {
        error(nowtoken.line,error_o);
    }
    if(nowtoken.type!=COLON)
    {
        error();
    }
    get_nowtoken();

    std::string lb2=next_label();
    add_instruct(OP::BNE_,r1,std::to_string(num),lb2);
    yuju();
    add_instruct(OP::JUMP_,lb1);
    add_instruct(OP::SETLABEL_,lb2);

    grammer g(QINGKUANGZIYUJU);
    print_grammer(g);
}

// <缺省> ::= default:<语句>
// 这里啥也不用干，因为前面如果有满足的就会直接跳过这里
void Parser::quesheng()
{
    if(nowtoken.type!=DEFAULTTK)
    {
        error(nowtoken.line,error_p);
        // 直接返回，当做default缺失
        return;
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
void Parser::youfanhuizhihanshudiaoyongyuju(std::string r_up)
{
    std::string tmp=nowtoken.word;
    biaoshifu();
    if(nowtoken.type!=LPARENT)
    {
        error();
    }
    get_nowtoken();
    std::vector<std::string> para_value;
    std::vector<std::string> para_name=get_parameters_name(tmp);
    int num=zhicanshubiao(get_parameters(tmp),para_value,r_up);
    symbolEntry se;
    if(!get_symbolEntry(nowWord,tmp,se))
    {
        // 不存在该函数
        //error(nowtoken.line,error_c);
    }
    else if(se.dimension!=num)
    {
        // 函数参数个数不匹配
        error(nowtoken.line,error_d);
    }
    if(nowtoken.type!=RPARENT)
    {
        error(nowtoken.line,error_l);
    }
    else get_nowtoken();

    // 先把自己用过的push
    add_instruct(OP::PUSH_);
    // 然后把R_SP减去分配空间
    add_instruct(OP::SUB_,R_SP,R_SP,std::to_string(get_total_offset(tmp)));
    // 然后发送参数
    for(int i=0;i<para_value.size();i++)
    {
        add_instruct(OP::SENDP_,para_value[i],
                     std::to_string(get_offset(tmp,para_name[i])),
                     R_SP);
    }
    // 然后调用函数
    add_instruct(OP::CALL_,tmp);
    // 然后把R_SP加上分配空间
    add_instruct(OP::ADD_,R_SP,R_SP,std::to_string(get_total_offset(tmp)));
    // 然后把用过的pop
    add_instruct(OP::POP_);

    // 最后把 R_RET 处的答案给 r_up
    add_instruct(OP::ASSIGN_,r_up,R_RET);


    grammer g(YOUFANHUIZHIHANSHUDIAOYONGYUJU);
    print_grammer(g);
}

// <无返回值函数调用语句> ::= <标识符>'('<值参数表>')'
void Parser::wufanhuizhihanshudiaoyongyuju()
{
    std::string tmp=nowtoken.word;
    biaoshifu();
    if(nowtoken.type!=LPARENT)
    {
        error();
    }
    get_nowtoken();
    std::vector<std::string> para_value;
    std::vector<std::string> para_name=get_parameters_name(tmp);
    int num=zhicanshubiao(get_parameters(tmp),para_value,first_r());
    symbolEntry se;
    if(!get_symbolEntry(nowWord,tmp,se))
    {
        // 不存在该函数
        //error(nowtoken.line,error_c);
    }
    else if(se.dimension!=num)
    {
        // 函数参数个数不匹配
        error(nowtoken.line,error_d);
    }
    if(nowtoken.type!=RPARENT)
    {
        error(nowtoken.line,error_l);
    }
    else get_nowtoken();

    // 先把自己用过的push
    add_instruct(OP::PUSH_);
    // 然后把R_SP减去分配空间
    add_instruct(OP::SUB_,R_SP,R_SP,std::to_string(get_total_offset(tmp)));
    // 然后发送参数
    for(int i=0;i<para_value.size();i++)
    {
        add_instruct(OP::SENDP_,para_value[i],
                     std::to_string(get_offset(tmp,para_name[i])),
                     R_SP);
    }
    // 然后调用函数
    add_instruct(OP::CALL_,tmp);
    // 然后把R_SP加上分配空间
    add_instruct(OP::ADD_,R_SP,R_SP,std::to_string(get_total_offset(tmp)));
    // 然后把用过的pop
    add_instruct(OP::POP_);
    // 最后不用return

    grammer g(WUFANHUIZHIHANSHUDIAOYONGYUJU);
    print_grammer(g);
}

// <值参数表> ::= <表达式>{,<表达式>}|<空>
/*
表达式的首符号集：
PLUS
MINU
IDENFR
LPARENT
INTCON
CHARCON
*/
// 值参数表只在函数调用语句里面出现，所以可以根据当前token的type是否为RPARENT来判断是否为空
int Parser::zhicanshubiao(std::vector<int> parameters,
                          std::vector<std::string>& para_value,
                          std::string r1)
{
    std::string r;
    int tot=0;
    int i=0;
    int num;
    if(nowtoken.type==PLUS || nowtoken.type==MINU || nowtoken.type==IDENFR ||
       nowtoken.type==LPARENT || nowtoken.type==INTCON || nowtoken.type==CHARCON)
    {
        r=next_r(r1);
        int x=biaodashi(r);
        if(i<(int)parameters.size() && parameters[i]!=x)
        {
            // 函数参数类型不匹配
            error(nowtoken.line,error_e);
        }
        para_value.push_back(r);
        i++;
        tot++;
        while(nowtoken.type==COMMA)
        {
            get_nowtoken();
            r=next_r(r);
            x=biaodashi(r);
            if(i<(int)parameters.size() && parameters[i]!=x)
            {
                // 函数参数类型不匹配
                error(nowtoken.line,error_e);
            }
            para_value.push_back(r);
            i++;
            tot++;
        }
    }

    grammer g(ZHICANSHUBIAO);
    print_grammer(g);
    return tot;
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
    symbolEntry se;
    std::string word=nowtoken.word;
    varType vt;
    if(get_symbolEntry(nowWord,nowtoken.word,se))
    {
        if(se.objtype==objType::CONST)
        {
            // 常量不能读入
            error(nowtoken.line,error_j);
        }
        vt=se.vartype;
    }
    biaoshifu();
    if(nowtoken.type!=RPARENT)
    {
        error(nowtoken.line,error_l);
    }
    else get_nowtoken();

    // read
    add_instruct(OP::READ_,word,vt==varType::INT?"INT":"CHAR");

    grammer g(DUYUJU);
    print_grammer(g);
}

// <写语句> ::= printf'('<字符串>,<表达式>')'| printf'('<字符串>')'| printf'('<表达式>')'
// <写语句> ::= printf'(' (<字符串>,<表达式>| <字符串>|<表达式>) ')'
void Parser::xieyuju()
{
    if(nowtoken.type!=PRINTFTK)
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
        add_instruct(OP::PRINT_,nowtoken.word,"STRING",strmap[nowtoken.word]);
        zifuchuan();
        if(nowtoken.type==COMMA)
        {
            get_nowtoken();
            std::string r=first_r();
            if(biaodashi(r)==INTTK)
            {
                add_instruct(OP::PRINT_,r,"INT");
            }
            else add_instruct(OP::PRINT_,r,"CHAR");
        }
    }
    else
    {
        std::string r=first_r();
        if(biaodashi(r)==INTTK)
        {
            add_instruct(OP::PRINT_,r,"INT");
        }
        else add_instruct(OP::PRINT_,r,"CHAR");
    }

    if(nowtoken.type!=RPARENT)
    {
        error(nowtoken.line,error_l);
    }
    else get_nowtoken();

    add_instruct(OP::PRINT_,"\\n","STRING","newline");

    grammer g(XIEYUJU);
    print_grammer(g);
}

// <返回语句> ::= return['('<表达式>')']
void Parser::fanhuiyuju()
{
    symbolEntry se;
    get_symbolEntry(nowWord,nowWord,se);
    varType vt=se.vartype;

    if(nowtoken.type!=RETURNTK)
    {
        error();
    }
    get_nowtoken();

    if(nowtoken.type==LPARENT)
    {
        if(vt==varType::VOID)
        {
            // 无返回值函数中有返回东西
            modify_isret(nowWord,1);
        }
        get_nowtoken();
        if(nowtoken.type==RPARENT && vt!=varType::VOID)
        {
            // 有返回值函数中，没有返回东西
            modify_isret(nowWord,0);
        }
        else if(nowtoken.type!=RPARENT && vt!=varType::VOID)
        {
            // 有返回值函数，有返回东西
            modify_isret(nowWord,1);
        }

        std::string r=first_r();
        int x=biaodashi(r);
        add_instruct(OP::ASSIGN_,R_RET,r);

        if((vt==varType::CHAR && x==INTTK) || (vt==varType::INT && x==CHARTK))
        {
            error(lasttoken.line,error_h);
        }
        if(nowtoken.type!=RPARENT)
        {
            error(nowtoken.line,error_l);
        }
        else get_nowtoken();
    }
    else
    {
        if(vt!=varType::VOID)
        {
            // 有返回值函数中，没有返回东西
            modify_isret(nowWord,0);
        }
    }

    if(nowWord=="main") add_instruct(OP::EXIT_);
    else add_instruct(OP::RETURN_);

    grammer g(FANHUIYUJU);
    print_grammer(g);
}


// <标识符> ::= IDENFR
void Parser::biaoshifu()
{
    symbolEntry s;
    if(nowtoken.type!=IDENFR)
    {
        error();
        get_nowtoken();
    }
    else if(!get_symbolEntry(nowWord,nowtoken.word,s))
    {
        error(nowtoken.line,error_c);
        get_nowtoken();
    }
    else
    {
        get_nowtoken();
        grammer g(BIAOSHIFU);
        //print_grammer(g);
    }
}


