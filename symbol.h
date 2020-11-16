#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include "head.h"

enum objType
{
    CONST,
    VARIABLE,
    ARRAY,
    FUNCTION,
};

enum varType
{
    INT,
    CHAR,
    STRING,     // 这个没用过
    VOID,
};

/*
按照文法规定，在编译阶段我们就已经可以知道数组的每一维的数目了，
所以给数组分配空间时，不用传递数组模板
*/

struct symbolEntry
{
    std::string word;
    objType objtype;
    varType vartype;
    int dimension;  // 数组维数/函数参数个数
    std::vector<int> dd;    // 数组每一维的维数
    int is_ret=0;   // 记录函数是否有返回语句
    std::vector<int> parameters; // 存储函数参数类型
    std::vector<std::string> parameters_name;   // 存储函数参数名字
    int offset;     // 地址偏移
    symbolEntry(std::string word,objType objtype,varType vartype, int dimension,int offset):
        word(word),objtype(objtype),vartype(vartype),dimension(dimension),offset(offset){};
    symbolEntry(){};
};

typedef std::map<std::string,symbolEntry> symbolTable;

int add_to_globalSymbolTable(std::string word,objType objtype,varType vartype,int dimension,std::vector<int> dd={});
int get_symbolEntry_from_globalSymbolTable(std::string word, symbolEntry& symbolentry);

int add_to_localSymbolTable(std::string funcWord,std::string word,objType objtype,varType vartype,int dimension,std::vector<int> dd={});
int get_symbolEntry_from_localSymbolTable(std::string funcWord,std::string word,symbolEntry& symbolentry);

int add_to_symbolTable(std::string funcWord,std::string word,objType objtype,varType vartype,int dimension,std::vector<int> dd={});
int get_symbolEntry(std::string funcWord,std::string word,symbolEntry& symbolentry);

void modify_globalSymbolTable(std::string funcWord,int number);
void modify_isret(std::string funcWord,int is_ret);
void modify_add_parameter(std::string funcWord,int type);
void modify_add_parameter_name(std::string funcWord,std::string name);
std::vector<int> get_parameters(std::string funcWord);
std::vector<std::string> get_parameters_name(std::string funcWord);

int get_offset(std::string funcWord,std::string word);
int get_total_offset(std::string funcWord);

std::string sp_or_zero(std::string funcWord,std::string word);

// debug
void print_symbolEntry(symbolEntry s);
void print_globalSymbolTable();
void print_localSymbolTable(std::string funcWord);
void print_all_localSymbolTable();


#endif // __SYMBOL_H__
