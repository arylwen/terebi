/*    /lib/command.c
 *    from the Dead Souls Object Library
 *    handles commands of living objects
 *    created by Descartes of Borg 950323
 *    Version: @(#) command.c 1.2@(#)
 *    Last modified: 96/12/07
 */


#include <lib.h>
#include <daemons.h>
#include "include/command.h"

#define OLD_STYLE_PLURALS 1

int Paused = 0;
private static int Forced = 0;
private static int StillTrying = 0;
private static int ParseRecurse = 0;
private static string CommandFail;
private static string *SearchPath;
private static int last_cmd_time = 0;
private static int cmd_count = 1;
private string *CommandHist = ({});
private string *localcmds = ({});
private string *next_command = ({});
private static string *QueuedCommands = ({});
private int MaxCommandHistSize = 20;
static string current_command = "";
static string original_command = "";

int direct_force_liv_str(){ return 1; }
int direct_force_liv_to_str(){ return 1; }


/*  ***************  /lib/command.c driver applies  ***************  */

static void create(){
    SearchPath = ({ DIR_PLAYER_CMDS, DIR_SECURE_PLAYER_CMDS, DIR_CLAN_CMDS,
      DIR_COMMON_CMDS, DIR_SECURE_COMMON_CMDS });
}

static string process_input(string cmd){ 
    current_command = cmd;
    return cmd;
}

/*  ***************  /lib/command.c command lfuns  ***************  */

