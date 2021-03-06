/*    /lib/interface.c
 *    from the Dead Souls Object Library
 *    handles user interface issues
 *    created by Descartes of Borg 950428
 *    Version: @(#) interface.c 1.4@(#)
 *    Last Modified: 96/10/15
 */

#include <lib.h>
#include <daemons.h>
#include <message_class.h>
#include "include/interface.h"

inherit LIB_CHAT;
inherit LIB_COMMAND;
inherit LIB_EDITOR;
inherit LIB_NMSH;

private string Terminal;
private mapping Blocked;
private int *Screen;
private static int LogHarass, Client;
private static mapping TermInfo;
string MessageQueue;
int PauseMessages, annoyblock;
int MessageExceptions, BlockAnnoying;

static void create(){
    chat::create();
    command::create();
    editor::create();
    nmsh::create();
    Terminal = "ansi";
    Screen = ({ 80, 20 });
    Blocked = ([]);
}

static string process_input(string str){
    write_file("log_gab", "interface.c.c process_input: " +str+"\n");
    SetCommandFail(0);
    command::process_input(str);
    if( Client ){
        int cl;

        sscanf(str, "%d %s", cl, str);
        write_file("log_gab", "interface.c.c process_input new str " +str+"\n");
    }
	//TODO no editor commands for this version
    //if( (str = editor::process_input(str)) == "" ) return "";
    //else {
        str = nmsh::process_input(str);
        if( str != "" ){
            return chat_command(str);
        }
        else {
            return "";
        }
    //}
}

static void terminal_type(string str){
    if( !stringp(str) ) return;
    else SetTerminal(lower_case(str));
}

static void window_size(int width, int height){ SetScreen(width, height); }

int eventReceive(string message){
	write_file("log_gab", "/lib/interface.c eventReceive: " +message+"\n");
    int max_length = __LARGEST_PRINTABLE_STRING__ - 192;
    if(sizeof(message) > max_length){
        while(sizeof(message)){
            string tmp = message[0..max_length];
            receive(tmp);
            message = replace_string(message, tmp, "");
        }
    }
    else{
    	write_file("log_gab", "/lib/interface.c eventReceive actual: " +message+"\n");
    	receive(message);
    }
}


void receive_message(mixed msg_class, string msg){
    int cl = 0;

    if(intp(msg_class)){
        cl = msg_class;
        eventPrint(msg, cl);
        return;
    }

    else if( msg_class[0] == 'N' ){
        msg_class = msg_class[1..];
        cl |= MSG_NOWRAP;
    }
    else if( msg_class == "prompt" && msg_class == "editor" ) cl |= MSG_NOWRAP;
    switch(msg_class){
    case "smell": case "sound": case "touch": 
        cl |= MSG_ENV;
        break;

    case "snoop":
        cl |= MSG_SYSTEM | MSG_NOCOLOUR;
    case "broadcast":
        cl |= MSG_SYSTEM;
        break;

    case "editor":
        cl |= MSG_EDIT;
        break;

    case "tell": case "shout":
        cl |= MSG_CONV;
        break;

    case "come": case "leave": case "telout": case "telin":
        cl |= MSG_ENV;
        break;

    case "living_item": case "inanimate_item":
        cl |= MSG_ROOMDESC;
        break;

    case "system": case "more":
        cl |= MSG_SYSTEM;
        break;

    case "prompt":
        cl = MSG_PROMPT;
        break;

    case "error":
        cl |= MSG_ERROR;
        break;

    case "help":
        cl |= MSG_HELP;

    default:
        cl |= MSG_ENV;

    }
    eventPrint(msg, cl);
}

static void receive_snoop(string str){ receive_message("snoop", "%"+str); } 

int Setup(){
    command::Setup();
    nmsh::Setup();
    TermInfo = (mapping)TERMINAL_D->query_term_info(Terminal);
}

int eventFlushQueuedMessages(){
    print_long_string(this_object(),MessageQueue);
    MessageQueue = "";
    return 1;
}

varargs int eventPauseMessages(int x, int exceptions){
    if(exceptions) MessageExceptions = exceptions;
    else MessageExceptions = 0;
    if(x) PauseMessages = 1;
    else {
        if(PauseMessages){
            //call_out( (: eventFlushQueuedMessages :), 1);
            eventFlushQueuedMessages();
        }
        PauseMessages = 0;
    }
    return PauseMessages;
}

