#include "symbol.h"

// 全局符号表
symbolTable globalSymbolTable;
// 局部符号表，其中每个函数（main除外）对应一个局部符号表
std::map<std::string,symbolTable> localSymbolTable;



// 添加全局变量，
// 返回0表示已经存在同名符号
int add_to_globalSymbolTable(std::string word,
                             objType objtype,
                             varType vartype,
                             int dimension)
{
    std::string t=str_tolower(word);
    if(globalSymbolTable.count(t))
    {
        globalSymbolTable[t]=symbolEntry(word,objtype,vartype,dimension,0);
        if(objtype==objType::FUNCTION && !localSymbolTable.count(t))
        {
            symbolTable s;
            localSymbolTable.insert(make_pair(t,s));
        }
        return 0;
    }
    globalSymbolTable.insert(make_pair(t,symbolEntry(word,objtype,vartype,dimension,0)));
    // 如果是函数，同时往局部符号表添加一项
    if(objtype==objType::FUNCTION)
    {
        symbolTable s;
        localSymbolTable.insert(make_pair(t,s));
    }
    return 1;
}


// 根据名字从全局符号表返回symbolEntry
// 返回0表示全局符号表不存在这个名字的符号
int get_symbolEntry_from_globalSymbolTable(std::string word,
                                           symbolEntry& symbolentry)
{
    std::string t=str_tolower(word);
    if(!globalSymbolTable.count(t))
    {
        return 0;
    }
    symbolentry=globalSymbolTable[t];
    return 1;
}


// 往名字为funcWord的局部符号表里添加符号
// 这里funcWord一定存在
// 如果局部符号表里已经有同名符号，报错
int add_to_localSymbolTable(std::string funcWord,
                            std::string word,
                            objType objtype,
                            varType vartype,
                            int dimension)
{
    std::string s=str_tolower(funcWord);
    std::string t=str_tolower(word);
    if(localSymbolTable[s].count(t))
    {
        localSymbolTable[s][t]=symbolEntry(word,objtype,vartype,dimension,0);
        return 0;
    }
    localSymbolTable[s].insert(make_pair(t,symbolEntry(word,objtype,vartype,dimension,0)));
    return 1;
}


// 从名字为funcWord的局部符号表里查询名字为word的符号的symbolEntry
// 返回0表示不存在这个符号
int get_symbolEntry_from_localSymbolTable(std::string funcWord,
                                          std::string word,
                                          symbolEntry& symbolentry)
{
    std::string s=str_tolower(funcWord);
    std::string t=str_tolower(word);
    if(!localSymbolTable.count(s))
    {
        return 0;
    }
    if(!localSymbolTable[s].count(t))
    {
        // 如果局部符号表没有，还要查询全局符号表
        if(!globalSymbolTable.count(t))
        {
            return 0;
        }
        symbolentry=globalSymbolTable[t];
        return 1;
    }
    symbolentry=localSymbolTable[s][t];
    return 1;
}


// 添加进符号表
// 如果funcWord是"global"，添加进全局符号表，否则添加进局部符号表
// 这里采用覆盖式处理，也就是如果有已经定义的，就覆盖掉，同时返回0
int add_to_symbolTable(std::string funcWord,
                       std::string word,
                       objType objtype,
                       varType vartype,
                       int dimension)
{
    if(funcWord=="global")
    {
        return add_to_globalSymbolTable(word,objtype,vartype,dimension);
    }
    else
    {
        return add_to_localSymbolTable(funcWord,word,objtype,vartype,dimension);
    }
}

// 从名字为funcWord的局部符号表里查询名字为word的符号的symbolEntry
// 如果funcWord==""，表示从全局符号表查询
// 返回0表示不存在
int get_symbolEntry(std::string funcWord,
                    std::string word,
                    symbolEntry& symbolentry)
{
    if(funcWord=="")
    {
        return get_symbolEntry_from_globalSymbolTable(word,symbolentry);
    }
    else
    {
        return get_symbolEntry_from_localSymbolTable(funcWord,word,symbolentry);
    }
}


// 对全局符号表的修改操作，只针对FUNCTION类型的符号，用于修改dimension，表示有多少个参数
void modify_globalSymbolTable(std::string funcWord,int number)
{
    if(globalSymbolTable.count(str_tolower(funcWord)))
        globalSymbolTable[str_tolower(funcWord)].dimension=number;
}

void modify_isret(std::string funcWord,int is_ret)
{
    if(globalSymbolTable.count(str_tolower(funcWord)))
        globalSymbolTable[str_tolower(funcWord)].is_ret=is_ret;
}

void modify_add_parameter(std::string funcWord,int type)
{
    if(globalSymbolTable.count(str_tolower(funcWord)))
    {
        globalSymbolTable[str_tolower(funcWord)].parameters.push_back(type);
    }
}


std::vector<int> get_parameters(std::string funcWord)
{
    symbolEntry se;
    if(!get_symbolEntry("",funcWord,se))
    {
        std::vector<int> ret;
        return ret;
    }
    else
    {
        return se.parameters;
    }
}

// debug
void print_symbolEntry(symbolEntry s)
{
    std::string t=s.word+'\t';
    if(s.objtype==objType::ARRAY) t+="ARRAY";
    else if(s.objtype==objType::CONST) t+="CONST";
    else if(s.objtype==objType::FUNCTION) t+="FUNCTION";
    else if(s.objtype==objType::VARIABLE) t+="VARIABLE";
    else t+="unknown objtype";

    t+='\t';
    if(s.vartype==varType::CHAR) t+="CHAR";
    else if(s.vartype==varType::INT) t+="INT";
    else if(s.vartype==varType::STRING) t+="STRING";
    else if(s.vartype==varType::VOID) t+="VOID";
    else t+="unknown vartype";

    t+='\t';
    t+=std::to_string(s.dimension);
    t+='\t';
    for(int i:s.parameters)
    {
        t+=token_type_to_string(i)+" ";
    }

    std::cout<<t<<std::endl;
}

void print_globalSymbolTable()
{
    for(std::pair<std::string,symbolEntry> s:globalSymbolTable)
    {
        print_symbolEntry(s.second);
    }
}

void print_localSymbolTable(std::string funcWord)
{
    for(std::pair<std::string,symbolEntry> s:localSymbolTable[str_tolower(funcWord)])
    {
        print_symbolEntry(s.second);
    }
}

void print_all_localSymbolTable()
{
    for(std::pair<std::string,symbolEntry> s:globalSymbolTable)
    {
        if(s.second.objtype==objType::FUNCTION)
        {
            std::cout<<s.first<<std::endl;
            print_localSymbolTable(s.first);
            std::cout<<std::endl;
        }
    }
}

