#include "head.h"
#include "error.h"

int tot_error_num=0;

void error()
{
    //
}

#define error_a 'a' // 非法符号或不符合词法
#define error_b 'b' // 名字重定义
#define error_c 'c' // 未定义的名字
#define error_d 'd' // 函数参数个数不匹配
#define error_e 'e' // 函数参数类型不匹配
#define error_f 'f' // 条件判断中出现不合法的类型
#define error_g 'g' // 无返回值的函数存在不匹配的return语句
#define error_h 'h' // 有返回值的函数缺少return语句或存在不匹配的return语句
#define error_i 'i' // 数组元素的下标只能是整型表达式
#define error_j 'j' // 不能改变常量的值
#define error_k 'k' // 行末应为分号
#define error_l 'l' // 应为右小括号’)’
#define error_m 'm' // 应为右中括号’]’
#define error_n 'n' // 数组初始化个数不匹配
#define error_o 'o' // switch <常量>类型不一致
#define error_p 'p' // switch 缺少缺省语句

void error(int line,char error_type)
{
    tot_error_num++;
    std::string s;
    switch(error_type)
    {
    case error_a: s="非法符号或不符合词法"; break;
    case error_b: s="名字重定义"; break;
    case error_c: s="未定义的名字"; break;
    case error_d: s="函数参数个数不匹配"; break;
    case error_e: s="函数参数类型不匹配"; break;
    case error_f: s="条件判断中出现不合法的类型"; break;
    case error_g: s="无返回值的函数存在不匹配的return语句"; break;
    case error_h: s="有返回值的函数缺少return语句或存在不匹配的return语句"; break;
    case error_i: s="数组元素的下标只能是整型表达式"; break;
    case error_j: s="不能改变常量的值"; break;
    case error_k: s="行末应为分号"; break;
    case error_l: s="应为右小括号’)’"; break;
    case error_m: s="应为右中括号’]’"; break;
    case error_n: s="数组初始化个数不匹配"; break;
    case error_o: s="switch <常量>类型不一致"; break;
    case error_p: s="switch 缺少缺省语句"; break;
    default:
        s="未定义错误类型";
    }
    if(tot_error_num<10000)
    {
        #ifdef PRINT_ERROR
        //std::cout<<"___error___"<<'\t';
        //std::cout<<line<<' '<<error_type<<' '<<s<<'\n';
        std::cout<<line<<' '<<error_type<<'\n';
        #endif // PRINT_ERROR
    }
}
