#include "head.h"
#include "parser.h"
using namespace std;

int main()
{
    system("chcp 65001");

    freopen("testfile.txt","r",stdin);
    freopen("output.txt","w",stdout);
    Parser pa;
    //pa.print_all_tokens();
    pa.chengxu();

    return 0;
}
