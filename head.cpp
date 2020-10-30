#include "head.h"

std::string str_tolower(std::string s)
{
    std::string t;
    t.resize(s.size());
    transform(s.begin(),s.end(),t.begin(),::tolower);
    return t;
}

token::token(int type,std::string word,int line)
{
    this->type=type;
    this->word=word;
    this->line=line;
}

std::string token_type_to_string(int type)
{
    switch(type)
    {
    case IDENFR: return "IDENFR";
    case INTCON: return "INTCON";
    case CHARCON: return "CHARCON";
    case STRCON: return "STRCON";
    case CONSTTK: return "CONSTTK";
    case INTTK: return "INTTK";
    case CHARTK: return "CHARTK";
    case VOIDTK: return "VOIDTK";
    case MAINTK: return "MAINTK";
    case IFTK: return "IFTK";
    case ELSETK: return "ELSETK";
    case SWITCHTK: return "SWITCHTK";
    case CASETK: return "CASETK";
    case DEFAULTTK: return "DEFAULTTK";
    case WHILETK: return "WHILETK";
    case FORTK: return "FORTK";
    case SCANFTK: return "SCANFTK";
    case PRINTFTK: return "PRINTFTK";
    case RETURNTK: return "RETURNTK";
    case PLUS: return "PLUS";
    case MINU: return "MINU";
    case MULT: return "MULT";
    case DIV: return "DIV";
    case LSS: return "LSS";
    case LEQ: return "LEQ";
    case GRE: return "GRE";
    case GEQ: return "GEQ";
    case EQL: return "EQL";
    case NEQ: return "NEQ";
    case COLON: return "COLON";
    case ASSIGN: return "ASSIGN";
    case SEMICN: return "SEMICN";
    case COMMA: return "COMMA";
    case LPARENT: return "LPARENT";
    case RPARENT: return "RPARENT";
    case LBRACK: return "LBRACK";
    case RBRACK: return "RBRACK";
    case LBRACE: return "LBRACE";
    case RBRACE: return "RBRACE";
    default: return "_____WRONG_____";
    }
}

int is_reserverd_word(std::string s)
{
    std::string t=str_tolower(s);
    if(t=="const") return CONSTTK;
    else if(t=="int") return INTTK;
    else if(t=="char") return CHARTK;
    else if(t=="void") return VOIDTK;
    else if(t=="main") return MAINTK;
    else if(t=="if") return IFTK;
    else if(t=="else") return ELSETK;
    else if(t=="switch") return SWITCHTK;
    else if(t=="case") return CASETK;
    else if(t=="default") return DEFAULTTK;
    else if(t=="while") return WHILETK;
    else if(t=="for") return FORTK;
    else if(t=="scanf") return SCANFTK;
    else if(t=="printf") return PRINTFTK;
    else if(t=="return") return RETURNTK;
    return 0;
}

int string_to_int(std::string s)
{
    int ret=0;
    for(char i:s)
    {
        ret=ret*10+i-'0';
    }
    return ret;
}

void print_token(token t)
{
    #ifdef PRINT_TOKEN
    //std::cout<<t.line<<'\t';
    std::cout<<token_type_to_string(t.type)<<' ';
    std::cout<<t.word<<std::endl;
    #endif // PRINT_TOKEN
}

bool is_letter(char c) {return isalpha(c)>0 || c=='_';}
bool is_digit(char c) {return isdigit(c)>0;}
bool is_space(char c) {return c==' ' || c=='\t' || c=='\r';}
bool is_newline(char c) {return c=='\n';}
bool is_plus(char c) {return c=='+';}
bool is_minu(char c) {return c=='-';}
bool is_mult(char c) {return c=='*';}
bool is_div(char c) {return c=='/';}
bool is_lss(char c) {return c=='<';}
bool is_gre(char c) {return c=='>';}
bool is_eql(char c) {return c=='=';}
bool is_neq(char c) {return c=='!';}
bool is_colon(char c) {return c==':';}
bool is_semicn(char c) {return c==';';}
bool is_comma(char c) {return c==',';}
bool is_lparent(char c) {return c=='(';}
bool is_rparent(char c) {return c==')';}
bool is_lbrack(char c) {return c=='[';}
bool is_rbrack(char c) {return c==']';}
bool is_lbrace(char c) {return c=='{';}
bool is_rbrace(char c) {return c=='}';}
bool is_valid_str(char c) {return c==32 || c==33 || (c>=35 && c<=126);}
bool is_valid_char(char c)
{
    return is_letter(c) || is_digit(c) || is_plus(c) || is_minu(c) || is_mult(c) || is_div(c);
}


