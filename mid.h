#ifndef __MID_H__
#define __MID_H__

#include "head.h"

#define R_HEAD "$t_"
#define R_SP "$sp"
#define R_ZERO "$zero"
#define R_RET "$ret"
#define LABEL_HEAD "_label_"

enum OP
{
    ADD_,
    SUB_,
    MULT_,
    DIV_,
    LARRAY_,
    SARRAY_,
    ASSIGN_,
    BEQ_,
    BNE_,
    BLE_,
    BLT_,
    JUMP_,
    SETLABEL_,
    RETURN_,
    CALL_,
    PUSH_,
    POP_,
    SENDP_,
    READ_,
    PRINT_,
    PARA_,
    SETSTR_,
    EXIT_,
};

std::string op_to_string(OP op);

struct instruct
{
    OP op;
    std::string t1,t2,t3;
    instruct(OP op,std::string t1="",std::string t2="",std::string t3=""):
        op(op),t1(t1),t2(t2),t3(t3){};
};

void add_instruct(OP op,std::string t1="",std::string t2="",std::string t3="");

void print_instruct(instruct ins);
void print_all_instruct();

std::vector<instruct> get_all_instructs();

#endif // __MID_H__
