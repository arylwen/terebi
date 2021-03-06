/*    /lib/torch.c
 *    from the Dead Souls Object Library
 *    a torch-like inheritable that burns and such
 *    created by Descartes of Borg 960512
 */

#include <lib.h>
#include "include/torch.h"

inherit LIB_BURN;
inherit LIB_ITEM;

static void create(){
    item::create();
    burn::create();
}

int GetRadiantLight(int ambient){
    if( !GetLit() ) return 0;
    else return item::GetRadiantLight(ambient);
}

string GetShort(){
    return item::GetShort() + burn::GetShort();
}

varargs string GetLong(string unused){
    string tmp;

    tmp = burn::GetLong(unused);
    if( tmp != "" ) return item::GetLong(unused) + "\n" + tmp;
    else return item::GetLong(unused);
}

int direct_pray_for_str_for_obj(){ return 1; }
