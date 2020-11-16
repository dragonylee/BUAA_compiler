#include "mips.h"
#include "parser.h"

std::string nowWord="global";

extern std::map<std::string,std::string> strmap;

mips::mips()
{
    inss=get_all_instructs();
}

void mips::add(std::string s)
{
    outs.push_back(s);
}

/*
本来的寄存器是 R_HEAD+num 的格式，从1开始
由于mips的寄存器限制，我们只分配3-22号的寄存器可以使用，也就是20个
23,24和25号寄存器用于临时使用，也就是中间表达式用到了变量名的话使用
*/
std::string mips::change(std::string r)
{
    if(r==R_ZERO) return "$0";
    if(r==R_SP) return "$sp";
    if(r==R_RET) return "$v0";

    r.replace(0,sizeof(R_HEAD)/sizeof(char)-1,"");
    int x=string_to_int(r)+2;
    return "$"+std::to_string(x);
}

void mips::load(std::string t,std::string word)
{
    int offset=get_offset(nowWord,word);
    std::string r=sp_or_zero(nowWord,word);

    add("lw\t"+t+", "+std::to_string(offset)+"("+r+")");
}

void mips::store(std::string t,std::string word)
{
    int offset=get_offset(nowWord,word);
    std::string r=sp_or_zero(nowWord,word);

    add("sw\t"+t+", "+std::to_string(offset)+"("+r+")");
}

// 检查t是否是寄存器
bool check(std::string t)
{
    if(t==R_ZERO || t==R_RET || t==R_SP) return 1;
    std::string s=R_HEAD;
    if(t.length()<s.length()) return 0;
    for(int i=0;i<s.length();i++)
        if(t[i]!=s[i])
            return 0;
    return 1;
}

bool is_func(std::string label)
{
    symbolEntry se;
    if(get_symbolEntry("global",label,se))
        return 1;
    return 0;
}

bool is_num(std::string t)
{
    if(t.length()==0 || (t.length()==1 && (t[0]<'0' || t[0]>'9'))) return 0;
    int i=0;
    if(t[0]=='-' || t[0]=='+') i++;
    for(;i<t.length();i++)
        if(t[i]<'0' || t[i]>'9')
            return 0;
    return 1;
}