grammer::grammer(int type)
{
    this->type=type;
}

std::string grammer_type_to_string(int type)
{
    switch(type)
    {
    case JIAFAYUNSUANFU: return "加法运算符";
    case CHENGFAYUNSUANFU: return "乘法运算符";
    case GUANXIYUNSUANFU: return "关系运算符";
    case ZIMU: return "字母";
    case SHUZI: return "数字";
    case ZIFU: return "字符";
    case ZIFUCHUAN: return "字符串";
    case CHENGXU: return "程序";
    case CHANGLIANGSHUOMING: return "常量说明";
    case CHANGLIANGDINGYI: return "常量定义";
    case WUFUHAOZHENGSHU: return "无符号整数";
    case ZHENGSHU: return "整数";
    case BIAOSHIFU: return "标识符";
    case SHENGMINGTOUBU: return "声明头部";
    case CHANGLIANG: return "常量";
    case BIANLIANGSHUOMING: return "变量说明";
    case BIANLIANGDINGYI: return "变量定义";
    case BIANLIANGDINGYIWUCHUSHIHUA: return "变量定义无初始化";
    case BIANLIANGDINGYIJICHUSHIHUA: return "变量定义及初始化";
    case LEIXINGBIAOSHIFU: return "类型标识符";
    case YOUFANHUIZHIHANSHUDINGYI: return "有返回值函数定义";
    case WUFANHUIZHIHANSHUDINGYI: return "无返回值函数定义";
    case FUHEYUJU: return "复合语句";
    case CANSHUBIAO: return "参数表";
    case ZHUHANSHU: return "主函数";
    case BIAODASHI: return "表达式";
    case XIANG: return "项";
    case YINZI: return "因子";
    case YUJU: return "语句";
    case FUZHIYUJU: return "赋值语句";
    case TIAOJIANYUJU: return "条件语句";
    case TIAOJIAN: return "条件";
    case XUNHUANYUJU: return "循环语句";
    case BUCHANG: return "步长";
    case QINGKUANGYUJU: return "情况语句";
    case QINGKUANGBIAO: return "情况表";
    case QINGKUANGZIYUJU: return "情况子语句";
    case QUESHENG: return "缺省";
    case YOUFANHUIZHIHANSHUDIAOYONGYUJU: return "有返回值函数调用语句";
    case WUFANHUIZHIHANSHUDIAOYONGYUJU: return "无返回值函数调用语句";
    case ZHICANSHUBIAO: return "值参数表";
    case YUJULIE: return "语句列";
    case DUYUJU: return "读语句";
    case XIEYUJU: return "写语句";
    case FANHUIYUJU: return "返回语句";
    default: return "_____WRONG_____";
    }
}

void print_grammer(grammer g)
{
    #ifdef PRINT_GRAMMER
    std::cout<<"<"+grammer_type_to_string(g.type)+">";
    std::cout<<std::endl;
    #endif // PRINT_GRAMMER
}


identifier::identifier(int type,std::string word)
{
    this->type=type;
    this->word=word;
}

bool identifier::operator < (const identifier x) const
{
    return word<x.word;
}

bool identifier::operator == (const identifier x) const
{
    return word==x.word;
}



// for debug
void debug_ptk(token t)
{
    std::cout<<"__debug__:\t";
    std::cout<<token_type_to_string(t.type)<<' ';
    std::cout<<t.word<<std::endl;
}


