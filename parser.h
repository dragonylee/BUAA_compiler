#ifndef __PARSER_H__
#define __PARSER_H__

#include "lexer.h"
#include "head.h"
#include "error.h"
#include "symbol.h"

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
public:
    // 各个子处理程序
    void jiafayunsuanfu();
    void chengfayunsuanfu();
    void guanxiyunsuanfu();
    void zifu();
    void zifuchuan();
    void chengxu();
    void changliangshuoming();
    void changliangdingyi();
    void wufuhaozhengshu();
    void zhengshu();
    void shengmingtoubu();
    int changliang();
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
    int biaodashi();
    int xiang();
    int yinzi();
    void yuju();
    void fuzhiyuju();
    void tiaojianyuju();
    void tiaojian();
    void xunhuanyuju();
    void buchang();
    void qingkuangyuju();
    void qingkuangbiao(int);
    void qingkuangziyuju(int);
    void quesheng();
    void youfanhuizhihanshudiaoyongyuju();
    void wufanhuizhihanshudiaoyongyuju();
    int zhicanshubiao(std::vector<int>);
    void yujulie();
    void duyuju();
    void xieyuju();
    void fanhuiyuju();
    void biaoshifu();

    Parser();

    // for debug
    void print_all_tokens();
};

#endif // __PARSER_H__
