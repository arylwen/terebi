/*    /daemon/chat.c
 *    from the Nightmare 3.2 Mudlib
 *    daemon to handle all mud chat channels
 *    created by Descartes of Borg 931220
 *
 *    IMC2 support added by Shadyman 2007-Sep-24
 *    Feelings support added by Shadyman 2007-Sep-24
 *    "mapping tags" added by Shadyman 2007-Sep-24
 */

#ifndef LOG_REMOTE_CHANS
#define LOG_REMOTE_CHANS 0
#endif

#ifndef LOG_LOCAL_CHANS
#define LOG_LOCAL_CHANS 1
#endif

#ifndef CHANNEL_PIPES
#define  CHANNEL_PIPES 0
#endif

#include <lib.h>
#include <config.h>
#include <pov.h>
#include <daemons.h>
#include <origin.h>
#include <message_class.h>
#include "include/chat.h"

inherit LIB_DAEMON;

string suspect,site,chan;
static private mapping Channels;
static private mapping chanlast;

static private string *local_chans = ({"newbie","cre","gossip","admin","error", "intermud",
  "priest", "mage", "explorer", "thief", "fighter", "death", "connections", "muds" });
static private string *remote_chans = ({ "Server01:ichat", "Server01:ibuild",
  "Server01:pchat", "Server02:i2chat", "Server02:i3chat", "Server02:icode",
  "Server02:igame", "Server02:inews", "Server02:irc" });
static string *syschans = ({ "intermud", "death", "connections", "muds" });

static private mapping localchans = ([
  //I3 Channels
  "imud_code": "intercre",
  "imud_gossip": "intergossip",
  "ie_flibcode": "foundation",
  "dutch": "dutch",
  "dead_test4": "ds_test",
  "dead_souls": "ds",

  //"free_speech": "fs",
  //"discworld-chat": "dw",
  //"discworld-cre": "dwcre",
  "lpuni": "lpuni",
  //IMC2 Channels
  "Server01:ibuild": "ibuild",
  "Server01:ichat": "ichat",
  "Server01:pchat": "pchat",
  "Server02:i2chat": "i2chat",
  "Server02:i3chat": "i3chat",
  "Server02:icode": "i2code",
  "Server02:igame": "i2game",
  "Server02:inews": "i2news",
  "Server02:irc": "irc",
]);

static private mapping remotechans = ([
  //I3 Channels
  "intercre": "imud_code",
  "intergossip": "imud_gossip",
  "foundation": "ie_flibcode",
  "dutch": "dutch",
  "ds_test": "dead_test4",
  "ds": "dead_souls",
  //"fs": "free_speech",
  //"dw": "discworld-chat",
  //"dwcre": "discworld-cre",
  "lpuni": "lpuni",

  //IMC2 Channels
  "ibuild" : "Server01:ibuild",
  "ichat" : "Server01:ichat",
  "pchat" : "Server01:pchat",
  "i2chat" : "Server02:i2chat",
  "i3chat" : "Server02:i3chat",
  "i2code" : "Server02:icode",
  "i2game" : "Server02:igame",
  "i2news" : "Server02:inews",
  "irc" : "Server02:irc",
]);

static private mapping tags = ([
  "intermud"    : "%^WHITE%^",
  "muds"        : "%^WHITE%^",
  "connections" : "%^BOLD%^WHITE%^",
  "death"       : "%^BOLD%^RED%^",
  "cre"         : "%^BOLD%^GREEN%^",
  "admin"       : "%^BOLD%^MAGENTA%^",
  "newbie"      : "%^BOLD%^B_YELLOW%^",
  "gossip"      : "%^BOLD%^B_BLUE%^",

  "ds"          : "%^YELLOW%^",
  "dchat"	 :"%^CYAN%^",
  "intergossip" : "%^GREEN%^",
  "intercre"    : "%^ORANGE%^",

  "ibuild"      : "%^B_RED%^%^YELLOW%^",
  "ichat"       : "%^B_RED%^%^GREEN%^",
  "pchat"       : "%^B_RED%^%^BOLD%^GREEN%^",
  "i2game"      : "%^B_BLUE%^",
  "i2chat"      : "%^B_GREEN%^",
  "i3chat"      : "%^B_RED%^",
  "i2code"      : "%^B_YELLOW%^%^RED%^",
  "i2news"      : "%^B_YELLOW%^%^BLUE%^",
  "irc"         : "%^B_BLUE%^%^GREEN%^",

  "default"     : "%^BOLD%^BLUE%^",
]);

