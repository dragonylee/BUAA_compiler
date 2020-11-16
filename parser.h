#ifndef __PARSER_H__
#define __PARSER_H__

#include "lexer.h"
#include "head.h"
#include "error.h"
#include "symbol.h"
#include "mid.h"

/*
语法分析。
约定：将各个语法成分（非终结符）定义一个解析程序，
然后每次进入解析程序之前，先预读入一个token
*/

class Parser
{
private:
    token nowtoken;
    token lasttoken;
    std::vector<token> tokens;
    int cur;    // 这里的cur永远指向tokens中的下一个token
    int tokens_num;
    // 获取下一个token，存入nowtoken中，如果没有了返回-1，并且nowtoken的type赋值为0
    int get_nowtoken();
    // 记录当前在哪个函数中，"global"表示在全局区
    std::string nowWord;

    void run_until(int token_type);
    void add_parameter();
    std::string base_addr();
public:
    // 各个子处理程序
    void jiafayunsuanfu();
    void chengfayunsuanfu();
    void guanxiyunsuanfu();
    void zifu(int& num);
    void zifuchuan();
    void chengxu();
    void changliangshuoming();
    void changliangdingyi();
    void wufuhaozhengshu(int& num);
    void zhengshu(int& num);
    void shengmingtoubu();
    int changliang(int& num);
    void bianliangshuoming();
    void bianliangdingyi();
    void bianliangdingyiwuchushihua();
    void bianliangdingyijichushihua();
    void leixingbiaoshifu();
    void youfanhuizhihanshudingyi();
    void wufanhuizhihanshudingyi();
    void fuheyuju();
    int canshubiao();
    void zhuhanshu();
    int biaodashi(std::string r_up);
    int xiang(std::string r_up);
    int yinzi(std::string r_up);
    void yuju();
    void fuzhiyuju();
    void tiaojianyuju();
    void tiaojian(std::string& r1,std::string& r2,OP& op);
    void xunhuanyuju();
    void buchang(int& num);
    void qingkuangyuju();
    void qingkuangbiao(int,std::string lb1,std::string r1);
    void qingkuangziyuju(int,std::string lb1,std::string r1);
    void quesheng();
    void youfanhuizhihanshudiaoyongyuju(std::string r_up);
    void wufanhuizhihanshudiaoyongyuju();
    int zhicanshubiao(std::vector<int>,std::vector<std::string>&,std::string);
    void yujulie();
    void duyuju();
    void xieyuju();
    void fanhuiyuju();
    void biaoshifu();

    Parser();

    // for debug
    void print_all_tokens();
};

// for debug
std::string first_r();
std::string next_r(std::string r);


#endif // __PARSER_H__