static int cmdAll(string args){
    object old_agent;
    mixed err;
    string verb, file;

	write_file("log_gab", "/lib/command.c cmdAll args: " +args+"\n");
	
    if(Paused){
        return 0;
    }

    if(!archp(this_player()) && MAX_COMMANDS_PER_SECOND){
        if(last_cmd_time == time()) cmd_count++;
        else {
            last_cmd_time = time();
            cmd_count = 1;
        }
        if(cmd_count > MAX_COMMANDS_PER_SECOND){
            write("You have exceeded the "+MAX_COMMANDS_PER_SECOND+" commands per second limit.");
            return 1;
        }
    }    

    if(sizeof(CommandHist) >= MaxCommandHistSize) CommandHist -= ({ CommandHist[0] }); 
    if(!args) CommandHist += ({ query_verb() });
    else CommandHist += ({ query_verb()+" "+args });

    old_agent = this_agent(this_object());
	write_file("log_gab", "/lib/command.c cmdAll old_agent: " +old_agent+"\n");
    
	verb = query_verb();
	write_file("log_gab", "/lib/command.c cmdAll verb: " +verb+"\n");
	
    if(this_player()->GetSleeping() > 0){
        if(verb != "wake"){
            this_player()->eventPrint("You are asleep.");
            return 1;
        }
    }

    if(BARE_EXITS){
	    write_file("log_gab", "/lib/command.c cmdAll BARE_EXITS\n");
        localcmds = ({});
        filter(this_player()->GetCommands(), (: localcmds += ({ $1[0] }) :));
	    write_file("log_gab", "/lib/command.c cmdAll EXITS: " + environment(this_player())->GetExits() +"\n");
  
        if(member_array(verb,CMD_D->GetCommands()) == -1 &&
          member_array(verb,keys(VERBS_D->GetVerbs())) == -1 &&
          member_array(verb,localcmds) == -1 && environment(this_player())->GetExits()){
            if(member_array(verb,environment(this_player())->GetExits()) != -1) verb = "go "+verb;
            if(member_array(verb,environment(this_player())->GetEnters()) != -1) verb = "enter "+verb;
        }
    }

	//match_command(verb);
    if(COMMAND_MATCHING && sizeof(match_command(verb))) verb = match_command(verb);

    if(OLD_STYLE_PLURALS && args){
        int numba, i;
        string tmp_ret;
        string *line = explode(args," ");
        for(i = 1; i < sizeof(line); i++){
            string element;
            if(!line[i]) error("String handling error in old style plural parser.");
            element = line[i];
            if(sscanf(element,"%d.%s",numba,tmp_ret) == 2){
                if(present(numba+ordinal(numba)+" "+tmp_ret,environment(this_player()))){
                    args = replace_string(args,element,numba+ordinal(numba)+" "+tmp_ret);
                    continue;
                }
            }
            if(numba = atoi(element)){
                object o1;
                string e1, e2;
                e1 = numba+ordinal(numba);
                e2 = line[i-1];
                o1 = present(e2+" "+numba,this_player());
                if(!o1) o1 = present(e2+" "+numba,environment(this_player()));
                if(o1){
                    tmp_ret = e1+" "+e2;
                    args = replace_string(args,e2+" "+numba,tmp_ret);
                }
            }//end single number check
        }
    }

    if(query_custom_command(verb) && query_custom_command(verb) != "" && !creatorp(this_player()) ){
        this_player()->eventPrint("How clever of you. Or lucky. In any case, this command is unavailable to you.");
        return 1;
    }
	
	write_file("log_gab", "/lib/command.c cmdAll not a custom command: " +verb+"\n");
	
    if( !(file = (query_custom_command(verb) )) || query_custom_command(verb) == ""){
	    file = (string)CMD_D->GetCommand(verb, GetSearchPath());
		write_file("log_gab", "command.c file: " +file+"\n");
        if( !(file) ){
            string cmd;
            int dbg;

            if( args ) cmd = verb + " " + args;
            else cmd = verb;
            if( (int)this_object()->GetProperty("parse debug") ) dbg = 1;
            else if( (int)this_object()->GetProperty("debug") ) dbg = 1;
            else dbg = 0;
            if( (err = parse_sentence(cmd, dbg)) == 1 ){
                this_agent(old_agent || 1);
                return 1;
            }
            if( err ){
                if( err == -1 ){
                    if( !(err = (string)VERBS_D->GetErrorMessage(verb)) &&
                      !(err = (string)SOUL_D->GetErrorMessage(verb)) ){
                        err = "Such a command exists, but no default "
                        "syntax is known.";
                    }
                }
                if( intp(err) )  /* MudOS bug */ err = "What?";
                SetCommandFail(err);
            }
            message("error", GetCommandFail(), this_object());
            this_agent(old_agent || 1);
            return 1;
        }
    }

	write_file("log_gab", "command.c cmdAll file: " +file+"\n");
	
    if( (err = (mixed)call_other(file, "cmd", args)) != 1 ){
        string cmd;

        if( err ) SetCommandFail(err);
        if( !args || args == "" ) cmd = verb;
        else cmd = verb + " " + args;
        if( (err = parse_sentence(cmd)) == 1 ){
            this_agent(old_agent || 1);
            return 1;
        }
        if( !err ) err = GetCommandFail();
        message("error", err, this_object());
        this_agent(old_agent || 1);
        return 1;
    }
    this_agent(old_agent || 1);
    return 1;
}

int cmdDebugAll(string args){
    object old_agent;
    mixed err;
    string verb, file;

    old_agent = this_agent(this_object());
    verb = query_verb();
    if( !(file = (string)CMD_D->GetCommand(verb, GetSearchPath())) ){
        string cmd;

        if( args ) cmd = verb + " " + args;
        else cmd = verb;
        if( (err = parse_sentence(cmd, 3)) == 1 ){
            this_agent(old_agent || 1);
            return 1;
        }
        if( err ) SetCommandFail(err);
        message("error", GetCommandFail(), this_object());
        this_agent(old_agent || 1);
        return 1;
    }
    if( (err = (mixed)call_other(file, "cmd", args)) != 1 ){
        string cmd;

        if( err ) SetCommandFail(err);
        if( !args || args == "" ) cmd = verb;
        else cmd = verb + " " + args;
        if( (err = parse_sentence(cmd, 3)) == 1 ){
            this_agent(old_agent || 1);
            return 1;
        }
        if( !err ) err = GetCommandFail();
        message("error", err, this_object());
        this_agent(old_agent || 1);
        return 1;
    }
    this_agent(old_agent || 1);
    return 1;
}

/*  ***************  /lib/command.c lfuns  ***************  */

int Setup(){
    enable_commands();
    add_action( (: cmdAll :), "", 1);
}