static void create() {
    object pl;
    string *tmp_arr = ({});
    daemon::create();
    SetNoClean(1);
    Channels = ([]);

    if(find_object(INTERMUD_D)){
        if(arrayp(INTERMUD_D->GetChannels()))
            remote_chans += INTERMUD_D->GetChannels();
    }

    foreach(string kanal in local_chans + syschans){
        if( !Channels[kanal] ) Channels[kanal] = ({});
    }
    foreach(pl in users()) {
        string *chans;
        string channel;

        if( pl && !(chans = (string *)pl->GetChannels()) ) continue;
        foreach(channel in chans) {
            if( !Channels[channel] ) Channels[channel] = ({});
            Channels[channel] = distinct_array(Channels[channel] + ({ pl }));
        }
    }
    foreach( string channel in local_chans ){
        tmp_arr += ({ channel + "emote" });
        tmp_arr += ({ channel + ":" });
    }

    local_chans += tmp_arr;
}

string *AddRemoteChannel(mixed chan){
    string *ret = copy(remote_chans);
    if(base_name(previous_object()) != INTERMUD_D) return ret;
    if(stringp(chan)) chan = ({ chan });
    if(!arrayp(chan)) return ret;
    foreach(string element in chan){
        if(member_array(element, local_chans) != -1){
            chan -= ({ element });
        }
    }
    return copy(remote_chans += chan);
}

string *RemoveRemoteChannel(mixed chan){
    string *ret = copy(remote_chans);
    if(base_name(previous_object()) != INTERMUD_D) return ret;
    if(stringp(chan)) chan = ({ chan });
    if(!arrayp(chan)) return ret;
    foreach(string element in chan){
        if(member_array(element, local_chans) != -1){
            chan -= ({ element });
        }
    }
    return copy(remote_chans -= chan);
}

string *GetRemoteChannels(){
    return copy(remote_chans);
}

string decolor(string str){
    string s1 = "", s2, s3, test;
    int tmp = 2;
    if(sscanf(str,"%s<%s>%s",s1,s2,s3) != 3)
        tmp = sscanf(str,"<%s>%s",s2,s3);
    if(tmp != 2) return str;
    else {
        test = s1+"<"+s2+">%^RESET%^"+strip_colours(s3);
        return test;
    }
}

varargs int CanListen(object who, string canal){
    if(!RESTRICTED_INTERMUD) return 1;
    if(canal && member_array(canal, local_chans) != -1) return 1;
    else return imud_privp(who);
}

varargs int CanTalk(object who, string canal){
    if(!RESTRICTED_INTERMUD) return 1;
    if(canal && member_array(canal, local_chans) != -1) return 1;
    else return imud_privp(who);
}

string *eventRegisterMember(string *chans) {
    string *tmp;
    object ob;
    string channel;

    if( !living(ob = previous_object()) ) return ({});
    tmp = ({});
    foreach(channel in chans) {
        /* just check out for secure channels */
        switch(channel) {
        case "admin":
            if( !archp(ob) ) break;
        case "cre": case "intercre": case "intergossip":
            if( !creatorp(ob) ) break;
        default:
            if( !Channels[channel]) Channels[channel] = ({});
            Channels[channel] = distinct_array(Channels[channel] + ({ ob }));
            tmp += ({ channel });
        }
    }
    return tmp;
}

string *eventRemoveMember(string *chans) {
    object ob;
    string channel;

    if( !living(ob = previous_object()) ) return({});
    foreach(channel in chans) {
        if( !Channels[channel] ) continue;
        else Channels[channel] -= ({ ob });
        if( !sizeof(Channels[channel]) ) map_delete(Channels, channel);
    }
    return chans;
}

int cmdLast(string feep){

    if(!chanlast||!Channels[feep]||member_array(this_player(), Channels[feep])==-1){

        this_player()->eventPrint("You are not subscribed to that channel.", MSG_ERROR);
        return 1;
    }
    if(!sizeof(chanlast[feep]))
    {
        this_player()->eventPrint("That channel has no backlog.", MSG_ERROR);
        return 1;
    }
    if(!CanListen(this_player(),feep)){
        write("You lack privileges to that channel.");
        return 1;
    }
    this_player()->eventPrint(implode(chanlast[feep], "\n"));
    return 1;
}

