#ifndef __MIPS_H__
#define __MIPS_H__

#include "mid.h"
#include "head.h"
#include "symbol.h"

class mips
{
private:
    std::vector<std::string> outs;
    std::vector<instruct> inss;

    void add(std::string s);
    std::string change(std::string r);
public:
    mips();
    void deal_instruct(instruct ins);
    void load(std::string t,int offset);
    void store(std::string t,int offset);
    void store(std::string t,std::string word);
    void load(std::string t,std::string word);

    void deal();
    void print_all();
};


#endif // __MIPS_H__
