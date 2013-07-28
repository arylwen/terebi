#include <lib.h>
#include <talk_type.h>
#include <daemons.h>

inherit LIB_DAEMON;

int cmd(string str) {
    int maxi;
	string *words;
	object other;

	write_file("log_gab", "/lib/cmds/players/read.c str: " +str+"\n");

	maxi=sizeof(words = explode(str, " "));
	if( maxi == 1 ){
		this_player()->eventPrint("Read what?\n");
	} else if (maxi == 2){
		other = get_object(words[1]);
		if(other){
			load_object("/verbs/items/read")->do_read_obj(other);
		} else {
			this_player()->eventPrint("There is no " + words[1] + " here.\n");
		}
	}

	return 1;
}