static int LogIt(string what, string where, string canale){
    if( (member_array(canale,local_chans) != -1 && LOG_LOCAL_CHANS) ||
      ( member_array(GetRemoteChannel(canale),remote_chans) != -1 && LOG_REMOTE_CHANS) ){
        unguarded( (: write_file($(where), $(what)) :) );
        return 1;
    }
    else return 0;
}

varargs int eventAddLast(string feep, string str, string pchan, string pmsg, string pwho)
{
    string plainmsg;
    string Chan=feep;
    if(!chanlast)
        chanlast=([]);
    if(!sizeof(chanlast[Chan]))
        chanlast[Chan] = ({});
    if(sizeof(chanlast[Chan]) == 50)
        chanlast[Chan] = chanlast[Chan][1..sizeof(chanlast[Chan])];
    chanlast[Chan] += ({ str });
    Chan = GetLocalChannel(Chan);

    if (Chan == "death") return 1;

    //Log in either SQL or file
#ifdef MYSQL
    if (MYSQL_D->sql_request("INSERT INTO LOG_CHAT (Channel,Who,What) VALUES (\'"+ escape(pchan) +"\',\'" + escape(pwho) + "\',\'" + escape(pmsg) + "\')") == 0) {
        true();
    }
#endif
    Chan = GetLocalChannel(Chan);
    if(!pchan || pchan == "") pchan = "foo";
    plainmsg = "bar";
    if(pchan) plainmsg = "<" + pchan + "> ";
    if(pmsg) plainmsg += pmsg;
    if(pwho && pwho !="") plainmsg = pwho+" "+plainmsg;
    if(pchan && pchan != "admin"){
        LogIt("["+timestamp()+"] "+plainmsg+"\n", "/log/chan/"+Chan, Chan);
    }
    else {
        LogIt("["+timestamp()+"] "+plainmsg+"\n", "/secure/log/"+Chan, Chan);
    }
    return 1;
}