int eventForce(string cmd){
    string err;
    int res;
    if(!cmd) return 0;

    cmd = process_input(cmd);
    Forced = 1;
    err = catch(res = command(cmd));
    Forced = 0;
    if(err) error(err);
    return res;
}

int eventForceQueuedCommand(string cmd){
    tell_object(this_object(),"%^RED%^Executing queued command: %^RESET%^"+cmd);
    eventForce(cmd);
}

int eventExecuteQueuedCommands(){
    int i = 0;
    foreach(string tmp in QueuedCommands){
        i++;
        call_out("eventForceQueuedCommand", i, tmp);
        QueuedCommands -= ({ tmp });
    }
}

int eventQueueCommand(string line){
    if(!line || !sizeof(line) || !stringp(line)) return 0;
    if(!this_player()) return 0;
    if(interactive(this_object())){
        if(this_player() && this_player() != this_object()) return 0;
    }
    if(line != "") QueuedCommands += ({ line });
    return 1;
}

int DoneTrying(){
    return StillTrying = 0;
}

int eventRetryCommand(string lastcmd){
    string virb, wrd, prep, rest,ret;
    string *tmp_arr = ({});
    string *prep_arr = MASTER_D->parse_command_prepos_list();
    object tmpob;
    mixed err;

    next_command = ({});
    prep_arr += ({ "out_of" });
    if(!original_command) original_command = lastcmd;
    prep_arr -= ({"here","room","exit","enter"});
    if(previous_object() != master()) return 0;
    StillTrying++;
    filter(explode(lastcmd," "), (: next_command += ({ trim($1) }) :) );
    if(tmpob = get_object(implode(next_command[1..]," "))){
        ret = next_command[0]+" a "+implode(next_command[1..]," ");
    }
    else if(sizeof(next_command) == 2){
        ret = next_command[0]+" a "+next_command[1];
    }
    else if(sizeof(next_command) == 3){
        if(member_array(next_command[1],prep_arr) != -1) 
            ret = next_command[0]+" "+next_command[1]+" a "+next_command[2];
        else ret = next_command[0]+" a "+next_command[1]+" "+next_command[2];
    }
    else if(sizeof(next_command) == 4 && StillTrying < MAX_COMMANDS_PER_SECOND){
        ret = next_command[0]+" a "+next_command[1]+" "+next_command[2]+" "+next_command[3];
    }

    if(!ret || !sizeof(ret)) ret = implode(next_command," ");

    if(StillTrying > 3 ){
        int i;
        tmp_arr = ({});
        tmp_arr = explode(ret," ");
        ret = "";
        for(i = 0; i < sizeof(tmp_arr);i++){
            ret += " "+tmp_arr[i];
            if(member_array(tmp_arr[i],prep_arr) != -1 && tmp_arr[i+1] != "a") ret += " a";
        }
        ret = trim(ret);
    }

    if(StillTrying > 3 && tmp_arr[1] != "a"){
        ret = tmp_arr[0]+" a "+implode(tmp_arr[1..]," ");
    }
    if(COMMAND_MATCHING){
        string vb;
        next_command = ({});
        if(!ret) ret = "";
        tmp_arr = explode(ret," ");
        if(sizeof(tmp_arr)){
            vb = match_command(tmp_arr[0]);
            if(sizeof(vb)) next_command = ({ vb });
            else next_command = ({ tmp_arr[0] });
            foreach(string element in tmp_arr[1..]){
                next_command += ({ element });
            }
            ret = implode(next_command," ");
        }
    }

    if(original_command && sizeof(original_command) && (StillTrying == 6 ||!ret)){
        string direct, indirect;
        string junk;
        string *generals = ({"my","a","first","1st"});
        int i, j;
        next_command = ({});
        original_command = replace_string(original_command,"out of","out_of");
        filter(explode(original_command," "), (: next_command += ({ trim($1) }) :) );
        if(!sizeof(next_command)) next_command = ({ original_command });
        virb = next_command[0];
        next_command = next_command[1..];
        j = sizeof(next_command);
        for(i = 0; i < j-1; i++){
            mixed *foo;
            if(get_object(implode(next_command[0..i]," "))){
                if(member_array(next_command[0], generals) == -1)
                    foo = ({ "a" }) + next_command[0..i];
                else foo = next_command[0..i];
                direct = implode(foo," ");
                next_command = next_command[i+1..];
                break;
            }
        }
        if(member_array(next_command[0], prep_arr) != -1){
            junk = next_command[0];
            next_command = next_command[1..];
        }
        if(direct && (j = sizeof(next_command))){
            mixed *foo;
            for(i = 0; i < j-1; i++){
                if(get_object(implode(next_command[0..i]," "))){
                    if(member_array(next_command[0], generals) == -1)
                        foo = ({ "a" }) + next_command[0..i];
                    else foo = next_command[0..i];
                    indirect = implode(foo," ");      
                    next_command = next_command[i+1..];
                    break;
                }    
            }
        }
        ret = virb+" "+direct+" "+(junk ? junk+" " : "")+indirect;
        if(sizeof(next_command)) ret += " "+implode(next_command," ");
    } 

    if(StillTrying > 7){	
        write("Your command is ambiguous. Please be more specific. Which thing do you mean?");
        StillTrying = 0;
        original_command = 0;
        return 1;
    }

    if(!ret || !sizeof(ret)) ret = implode(next_command," ");
    if(err = parse_sentence(ret)){
        if(stringp(err) && sizeof(trim(err))){
            write(err);
            return 1;
        }
    }
    return 1;
}

