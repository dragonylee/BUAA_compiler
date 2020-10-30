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
    STRING,
    VOID,
};

struct symbolEntry
{
    std::string word;
    objType objtype;
    varType vartype;
    int dimension;  // 数组维数/函数参数个数
    int addr;
    int is_ret=0;   // 记录函数是否有返回语句
    std::vector<int> parameters; // 存储函数参数类型
    symbolEntry(std::string word,objType objtype,varType vartype, int dimension,int addr):
        word(word),objtype(objtype),vartype(vartype),dimension(dimension),addr(addr){};
    symbolEntry(){};
};

typedef std::map<std::string,symbolEntry> symbolTable;

int add_to_globalSymbolTable(std::string word,objType objtype,varType vartype,int dimension);
int get_symbolEntry_from_globalSymbolTable(std::string word, symbolEntry& symbolentry);

int add_to_localSymbolTable(std::string funcWord,std::string word,objType objtype,varType vartype,int dimension);
int get_symbolEntry_from_localSymbolTable(std::string funcWord,std::string word,symbolEntry& symbolentry);

int add_to_symbolTable(std::string funcWord,std::string word,objType objtype,varType vartype,int dimension);
int get_symbolEntry(std::string funcWord,std::string word,symbolEntry& symbolentry);

void modify_globalSymbolTable(std::string funcWord,int number);
void modify_isret(std::string funcWord,int is_ret);
void modify_add_parameter(std::string funcWord,int type);
std::vector<int> get_parameters(std::string funcWord);


// debug
void print_symbolEntry(symbolEntry s);
void print_globalSymbolTable();
void print_localSymbolTable(std::string funcWord);
void print_all_localSymbolTable();


#endif // __SYMBOL_H__
