#include "head.h"
#include "parser.h"
#include "symbol.h"
#include "mid.h"
#include "mips.h"
using namespace std;

int main()
{
    system("chcp 65001");

    freopen("testfile.txt","r",stdin);
    freopen("mips.txt","w",stdout);

    extern int global_offset;
    global_offset=0x10000000;   // due to mips

    Parser pa;
    //pa.print_all_tokens();
    pa.chengxu();
    //print_all_localSymbolTable();
    //print_all_instruct();

    mips mp;
    mp.deal();
    mp.print_all();

    return 0;
}
