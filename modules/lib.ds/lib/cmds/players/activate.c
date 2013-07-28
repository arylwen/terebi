#include <lib.h>
#include <talk_type.h>
#include <daemons.h>

inherit LIB_DAEMON;

int cmd(string str) {
    int maxi;
	string *words;
	object other;

	write_file("log_gab", "/lib/cmds/players/activate.c str: " +str+"\n");

	maxi=sizeof(words = explode(str, " "));
	if( maxi == 1 ){
		load_object("/verbs/items/activate")->do_activate();
	} else if (maxi == 2){
	   if( words[1] == "me" ){
	       load_object("/verbs/items/activate")->do_activate_obj(this_player(), 0);
	   } else {
		   other = get_object(words[1]);
		   write_file("log_gab", "/lib/cmds/players/activate.c other: " +other+"\n");
		   if(other){
			   load_object("/verbs/items/activate")->do_activate_obj(other, 0);
		   } else {
			   this_player()->eventPrint("There is no "+words[1]+" here.");
		   }
	   }
	}

	return 1;
}