int cmdChannel(string verb, string str) {
    string msg, name, rc, target, targetkey, target_msg, emote_cmd, remains;
    string *exploded;
    mixed array msg_data;
    object ob = 0;
    int i, emote, forcedemote;

    if(grepp(verb,"|")){
        string foo, bar;

        if(CHANNEL_PIPES){
            if(grepp(verb,"|morse")){
                str = morse(str);
                verb = replace_string(verb,"|morse","");
            }

            if(grepp(verb,"|colorize")){
                str = dbz_colors(str, 0);
                verb = replace_string(verb,"|colorize","");
            }

            if(grepp(verb,"|annoy")){
                str = dbz_colors(str,2);
                verb = replace_string(verb,"|annoy","");
            }
            if(grepp(verb,"|file")){
                if(!file_exists(str) || !(str = read_file(str))){
                    write("Can't read that file.");
                    return 0;
                }
                verb = replace_string(verb,"|file","");
            }
        }

        if(sscanf(verb, "%s|%s", foo, bar) == 2) verb = foo;
    }

    if(grepp(verb, ":")){
        verb = replace_string(verb,":","emote");
    }

    if(grepp(verb, ";")){
        verb = replace_string(verb,";","forcedemote");
    }


    if(sizeof(str) > 2){

        if((str[0..0] == ":" || str[0..0] == ";") &&
          alphap(str[1..1]) && str[2..2] != " "){
            if(str[0..0] == ";" && !grepp(verb,"forcedemote")) 
                verb = replace_string(verb,"emote","") + "forcedemote";
            else if(str[0..0] == ":" && !grepp(verb,"emote")) verb += "emote";
            str = str[1..];
        }

    }

    //******LIST******
              //allow "list <chan>" to list users listening
    if( verb == "list" ) {
        string *who;
        string ch, mud;

        if( !str ) return 0;

        //Find the remote listing for a channel on a specific mud
        if( sscanf(str, "%s@%s", ch, mud) == 2 ) {
            mud = trim(mud);
            if(!alphap(last(mud,1))) mud = truncate(mud,1);

            if( !Channels[ch] ) return 0;

            if( member_array(this_player(), Channels[ch]) == -1 ) return 0;

            if( ch == (ch = GetRemoteChannel(ch)) ) {
                if(!creatorp(this_player())){
                    write("Remote channel information is not available to players.");
                    return 1;
                }
            }

            if( !(mud = (string)INTERMUD_D->GetMudName(mud)) ) {
                this_player()->eventPrint(mud_name() + " is not aware of "+
                  "such a place.", MSG_ERROR);
                return 1;
            }

            if(!CanTalk(this_player(),verb)) {
                write("You lack privileges to that channel.");
                return 1;
            }
            SERVICES_D->eventSendChannelWhoRequest(ch, mud);
            this_player()->eventPrint("Remote listing request sent.",
              MSG_SYSTEM);
            return 1;
        }
        else ch = str;

        //If no such channel, or not a part of that channel, then no list.
        if( !Channels[ch] ) return 0;
        if( member_array(this_player(), Channels[str]) == -1 ) return 0;

        //Build and print the list of listeners
        who = GetChannelList(str);
        msg = "Online: " + implode(who, "   ");
        this_player()->eventPrint(msg, MSG_SYSTEM);
        return 1;
    }
    //******END LIST******


        //All emotes will fall into this IF structure and get tagged
    //as emote = 1 or forcedemote = 1

    //If it's a verb+emote, de-emote the verb, and mark as an emote
    if(grepp(verb, "emote")) {
        //Get the real channel
        if(grepp(verb, "forcedemote")){
            verb = replace_string(verb,"forcedemote","");   
            forcedemote = 1;
        }
        else verb = replace_string(verb,"emote","");
        emote = 1;
    }

    if(!strsrch(str,"^encode")) str = morse("(encoded):  "+str[7..]);

    if(find_object(INTERMUD_D) && !sizeof(remote_chans))
        remote_chans = INTERMUD_D->GetChannels();

    if(member_array(GetRemoteChannel(verb), remote_chans) == -1 &&
      member_array(verb, local_chans) == -1) local_chans += ({ verb });


    //******Access Checks
    //No talking if you're not allowed.
    if ( !CanTalk(this_player(),verb) ) {
        write("You lack privileges to that channel.");
        return 1;
    }

    //Toggle channel blocking
    if ( emote == 1 && ( !str || str == "" ) ) {
        this_player()->SetBlocked(verb + "emote");
        return 1;
    } else if ( !str || str == "" ) {
        this_player()->SetBlocked(verb);
        return 1;
    }

    //Syschans aren't for chatting on, only listening
    if ( member_array(verb, syschans) != -1 ) {
        write("This is not a channel for chatting.");
        return 1;
    }

    //If gagged, you can't talk on channels
    if ( this_player()->GetProperty("gag") ) {
        write("You have gag mode enabled. Type: 'gag off' to talk on channels.");
        return 1;
    }

    //If not part of the channel, no chatting
    if( member_array(this_player(), Channels[verb]) == -1 ) return 0;

    //If blocked, allow no chatting
    if( (int)this_player()->GetBlocked(verb) ) {
        if( (int)this_player()->GetBlocked("all") ) {
            this_player()->eventPrint("You cannot chat while totally blocked.",
              MSG_ERROR);
            return 1;
        }
        this_player()->eventPrint("Turn this channel on to talk on it.", MSG_ERROR);
        return 1;
    }
    //******End Access Checks


    //If there's no channel matching now, then it's a typo or wasn't meant for this file to pick up.
    if( !Channels[verb] ) return 0;

    //Find the remote channel's name, based on the local, pretty name
    rc = GetRemoteChannel(verb);

    //Check emotes
    if (emote == 1) {
        exploded = explode(str, " "); //BOOM!!! We have an array of words.
        if (sizeof(exploded) <= 1) {
            emote_cmd = str;
            remains = 0;
        } else {
            emote_cmd = exploded[0];
            remains = implode(exploded[1..]," ");
        }

        //***********************************
        //Find a target for targetted emotes.
        //***********************************

        if( !remains ) { //If no arguments and just one word
        msg_data = SOUL_D->GetChannelEmote(emote_cmd, ""); //Search for a feeling that matches
    } else { //If there is an argument to the emote,
        if( ob = find_living(target = convert_name(remains)) ) {  //If there is a living target
            msg_data = SOUL_D->GetChannelEmote(emote_cmd, "LIV");
            //If it's not there, get the emote's LVS text.
            //if (!msg_data)
            //	msg_data = SOUL_D->GetChannelEmote(emote_cmd, "LVS", remains);
        } else if( strsrch(target, "@") == -1 || rc == verb ) { //If no living target
            string array words = explode(remains, " ");
            target = "";
            for(i=0; i<sizeof(words); i++) {
                target += lower_case(words[i]);
                if( ob = find_living(target) ) {
                    if( i < sizeof(words)-1 ) {
                        remains = implode(words[(i+1)..], " ");
                    } else {
                        remains = 0;
                    }
                    //If it's not there, get the emote's LVS STR text.
                    if (!msg_data)
                        msg_data = SOUL_D->GetChannelEmote(emote_cmd, "LVS STR", remains);
                    break;
                }
            }

            if( !ob ) {
                msg_data = SOUL_D->GetChannelEmote(emote_cmd, "STR", remains);
                target = 0;
            }

        } else if ( rc != verb ) {
            string array words;

            //Find any @'s in the remains.. Should be User@Mud
            i = strsrch(remains, "@", -1);

            //If there's not enough room for a proper name@mud, just do it as a string
            if ( i >= strlen(remains)-1 ) {
                msg_data = SOUL_D->GetChannelEmote(emote_cmd, "STR", remains);
                target = 0;
            } else { //Otherwise, call mud and find user
                string mud,temp;

                words = explode(remains[(i+1)..], " ");
                target = remains[0..i];
                remains = "";
                while(sizeof(words)) {
                    temp = implode(words, " ");
                    temp = trim(temp);
                    if(!alphap(last(temp,1))) temp = truncate(temp,1);
                    mud = INTERMUD_D->GetMudName(lower_case(temp));
                    if (!mud) mud = IMC2_D->find_mud(lower_case(temp));
                    if( mud ) {
                        target += mud;
                        break;
                    }
                    if( remains == "" ) remains = words[<1];
                    else remains = words[<1] + " " + remains;
                    words = words[0..<2];
                }

                //If we couldn't find the mud,
                if ( !mud ) {
                    msg_data = SOUL_D->GetChannelEmote(emote_cmd, "STR", remains);
                    target = 0;
                } else {
                    if ( trim(remains) == "" ) {
                        msg_data = SOUL_D->GetChannelEmote(emote_cmd, "LIV");
                    } else {
                        msg_data = SOUL_D->GetChannelEmote(emote_cmd, "LIV STR", remains);
                    }
                }
            }
        }
    } //Done finding target

    //***********************************
    //Find the target's proper name and target the message at him/her.
    //***********************************

    if ( msg_data ) { //There's a target
    string sgen = this_player()->GetGender();
    string tgen = 0;

    if ( ob ) { //If a local user
        target = ob->GetName();
        tgen = ob->GetGender();
    } else if ( target ) { //If a mud user
        string user, mud;

        sscanf(target, "%s@%s", user, mud);
        targetkey = target;
        tgen = SERVICES_D->GetRemoteGender(user, mud);
        target = SERVICES_D->GetRemoteDisplayName(user, mud);
        if( !target ) target = capitalize(targetkey);
    }

    //Calculate the viewpoint for 3rd parties
    str = create_message(POV_OBSERVER, msg_data[0][0],
      msg_data[0][1], "$N", sgen, "$O", tgen,
      msg_data[1]);

    //If it's targetted, calculate the message for the target
    if ( target ) {
        target_msg = create_message(POV_TARGET, msg_data[0][0],
          msg_data[0][1], "$N", sgen,
          "$O", tgen, msg_data[1]);
        target_msg = replace_string(target_msg, "$O's", "your");
    }

} else { //There's no target. Spurt it out like the user put it in.
    //Forced emotes only allow real emotes, not custom ones.
    if (forcedemote == 1) {
        if ( member_array( emote_cmd,SOUL_D->GetEmotes() ) > -1 ) {
            write("Invalid syntax. See %^CYAN%^help "+emote_cmd+"%^RESET%^ for a list of usages.");
            return 1;
        } else {
            write("No such feeling. See %^CYAN%^help feelings%^RESET%^ for a list of feelings.");
            return 1;
        }
    } else {
        str = "$N " + str;
        target = 0;
    }
}
}


//If admin or cre channels, Capitalize a person's real name, because admins can be physically hidden
if( verb == "admin" || verb == "cre" ) {
    if( !(name = (string)this_player()->GetCapName()) )
        name = capitalize((string)this_player()->GetKeyName());
}
else name = (string)this_player()->GetName();

//Add the "Name" $N to the string
if(!grepp(str,"$N") && emote) str = "$N "+str;

//Send locally
eventSendChannel(name, verb, str, emote, target, target_msg);

//If it's a remote channel, send it remotely.
if(member_array(GetRemoteChannel(verb), remote_chans) != -1
  && member_array(verb, local_chans) == -1){
    if (rc[0..5] == "Server") { //It's an IMC2 channel
        if( ob ) {
            IMC2_D->channel_out(name, rc, replace_string(replace_string(str,"$N ",""),"$O",target), emote);
        } else if ( targetkey ) {
            IMC2_D->channel_out(name, rc, replace_string(replace_string(str,"$N ",""),"$O",targetkey), emote);
        } else {
            IMC2_D->channel_out(name, rc, replace_string(str,"$N ",""), emote);
        }
    } else { //It's an I3 channel
        if( ob ) {
            SERVICES_D->eventSendChannel(name, rc, str, emote, convert_name(target), target_msg);
        } else {
            SERVICES_D->eventSendChannel(name, rc, str, emote, convert_name(targetkey), target_msg);;
        }
    }
}

return 1;
}

