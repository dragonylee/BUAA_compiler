#ifndef __HEAD_H__
#define __HEAD_H__

#include <iostream>
#include <cstdio>
#include <algorithm>
#include <vector>

// word code define
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

struct token
{
    int code;
    std::string word;
    int line;
    token(int code=0,std::string word="",int line=0);
};

std::string get_format_code(int code);
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


#endif // __HEAD_H__
