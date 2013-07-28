#include <lib.h>
#include <talk_type.h>
#include <daemons.h>

inherit LIB_DAEMON;

int cmd(string str) {
    int maxi;
	string *words;
	object other;
	
	write_file("log_gab", "/lib/cmds/players/look.c str: " +str+"\n");   
	
	maxi=sizeof(words = explode(str, " "));
	if( maxi == 1 ){
		load_object("/verbs/items/look")->do_look();			
	} else if (maxi == 3){
	   if( words[2] == "me" ){
	       load_object("/verbs/items/look")->do_look_at_obj(this_player(), "me");
	   } else {
		   other = get_object(words[2]);
		   write_file("log_gab", "/lib/cmds/players/look.c other: " +other+"\n");
		   if(other){
			   load_object("/verbs/items/look")->do_look_at_obj(other, words[2]);
		   } else {
	           load_object("/verbs/items/look")->do_look_at_str(words[2]);
		   }
	   }
	}
		
	return 1;
}
