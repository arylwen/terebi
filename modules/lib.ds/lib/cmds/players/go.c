#include <lib.h>
#include <talk_type.h>
#include <daemons.h>

inherit LIB_DAEMON;

int cmd(string str) {
    int maxi;
	string *words;
	
	write_file("log_gab", "/lib/cmds/players/go.c str: " +str+"\n");   
	
	maxi=sizeof(words = explode(str, " "));
	if( maxi == 1 ){
		//load_object("/verbs/roo/look")->do_look();	
        this_player()->eventPrint("Go where?\n");		
	} else if (maxi == 2){
	   load_object("/verbs/rooms/go")->do_go_str(words[1]);
	}
		
	return 1;
}