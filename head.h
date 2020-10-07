#ifndef __HEAD_H__
#define __HEAD_H__

#include <iostream>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <set>

#define PW std::cout<<"________something_need_to_show__________\n";

// token������붨��
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

// �ս��
struct token
{
    int type;
    std::string word;
    int line;
    token(int type=0,std::string word="",int line=0);
};

// ��������루���ͣ������ظ������ַ���
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


// grammer������붨��
#define JIAFAYUNSUANFU 1        // �ӷ������
#define CHENGFAYUNSUANFU 2      // �˷������
#define GUANXIYUNSUANFU 3       // ��ϵ�����
#define ZIMU 4                  // ��ĸ
#define SHUZI 5                 // ����
// ���µĶ����﷨�ӳ�����д���
#define ZIFU 6                  // �ַ�
#define ZIFUCHUAN 7             // �ַ���
#define CHENGXU 8               // ����
#define CHANGLIANGSHUOMING 9    // ����˵��
#define CHANGLIANGDINGYI 10     // ��������
#define WUFUHAOZHENGSHU 11      // �޷�������
#define ZHENGSHU 12             // ����
#define BIAOSHIFU 13            // ��ʶ��
#define SHENGMINGTOUBU 14       // ����ͷ��
#define CHANGLIANG 15           // ����
#define BIANLIANGSHUOMING 16    // ����˵��
#define BIANLIANGDINGYI 17      // ��������
#define BIANLIANGDINGYIWUCHUSHIHUA 18   // ���������޳�ʼ��
#define BIANLIANGDINGYIJICHUSHIHUA 19   // �������弰��ʼ��
#define LEIXINGBIAOSHIFU 20     // ���ͱ�ʶ��
#define YOUFANHUIZHIHANSHUDINGYI 21     // �з���ֵ��������
#define WUFANHUIZHIHANSHUDINGYI 22      // �޷���ֵ��������
#define FUHEYUJU 23             // �������
#define CANSHUBIAO 24           // ������
#define ZHUHANSHU 25            // ������
#define BIAODASHI 26            // ���ʽ
#define XIANG 27                // ��
#define YINZI 28                // ����
#define YUJU 29                 // ���
#define FUZHIYUJU 30            // ��ֵ���
#define TIAOJIANYUJU 31         // �������
#define TIAOJIAN 32             // ����
#define XUNHUANYUJU 33          // ѭ�����
#define BUCHANG 34              // ����
#define QINGKUANGYUJU 35        // ������
#define QINGKUANGBIAO 36        // �����
#define QINGKUANGZIYUJU 37      // ��������
#define QUESHENG 38             // ȱʡ
#define YOUFANHUIZHIHANSHUDIAOYONGYUJU 39   // �з���ֵ�����������
#define WUFANHUIZHIHANSHUDIAOYONGYUJU 40    // �޷���ֵ�����������
#define ZHICANSHUBIAO 41        // ֵ������
#define YUJULIE 42              // �����
#define DUYUJU 43               // �����
#define XIEYUJU 44              // д���
#define FANHUIYUJU 45           // �������

// ���ս��
struct grammer
{
    int type;
    grammer(int type=0);
};

// ����grammer����룬���ض�Ӧ�﷨�ɷֵ��ַ���
std::string grammer_type_to_string(int type);

void print_grammer(grammer g);


// ���ű����ض���
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
