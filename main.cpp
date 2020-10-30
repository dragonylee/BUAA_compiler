#include "head.h"
#include "parser.h"
#include "symbol.h"
using namespace std;

int main()
{
    system("chcp 65001");

    freopen("testfile.txt","r",stdin);
    freopen("error.txt","w",stdout);
    Parser pa;
    //pa.print_all_tokens();
    pa.chengxu();
    //print_globalSymbolTable();
    //cout<<endl;
    //print_all_localSymbolTable();

    return 0;
}
