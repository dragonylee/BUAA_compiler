#include "head.h"

token::token(int code,std::string word,int line)
{
    this->code=code;
    this->word=word;
    this->line=line;
}

std::string get_format_code(int code)
{
    switch(code)
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
    std::string t;
    t.resize(s.size());
    transform(s.begin(),s.end(),t.begin(),::tolower);
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
    std::cout<<get_format_code(t.code)<<' ';
    std::cout<<t.word<<std::endl;
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