/*
翻译程序
*/
void mips::deal_instruct(instruct ins)
{
    std::string tmp1="$23",tmp2="$24",tmp3="$25";
    std::string t1,t2,t3;
    // 检查一个
    #define hhh1 do{\
            t1=check(ins.t1)?change(ins.t1):tmp1; \
            }while(0);
    // 检查两个，可能需要load t2
    #define hhh2 do{\
            t1=check(ins.t1)?change(ins.t1):tmp1; \
            t2=check(ins.t2)?change(ins.t2):tmp2; \
            if(t2==tmp2) load(t2,ins.t2); \
            }while(0);
    /*
    加减的判断，需要检查t1,t2和t3，分情况处理
    其中t1不可能是常数，t2如果是常数就要li，t3可以是常数
    */
    #define hhh3 do{\
            if(check(ins.t1)) t1=change(ins.t1);\
            else t1=tmp1;\
            if(check(ins.t2)) t2=change(ins.t2);\
            else if(is_num(ins.t2)) add("li\t"+tmp2+", "+ins.t2),t2=tmp2;\
            else t2=tmp2,load(t2,ins.t2);\
            if(check(ins.t3)) t3=change(ins.t3);\
            else if(is_num(ins.t3)) t3=ins.t3;\
            else t3=tmp3,load(t3,ins.t3);\
            }while(0);

    /*
    跳转指令时需要检查t1和t2，分情况（寄存器、常数、变量）分别处理
    其中t2可以直接赋为常数
    */
    #define bbb2 do{\
        if(check(ins.t1)) t1=change(ins.t1);\
        else if(is_num(ins.t1)) add("li\t"+tmp1+", "+ins.t1),t1=tmp1;\
        else t1=tmp1,load(t1,ins.t1);\
        if(check(ins.t2)) t2=change(ins.t2);\
        else if(is_num(ins.t2)) t2=ins.t2;\
        else t2=tmp2,load(t2,ins.t2);\
        }while(0);

    // 加减乘除，注意最后还可能要store一下t1
    if(ins.op==OP::ADD_)
    {
        hhh3;
        add("add\t"+t1+", "+t2+", "+t3);
        if(t1==tmp1) store(t1,ins.t1);
    }
    else if(ins.op==OP::SUB_)
    {
        hhh3;
        add("sub\t"+t1+", "+t2+", "+t3);
        if(t1==tmp1) store(t1,ins.t1);
    }
    // 乘除还要多判断一下t3
    else if(ins.op==OP::MULT_)
    {
        hhh3;
        if(is_num(t3)) add("li\t"+tmp3+", "+t3),t3=tmp3;
        add("mult\t"+t2+", "+t3);
        add("mflo\t"+t1);
        if(t1==tmp1) store(t1,ins.t1);
    }
    else if(ins.op==OP::DIV_)
    {
        hhh3;
        if(is_num(t3)) add("li\t"+tmp3+", "+t3),t3=tmp3;
        add("div\t"+t2+", "+t3);
        add("mflo\t"+t1);
        if(t1==tmp1) store(t1,ins.t1);
    }
    else if(ins.op==OP::LARRAY_)
    {
        // ins.t1 = ins.t2[ins.t3]
        // ins.t1一定是一个寄存器或者变量
        // ins.t2一定是一个变量
        // ins.t3可能是寄存器、变量、常数
        if(check(ins.t1)) t1=change(ins.t1);
        else t1=tmp1;

        t2=tmp2;
        add("la\t"+t2+", "+std::to_string(get_offset(nowWord,ins.t2))+"("+sp_or_zero(nowWord,ins.t2)+")");

        if(check(ins.t3)) t3=change(ins.t3);
        else if(is_num(ins.t3)) t3=tmp3,add("li\t"+t3+", "+ins.t3);
        else t3=tmp3,load(t3,ins.t3);
        add("sll\t"+t3+", "+t3+", 2");
        add("addu\t"+t2+", "+t2+", "+t3);
        add("lw\t"+t1+", "+"("+t2+")");
        if(t1==tmp1) store(t1,ins.t1);
    }
    else if(ins.op==OP::SARRAY_)
    {
        // ins.t2[ins.t3] = ins.t1
        // ins.t1可能是寄存器、变量或者常数
        // ins.t2一定是变量
        // ins.t3可能是寄存器、变量或者常数
        if(check(ins.t1)) t1=change(ins.t1);
        else if(is_num(ins.t1)) t1=tmp1,add("li\t"+t1+", "+ins.t1);
        else t1=tmp1,load(t1,ins.t1);
        t2=tmp2;
        add("la\t"+t2+", "+std::to_string(get_offset(nowWord,ins.t2))+"("+sp_or_zero(nowWord,ins.t2)+")");
        if(check(ins.t3)) t3=change(ins.t3);
        else if(is_num(ins.t3)) t3=tmp3,add("li\t"+t3+", "+ins.t3);
        else t3=tmp3,load(t3,ins.t3);
        add("sll\t"+t3+", "+t3+", 2");
        add("addu\t"+t2+", "+t2+", "+t3);
        add("sw\t"+t1+", "+"("+t2+")");
    }
    // assign 右边可能是常数
    // 常数用li，变量用move
    else if(ins.op==OP::ASSIGN_)
    {
        if(is_num(ins.t2))
        {
            t1=tmp1;
            add("li\t"+t1+", "+ins.t2);
            if(check(ins.t1)) add("move\t"+change(ins.t1)+", "+t1);
            else store(t1,ins.t1);
        }
        else
        {
            hhh2;
            add("move\t"+t1+", "+t2);
            if(t1==tmp1) store(t1,ins.t1);
        }
    }
    // 接下来是一些跳转指令
    else if(ins.op==OP::BEQ_)
    {
        bbb2;
        add("beq\t"+t1+", "+t2+", "+ins.t3);
    }
    else if(ins.op==OP::BNE_)
    {
        bbb2;
        add("bne\t"+t1+", "+t2+", "+ins.t3);
    }
    else if(ins.op==OP::BLE_)
    {
        bbb2;
        add("ble\t"+t1+", "+t2+", "+ins.t3);
    }
    else if(ins.op==OP::BLT_)
    {
        bbb2;
        add("blt\t"+t1+", "+t2+", "+ins.t3);
    }
    else if(ins.op==OP::JUMP_)
    {
        add("j\t"+ins.t1);
    }
    else if(ins.op==OP::SETLABEL_)
    {
        if(is_func(ins.t1)) nowWord=ins.t1;
        add(ins.t1+":");
        if(ins.t1=="main")
        {
            // 主函数一开始也要分配空间
            add("subu\t$sp, $sp, "+std::to_string(get_total_offset("main")));
        }
    }
    else if(ins.op==OP::RETURN_)
    {
        add("jr\t$ra");
    }
    else if(ins.op==OP::SENDP_)
    {
        // send value t1 to t2(t3) (t2 is offset)
        // t2一定是num
        // t3一定是R_SP
        if(check(ins.t1)) t1=change(ins.t1);
        else if(is_num(ins.t1)) t1=tmp1,add("li\t"+t1+", "+ins.t1);
        else t1=tmp1,load(t1,ins.t1);
        add("sw\t"+t1+", "+ins.t2+"("+change(ins.t3)+")");
    }
    else if(ins.op==OP::CALL_)
    {
        add("jal\t"+ins.t1);
    }
    else if(ins.op==OP::PUSH_)
    {
        // 目前来说用过的寄存器可能是3-22号，加上$ra，总共21个，地址偏移84
        for(int i=3;i<=22;i++)
        {
            add("sw\t$"+std::to_string(i)+", -"+std::to_string((i-2)*4)+"($sp)");
        }
        add("sw\t$ra, -84($sp)");
        add("subiu\t$sp, $sp, 84");
    }
    else if(ins.op==OP::POP_)
    {
        // 同上
        add("lw\t$ra, 0($sp)");
        for(int i=3,j=22;i<=22;i++,j--)
        {
            add("lw\t$"+std::to_string(j)+", "+std::to_string((i-2)*4)+"($sp)");
        }
        add("addiu\t$sp, $sp, 84");
    }
    // read的t1不可能是寄存器
    else if(ins.op==OP::READ_)
    {
        if(ins.t2=="CHAR")
        {
            add("li\t$v0, 12");
            add("syscall");
            add("move\t"+tmp1+", "+"$v0");
            store(tmp1,ins.t1);
        }
        else if(ins.t2=="INT")
        {
            add("li\t$v0, 5");
            add("syscall");
            add("move\t"+tmp1+", "+"$v0");
            store(tmp1,ins.t1);
        }
    }
    // print的t1可能是寄存器
    else if(ins.op==OP::PRINT_)
    {
        if(ins.t2=="CHAR")
        {
            add("li\t$v0, 11");
            t1=check(ins.t1)?change(ins.t1):tmp1;
            if(t1==tmp1) load(t1,ins.t1);
            add("move\t$a0, "+t1);
            add("syscall");
        }
        else if(ins.t2=="INT")
        {
            add("li\t$v0, 1");
            t1=check(ins.t1)?change(ins.t1):tmp1;
            if(t1==tmp1) load(t1,ins.t1);
            add("move\t$a0, "+t1);
            add("syscall");
        }
        else if(ins.t2=="STRING")
        {
            add("li\t$v0, 4");
            add("la\t$a0, "+ins.t3);
            add("syscall");
        }
    }
    else if(ins.op==OP::PARA_)
    {
        // nothing to do
    }
    else if(ins.op==OP::SETSTR_)
    {
        add(ins.t1);
    }
    else if(ins.op==OP::EXIT_)
    {
        add("li\t$v0, 10");
        add("syscall");
    }
}

void mips::deal()
{
    for(instruct ins:inss)
    {
        deal_instruct(ins);
    }
}

void mips::print_all()
{
    for(std::string s:outs)
    {
        if(s[s.length()-1]!=':') std::cout<<"\t";
        std::cout<<s<<std::endl;
    }
}
