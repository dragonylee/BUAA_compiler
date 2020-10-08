#ifndef __PARSER_H__
#define __PARSER_H__

#include "lexer.h"
#include "head.h"
#include "error.h"


/*
语法分析。
约定：将各个语法成分（非终结符）定义一个解析程序，
然后每次进入解析程序之前，先预读入一个token
*/

class Parser
{
private:
    token nowtoken;
    std::vector<token> tokens;
    std::vector<identifier> table; // 符号表
    int cur;    // 这里的cur永远指向tokens中的下一个token
    int tokens_num;
    // 获取下一个token，存入nowtoken中，如果没有了返回-1，并且nowtoken的type赋值为0
    int get_nowtoken();
    bool insert_identifier(int type,std::string word);  // 插入符号表
    // 返回某个标识符s的类别，如果不存在该标识符则返回-1
    int get_identifier_type(std::string word);
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
    bool biaoshifu();
    void shengmingtoubu();
    void changliang();
    void bianliangshuoming();
    void bianliangdingyi();
    void bianliangdingyiwuchushihua();
    void bianliangdingyijichushihua();
    void leixingbiaoshifu();
    void youfanhuizhihanshudingyi();
    void wufanhuizhihanshudingyi();
    void fuheyuju();
    void canshubiao();
    void zhuhanshu();
    void biaodashi();
    void xiang();
    void yinzi();
    void yuju();
    void fuzhiyuju();
    void tiaojianyuju();
    void tiaojian();
    void xunhuanyuju();
    void buchang();
    void qingkuangyuju();
    void qingkuangbiao();
    void qingkuangziyuju();
    void quesheng();
    void youfanhuizhihanshudiaoyongyuju();
    void wufanhuizhihanshudiaoyongyuju();
    void zhicanshubiao();
    void yujulie();
    void duyuju();
    void xieyuju();
    void fanhuiyuju();

    Parser();

    // for debug
    void print_all_tokens();
};

#endif // __PARSER_H__
