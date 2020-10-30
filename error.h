#ifndef __ERROR_H__
#define __ERROR_H__

#include "head.h"

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

void error();
void error(int line,char error_type);

#endif // __ERROR_H__
