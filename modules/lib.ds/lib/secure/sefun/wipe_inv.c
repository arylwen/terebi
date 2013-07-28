#include <lib.h>
#include <dirs.h>

int wipe_inv(mixed dude){
    string nom, str, orig_file, bak_file, contents;

	nom = dude->GetKeyName();
    if(objectp(dude) && !nom) return 0;
	nom = lower_case(dude);
    if(stringp(dude) && !nom) return -1;
    if(!nom || !sizeof(nom)) return -2;
    if(!user_exists(nom)) return -3;

    str = DIR_PLAYERS "/" + nom[0..0] + "/" + nom;
    if( !str || !file_exists(str + __SAVE_EXTENSION__) ) 
        str = DIR_CRES "/" + nom[0..0] + "/" + nom;

    orig_file = str + __SAVE_EXTENSION__;
    bak_file = str + "_rescue" + __SAVE_EXTENSION__;

    cp(orig_file, bak_file);
    contents = replace_string(read_file(orig_file),"\n",";\n");
    contents = remove_matching_line(contents, "Inventory");
    contents = replace_string(contents, ";\n", "\n");
    //write_file(generate_tmp()+".o",contents);
    write_file(orig_file, contents,1);
    return 1;
}