varargs int eventPrint(string msg, mixed arg2, mixed arg3){
    int msg_class;

    write_file("log_gab", "/lib/interface.c eventPrint: " +msg+"\n");
    //write_file("log_gab", "/lib/interface.c this_object: " +this_object()+"\n");
	//write_file("log_gab", "/lib/interface.c functions : " +functions(this_object())+"\n");

    if( !msg ) return 0;
    if( !arg2 && !arg3 ) msg_class = MSG_ENV;
    else if( !arg2 ){
        if( !intp(arg3) ) msg_class = MSG_ENV;
        else msg_class = arg3;
    }
    else if( !intp(arg2) ) msg_class = MSG_ENV;
    else msg_class = arg2;
    if( !(msg_class & MSG_NOBLOCK) && GetBlocked("all") ) return 0;

    if((msg_class & MSG_ANNOYING) && annoyblock) return 0;

    /* This is no longer necessary, since the commands
     * "mute" and "gag" can now keep things quiet on
     * on channels for individuals if they so wish.
     * if((msg_class & MSG_CHAN) && environment() &&
     *  environment()->GetProperty("meeting room")) return 0;
     */

    if( GetLogHarass() )
        log_file("harass/" + GetKeyName(), strip_colours(msg) + "\n");

    if( !TermInfo ){
    	TermInfo = (mapping)TERMINAL_D->query_term_info(GetTerminal());
    }

    if( !(msg_class & MSG_NOCOLOUR) ){
        int indent;

        if( msg_class & MSG_CONV ) indent = 4;
        else indent = 0;

        if( msg_class & MSG_NOWRAP )
            msg = terminal_colour(msg + "%^RESET%^", TermInfo);
        else
            msg = terminal_colour(msg + "%^RESET%^\n", TermInfo,
              GetScreen()[0], indent);

        write_file("log_gab", "/lib/interface.c eventPrint idented: " +msg+"\n");
    } else if( !(msg_class & MSG_NOWRAP) ){
    	msg = wrap(msg, GetScreen()[0]-1);
    	write_file("log_gab", "/lib/interface.c eventPrint: wrapped " +msg+"\n");
    }

    if(PauseMessages && !(msg_class & MessageExceptions)){
        MessageQueue += msg;
    } else {
        if( Client ) eventReceive("<" + msg_class + " " + msg + " " + msg_class +">\n");
        else eventReceive(msg);
    }
    return 1;
}

varargs int SetBlocked(string type, int flag){
    if( !type ) return 0;
    if( !flag ) flag = !Blocked[type];
    if( Blocked[type] == 2 && !archp(this_player()) ){
        this_player()->eventPrint("Unable to unblock " + type + ".");
        return -1;
    }
    Blocked[type] = flag;
    message("system", "You are "+(Blocked[type] ? "now blocking" :
        "no longer blocking")+" "+type+".", this_object());
    return Blocked[type];
}

int GetBlocked(string type){ return (Blocked["all"] || Blocked[type]); }

int SetClient(int x){
    return 0;
    if( x ) SetTerminal("unknown");
    return (Client = x);
}

int GetClient(){ return Client; }

int SetLogHarass(int x){
    string txt;

    if( GetForced() || (this_player(1) != this_object()) ) return LogHarass;
    if( LogHarass == x ) return LogHarass;
    if( x ){
        txt = "**************** Start of Log *****************\n"+
        "Time: " + ctime( time() ) + "\n";
        if( environment( this_object() ) ) txt += "Place: " +
            file_name( environment( this_object() ) ) + "\n";
    } else {
        txt = "**************** End of Log *****************\n"+
        "Time: " + ctime( time() ) + "\n";
    }
    log_file("harass/" + GetKeyName(), txt);
    return (LogHarass = x);
}

int GetLogHarass(){ return LogHarass; }

int *SetScreen(int width, int height){ 
    if( !width || !height ) return Screen;
    width--;
    if( width * height > __LARGEST_PRINTABLE_STRING__ ){
        if( width > height ) width = __LARGEST_PRINTABLE_STRING__/height;
        else if( height > width ) height = __LARGEST_PRINTABLE_STRING__/width;
        else width = height = (__LARGEST_PRINTABLE_STRING__-1)/2;
    }
    return (Screen = ({ width, height })); 
}

int *GetScreen(){ return Screen; }

string SetTerminal(string terminal){ 
    switch( terminal ){
    case "iris-ansi-net": case "vt100": case "vt220": case "vt102":
    case "vt300": case "dec-vt100":
        terminal = "ansi";
        break;
    case "unknown": case "ansi": case "freedom": case "ansi-status":
    case "xterm": 
        break;
    case "console": case "ibm-3278-2":
        terminal = "unknown";
        break;
    default:
        log_file("terminals", "Unknown terminal type: " + terminal + "\n");
        terminal = Terminal;
        break;
    }
    if( terminal != Terminal ) 
        TermInfo = (mapping)TERMINAL_D->query_term_info(terminal);
    return Terminal = terminal;
}

string GetTerminal(){ return Terminal; }

string GetKeyName(){ return 0; }

int SetAnnoyblock(int i){
    if(!this_player()) return 0;
    if(archp(this_object()) && !archp(this_player())) return 0;
    if(!archp(this_object()) && this_player() != this_object()) return 0;
    if(i) annoyblock = 1;
    else annoyblock = 0;
    return annoyblock;
}

int GetAnnoyblock(){
    return annoyblock;
}