/*  **********  /lib/command.c data manipulation functions  ********** */

string *AddSearchPath(mixed val){
    if(stringp(val)){
        if(!strsrch(val,"/secure/cmds/admins") || !strsrch(val,"/cmds/admins")){
            if(!(int)master()->valid_apply(({ "SECURE", "ASSIST", "LIB_CONNECT" })) ){
                tell_creators("Security violation in progress: "+identify(previous_object(-1)) + ", "+get_stack());
                error("Illegal attempt to modify path data: "+identify(previous_object(-1)) + ", "+get_stack());

            }
        }
        val = ({ val });
    }

    else if(!pointerp(val)) error("Bad argument 1 to AddSearchPath()\n");
    return (SearchPath = distinct_array(SearchPath + val));
}

string *RemoveSearchPath(mixed val){
    if(stringp(val)) val = ({ val });
    else if(!pointerp(val)) error("Bad argument 1 to RemoveSearchPath()\n");
    return (SearchPath -= val);
}

string *GetSearchPath(){ return SearchPath; }

int GetForced(){ return Forced; }

int GetClient(){ return 0; }

static string *GetCommandHist(){
    return CommandHist;
}

string GetLastCommand(){
    if(!GetForced() && (this_player() == this_object() || previous_object() == master())){
        return CommandHist[sizeof(CommandHist)-1];
    }
    else return "";
}

string GetCurrentCommand(){
    if(!this_player()) return "";
    if(this_player() != this_object()) return "";
    return current_command;
}

int GetMaxCommandHistSize(){
    return MaxCommandHistSize;
}

int SetMaxCommandHistSize(int i){
    if(!i || i < 2) i = 2;
    return MaxCommandHistSize = i;
}

int SetPlayerPaused(int i){
    if( !this_player() || !archp(this_player()) ){
        error("Illegal attempt to pause a player: "+get_stack()+" "+identify(previous_object(-1)));
        log_file("adm/pause",timestamp()+" Illegal attempt to access SetPlayerPaused on "+identify(this_object())+" by "+identify(previous_object(-1))+"\n");
    }
    Paused = i;
    return Paused;
}

int GetPlayerPaused(){
    return Paused;
}

string SetCommandFail(string str){ 
    if( !str || str == "" ){
        if(!creatorp(this_player())) CommandFail = "Try \"help commands\" for a list of some commands.";
        if(creatorp(this_player())) CommandFail = "Try \"help creator commands\" for a list of some creator commands.";
        return CommandFail;
    }
    else return (CommandFail = str);
}

string GetCommandFail(){ return CommandFail; }
