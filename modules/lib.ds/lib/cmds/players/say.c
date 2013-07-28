#include <lib.h>
#include <talk_type.h>
#include <daemons.h>

inherit LIB_DAEMON;

int cmd(string str) {
    int maxi;
	string *words;
	string line;
	object other;

	write_file("log_gab", "/lib/cmds/players/say.c str: " +str+"\n");

	maxi=sizeof(words = explode(str, " "));
	if( maxi == 1 ){
		 this_object()->eventPrint("Say what?\n");
		 load_object("/verbs/common/say")->GetHelp();
	} else if (maxi >= 2){
		   if( words[1] == "to" ){
			   if( maxi >= 3){
			       other = get_object(words[2]);
			       if(other){
			           if( maxi >= 4){
			               line = implode(words[3..]," ");
	                       load_object("/verbs/common/say")->do_say_str(words[1]);
			          } else {
						   this_object()->eventPrint("Say what to " + words[2] + "?.\n");
						   load_object("/verbs/common/say")->GetHelp();
			          }
			       } else {
					   this_object()->eventPrint("You cannot find " + words[2] + "here.\n");
					   load_object("/verbs/common/say")->GetHelp();
			       }
			   } else {
				   this_object()->eventPrint("Say to whom ?\n");
				   load_object("/verbs/common/say")->GetHelp();
			   }
		   } else {
			   line = implode(words[1..]," ");
			   write_file("log_gab", "/lib/cmds/players/say.c other_do_say: " +line+"\n");
			   load_object("/verbs/common/say")->other_say_to_liv_str(previous_object(), 0, line);
		   }
	}

	return 1;
}