varargs void eventSendChannel(string who, string ch, string msg, int emote,
  string target, string targmsg) {
    object channeler = find_player(lower_case(who));
    string pchan,pmsg;

    string chatlayout = "%s %s<%s>%s %s"; //Default: "%s %s<%s>%s %s" -> "Name COLOR<channel>RESET talks."
    string emotelayout = "%s<%s>%s %s"; //Default: "%s<%s>%s %s" -> "COLOR<channel>RESET Name emotes."

    //Uncomment these next two lines instead of the two above for another channel chat format
    //string chatlayout = "%s says, %s(%s)%s '%s'"; //Default: "%s %s<%s>%s %s" -> "Name COLOR<channel>RESET talks."
    //string emotelayout = "%s(%s)%s %s"; //Default: "%s<%s>%s %s" -> "COLOR<channel>RESET Name emotes."

    pchan=ch;
    if(!channeler) channeler = this_player();

    if(!strsrch(msg,"-.--. . -. -.-. --- -.. . -.. -.--.- ---...")) msg = unmorse(msg);
    if(targmsg && !strsrch(targmsg,"-.--. . -. -.-. --- -.. . -.. -.--.- ---..."))
        targmsg = unmorse(targmsg);

    if(this_player() && this_player() != channeler) channeler = this_player();

    if(!strsrch(base_name(previous_object()), "/realms/") ||
      !strsrch(base_name(previous_object()), "/open/")) {
        return 0;
    }

    if(member_array(ch, syschans) != -1) {
        emote = 0;
    }
    if(channeler){
        if(!CanTalk(channeler, ch) && member_array(ch, syschans) == -1){
            return;
        }
    }
    if( file_name(previous_object()) == SERVICES_D || file_name(previous_object()) == IMC2_D) {
        ch = GetLocalChannel(ch);
        if( emote && sizeof(who)) msg = replace_string(msg, "$N", who);
    }
    else if( origin() != ORIGIN_LOCAL && previous_object() != master() &&
      file_name(previous_object()) != PARTY_D && member_array(ch, syschans) == -1){
        return;
    }
    if(!Channels[ch] && file_name(previous_object()) != SERVICES_D){
        return;
    }
    if( emote ) {
        object *obs;
        object ob;
        string this_msg, tmp;

        if( target && (ob = find_player(convert_name(target))) ) {
            target = (string)ob->GetName();
        }

        //Colorize emote channels
        if (member_array(lower_case(ch),keys(tags)) >= 0) { //If there's an entry for the channel
            this_msg = tags[lower_case(ch)]; //Use it
        } else { //Otherwise
            this_msg = tags["default"];	//Use the default entry
        }

        //this_msg += "<" + ch + ">%^RESET%^ ";
        msg = replace_string(msg, "$N", who);
        if( target ) {
            msg = replace_string(msg, "$O", target);
            targmsg = replace_string(targmsg, "$N", who);
            targmsg = capitalize(replace_string(targmsg, "$O", "you"));
        }
        //tmp = this_msg + msg;

        //Put together the channel emote message
        //tmp = sprintf(emotelayout, this_msg, upper_case(ch), "%^RESET%^", msg);
        tmp = sprintf(emotelayout, this_msg, ch, "%^RESET%^", msg);

        //Store message in the history list
        eventAddLast(ch, tmp, pchan, msg);

        if(Channels[ch]){
            obs = filter(Channels[ch], (: $1 && !((int)$1->GetBlocked($(ch))) :));
            foreach(object listener in obs) {
                int ignore;
                if(sscanf(who,"%s@%s",suspect,site) < 2) {
                    suspect = who;
                    site = "@"+mud_name();
                }
                else site = "@"+site;
                if( listener == ob ) continue;
                if(sizeof(listener->GetMuffed()))
                    foreach(string jerk in listener->GetMuffed()){
                    if(jerk && lower_case(suspect) == lower_case(jerk)) ignore = 1;
                    if(jerk && lower_case(site) == lower_case(jerk)) ignore = 1;
                }
                if(listener->GetNoChanColors()) tmp = decolor(tmp);
                if(!ignore && CanListen(listener,ch) && !(listener->GetProperty("mute")))
                    listener->eventPrint(tmp, MSG_CHAN);
                ignore = 0;
            }
            if( member_array(ob, obs) != -1 ) {
                if( ob && !((int)ob->GetBlocked(ch)) ) {
                    int ignore;
                    //tmp = this_msg + targmsg;
                    //tmp = sprintf(emotelayout, this_msg, upper_case(ch), "%^RESET%^", targmsg);
                    tmp = sprintf(emotelayout, this_msg, ch, "%^RESET%^", targmsg);
                    if(sizeof(ob->GetMuffed()))
                        foreach(string jerk in ob->GetMuffed()){
                        if(jerk && lower_case(suspect) == lower_case(jerk)) ignore = 1;
                        if(jerk && lower_case(site) == lower_case(jerk)) ignore = 1;
                    }
                    if(ob->GetNoChanColors()) tmp = decolor(tmp);
                    if(!ignore && CanListen(ob,ch)&& !(ob->GetProperty("mute")))
                        ob->eventPrint(tmp, MSG_CHAN);
                    ignore = 0;
                }
            }
        }
        suspect = "";
        site = "";
    }
    else {
        object *obs;
        string chancolor;

        //Colorize flag
        if (member_array(lower_case(ch),keys(tags)) >= 0) { //If there's an entry for the channel
            chancolor = tags[lower_case(ch)]; //Use it
        } else { //Otherwise
            chancolor = tags["default"]; //Use the default entry
        }

        pmsg = msg;

        //Put together the channel emote message
        //msg = sprintf(chatlayout, who, chancolor, upper_case(ch), "%^RESET%^", pmsg);
        msg = sprintf(chatlayout, who, chancolor, ch, "%^RESET%^", pmsg);
        eventAddLast(ch, msg, pchan, pmsg, who);

        if(Channels[ch]) {
            obs = filter(Channels[ch], (: $1 && !((int)$1->GetBlocked($(ch))) :));
            foreach(object ob in obs){
                int ignore;
                if(sscanf(who,"%s@%s",suspect,site) < 2) {
                    suspect = who;
                    site = "@"+mud_name();
                }
                else site = "@"+site;

                if(sizeof(ob->GetMuffed()))
                    foreach(string jerk in ob->GetMuffed()){
                    if(jerk && lower_case(suspect) == lower_case(jerk)) ignore = 1;
                    if(jerk && lower_case(site) == lower_case(jerk)) ignore = 1;
                }
                if(ob->GetNoChanColors()) msg = decolor(msg);
                if(!ignore && CanListen(ob,ch)&& !(ob->GetProperty("mute")))
                    ob->eventPrint(msg, MSG_CHAN);

                ignore = 0;
                suspect ="";
                site = "";
            }
        }
    }
}

string *GetChannelList(string ch) {
    string *ret;
    object who;

    if( file_name(previous_object()) == SERVICES_D ) ch = GetLocalChannel(ch);
    else if( origin() != ORIGIN_LOCAL ) return ({});
    if( !Channels[ch] ) return ({});
    ret = ({});
    foreach(who in Channels[ch]) {
        if( !who || (int)who->GetInvis() || (int)who->GetBlocked(ch) )
            continue;
        ret += ({ (string)who->GetName() });
    }
    return ret;
}

string *GetLocalChannels(){
    return copy(local_chans);
}

string GetLocalChannel(string ch) {
    if (sizeof(localchans[ch])) return localchans[ch];
    else return ch;
}

string GetRemoteChannel(string ch) {
    if (sizeof(remotechans[ch])) return remotechans[ch];
    else return ch;
}

string *GetChannels() { return copy(keys(Channels)); }
string *GetSystemChannels() { return copy(syschans); }
