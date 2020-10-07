#ifndef __HEAD_H__
#define __HEAD_H__

#include <iostream>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <set>

#define PW std::cout<<"________something_need_to_show__________\n";

// token的类别码定义
#define IDENFR 1
#define INTCON 2
#define CHARCON 3
#define STRCON 4
#define CONSTTK 5
#define INTTK 6
#define CHARTK 7
#define VOIDTK 8
#define MAINTK 9
#define IFTK 10
#define ELSETK 11
#define SWITCHTK 12
#define CASETK 13
#define DEFAULTTK 14
#define WHILETK 15
#define FORTK 16
#define SCANFTK 17
#define PRINTFTK 18
#define RETURNTK 19
#define PLUS 20
#define MINU 21
#define MULT 22
#define DIV 23
#define LSS 24
#define LEQ 25
#define GRE 26
#define GEQ 27
#define EQL 28
#define NEQ 29
#define COLON 30
#define ASSIGN 31
#define SEMICN 32
#define COMMA 33
#define LPARENT 34
#define RPARENT 35
#define LBRACK 36
#define RBRACK 37
#define LBRACE 38
#define RBRACE 39

// 终结符
struct token
{
    int type;
    std::string word;
    int line;
    token(int type=0,std::string word="",int line=0);
};

// 根据类别码（整型），返回该类别的字符串
std::string token_type_to_string(int type);

int is_reserverd_word(std::string s);
int string_to_int(std::string s);

void print_token(token t);

bool is_letter(char c);
bool is_digit(char c);
bool is_space(char c);
bool is_newline(char c);
bool is_plus(char c);
bool is_minu(char c);
bool is_mult(char c);
bool is_div(char c);
bool is_lss(char c);
bool is_gre(char c);
bool is_eql(char c);
bool is_neq(char c);
bool is_colon(char c);
bool is_semicn(char c);
bool is_comma(char c);
bool is_lparent(char c);
bool is_rparent(char c);
bool is_lbrack(char c);
bool is_rbrack(char c);
bool is_lbrace(char c);
bool is_rbrace(char c);
bool is_valid_str(char c);
bool is_valid_char(char c);


// grammer的类别码定义
#define JIAFAYUNSUANFU 1        // 加法运算符
#define CHENGFAYUNSUANFU 2      // 乘法运算符
#define GUANXIYUNSUANFU 3       // 关系运算符
#define ZIMU 4                  // 字母
#define SHUZI 5                 // 数字
// 以下的都有语法子程序进行处理
#define ZIFU 6                  // 字符
#define ZIFUCHUAN 7             // 字符串
#define CHENGXU 8               // 程序
#define CHANGLIANGSHUOMING 9    // 常量说明
#define CHANGLIANGDINGYI 10     // 常量定义
#define WUFUHAOZHENGSHU 11      // 无符号整数
#define ZHENGSHU 12             // 整数
#define BIAOSHIFU 13            // 标识符
#define SHENGMINGTOUBU 14       // 声明头部
#define CHANGLIANG 15           // 常量
#define BIANLIANGSHUOMING 16    // 变量说明
#define BIANLIANGDINGYI 17      // 变量定义
#define BIANLIANGDINGYIWUCHUSHIHUA 18   // 变量定义无初始化
#define BIANLIANGDINGYIJICHUSHIHUA 19   // 变量定义及初始化
#define LEIXINGBIAOSHIFU 20     // 类型标识符
#define YOUFANHUIZHIHANSHUDINGYI 21     // 有返回值函数定义
#define WUFANHUIZHIHANSHUDINGYI 22      // 无返回值函数定义
#define FUHEYUJU 23             // 复合语句
#define CANSHUBIAO 24           // 参数表
#define ZHUHANSHU 25            // 主函数
#define BIAODASHI 26            // 表达式
#define XIANG 27                // 项
#define YINZI 28                // 因子
#define YUJU 29                 // 语句
#define FUZHIYUJU 30            // 赋值语句
#define TIAOJIANYUJU 31         // 条件语句
#define TIAOJIAN 32             // 条件
#define XUNHUANYUJU 33          // 循环语句
#define BUCHANG 34              // 步长
#define QINGKUANGYUJU 35        // 情况语句
#define QINGKUANGBIAO 36        // 情况表
#define QINGKUANGZIYUJU 37      // 情况子语句
#define QUESHENG 38             // 缺省
#define YOUFANHUIZHIHANSHUDIAOYONGYUJU 39   // 有返回值函数调用语句
#define WUFANHUIZHIHANSHUDIAOYONGYUJU 40    // 无返回值函数调用语句
#define ZHICANSHUBIAO 41        // 值参数表
#define YUJULIE 42              // 语句列
#define DUYUJU 43               // 读语句
#define XIEYUJU 44              // 写语句
#define FANHUIYUJU 45           // 返回语句

// 非终结符
struct grammer
{
    int type;
    grammer(int type=0);
};

// 根据grammer类别码，返回对应语法成分的字符串
std::string grammer_type_to_string(int type);

void print_grammer(grammer g);


// 符号表的相关定义
#define ZHENGXINGBIANLIANG 1
#define ZIFUBIANLIANG 2
#define ZHENGXINGCHANGLIANG 3
#define ZIFUCHANGLIANG 4
#define YOUFANHUIZHIHANSHU 5
#define WUFANHUIZHIHANSHU 6
#define YIWEIZHENGXING 7
#define ERWEIZHENGXING 8
#define YIWEIZIFU   9
#define ERWEIZIFU   10

struct identifier
{
    int type;
    std::string word;
    identifier(int type,std::string word);
    bool operator < (const identifier x) const;
    bool operator == (const identifier x) const;
};


// for debug
void debug_ptk(token t);

#endif // __HEAD_H__
/*
#include "lexer.h"
#include "error.h"
#include "parser.h"
*/
