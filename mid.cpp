#include "mid.h"

std::vector<instruct> inss;

std::vector<instruct> get_all_instructs()
{
    return inss;
}

std::string op_to_string(OP op)
{
    std::string s;
    switch(op)
    {
        case OP::ADD_: s="ADD"; break;
        case OP::SUB_: s="SUB"; break;
        case OP::MULT_: s="MULT"; break;
        case OP::DIV_: s="DIV"; break;
        case OP::LARRAY_: s="LARRAY"; break;
        case OP::SARRAY_: s="SARRAY"; break;
        case OP::ASSIGN_: s="ASSIGN"; break;
        case OP::BEQ_: s="BEQ"; break;
        case OP::BNE_: s="BNE"; break;
        case OP::BLE_: s="BLE"; break;
        case OP::BLT_: s="BLT"; break;
        case OP::JUMP_: s="JUMP"; break;
        case OP::SETLABEL_: s="SETLABEL"; break;
        case OP::RETURN_: s="RETURN"; break;
        case OP::CALL_: s="CALL"; break;
        case OP::SENDP_: s="SENDP"; break;
        case OP::PUSH_: s="PUSH"; break;
        case OP::POP_: s="POP"; break;
        case OP::READ_: s="READ"; break;
        case OP::PRINT_: s="PRINT"; break;
        case OP::PARA_: s="PARA"; break;
        case OP::SETSTR_: s="SETSTR"; break;
        case OP::EXIT_: s="EXIT"; break;
        default: s="NONETYPE";
    }
    return s;
}


void add_instruct(OP op,std::string t1,std::string t2,std::string t3)
{
    inss.push_back(instruct(op,t1,t2,t3));
}

void print_instruct(instruct ins)
{
    std::cout<<op_to_string(ins.op)<<"  ";
    if(ins.t1!="") std::cout<<ins.t1;
    if(ins.t2!="") std::cout<<" , "<<ins.t2;
    if(ins.t3!="") std::cout<<" , "<<ins.t3;
}

void print_all_instruct()
{
    for(instruct ins:inss)
    {
        if(ins.op==OP::SETLABEL_)
        {
            std::cout<<ins.t1<<":\t";
        }
        else std::cout<<"\t";
        print_instruct(ins);
        std::cout<<"\n";
    }
}
