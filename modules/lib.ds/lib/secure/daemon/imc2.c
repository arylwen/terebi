// Tim Johnson
// Started on May 1, 2004.
// Use this however you want.

#include <lib.h>
#include <logs.h>
#include <cgi.h>
#include <save.h>
#include <network.h>
#include <socket_err.h>
#include <daemons.h>
#include <message_class.h>

// Connection data...
#define HOSTNAME "server01.intermud.us"
#define HOSTPORT 5000

//define HOSTNAME "server02.intermud.us"
//#define HOSTPORT 9000

// HostIP overrides HOSTNAME, in case the mud doesn't want to resolve addresses.
#define HOSTIP "209.190.9.170"

//#define HOSTIP "66.218.49.113"

// What name the network knows your mud as. Replace MUD_NAME with "whatever" if you want it to be different.
#define MUDNAME (imc2_mud_name())

// Passwords... make up a client and server password and **REGISTER IT ON INTERMUD.US**. 
//NOTE: **IMC2 mayn't work until you register your mud***
//#define IMC2_CLIENT_PW "clientpass"
//#define IMC2_SERVER_PW "serverpass"

// File to save data to, .o will be added automatically to the end.
// This will have private stuff in it, don't put this in a directory where your wizards can read it.
#define SAVE_FILE "/secure/save/imc2"

// COMMAND_NAME is the command that people type to use this network.
// **OUTDATED (not really)
#define COMMAND_NAME "imc2"

// NETWORK_ID is what your mud calls this network.
// This is the prefix that comes up on all messages to identify the IMC2 network.
// Tells for example look like:
// NETWORK_ID- Tim@TimMUD tells you: hi
// Make it similar to the command name, so players will understand.
// **OUTDATED
#define NETWORK_ID "IMC2"

// DATA_LOG is where packets are logged to.
// Turn IMC2_logging off when not working on the system, as it invades privacy.
// Comment this out to turn it off.
#undef IMC2_LOGGING

#ifndef LOG_IMC2
#define DATA_LOG "/secure/log/imc2"
#else 
#define DATA_LOG LOG_IMC2
#endif

// UNKNOWN_DATA_LOG is where unrecognized packets are logged to.
// I wrote handlers for all packets I know of, so this should only pick
// up tests and possibly if anyone is creating new packets.
#define UNKNOWN_DATA_LOG DATA_LOG ".unk"

// Your MUD's URL is shared with other muds when building the mud list.
// This you could also put this in your who reply.
#define URL "http://dead-souls.net"

// ANNOUNCE_LOG is where network announcements get logged to.
// I suggest you keep this turned on.
// These announcements seem to be about channels being created and
// deleted, but may possibly have more.
#define ANNOUNCE_LOG "IMC2_ANNOUNCEMENTS"

// How many lines you want the backlog to be.
#define BACKLOG_SIZE 20

// Minimum permission number for channel to be viewable on the web.
#define BACKLOG_WEB_LEVEL 0

// WHO_STR is the code that you want a who request to display.
#define WHO_STR CGI_WHO->gateway(1)+URL+"\ntelnet://alcatraz.wolfpaw.com:8000\n"+"______________________________________________________________________________"

// What's the file for the channel daemon?
#ifndef CHANNEL_BOT
#define CHANNEL_BOT CHAT_D
#endif
//#ifndef CHANNEL_BOTS
//#define CHANNEL_BOTS ([ "local" : CHAT_D ])
//#endif
#ifndef STREAM
#define STREAM 1
#endif
#ifndef EESUCCESS
#define EESUCCESS         1     /* Call was successful */
#endif
#ifndef THIS_PLAYER
#define THIS_PLAYER this_player()
#endif
#ifndef FIND_PLAYER
#define FIND_PLAYER(x) find_player(x)
#endif
#ifndef GET_CAP_NAME
#define GET_CAP_NAME(x) replace_string(x->GetName()," ","")
#endif
#ifndef GET_NAME
#define GET_NAME(x) convert_name(x->GetName())
#endif
#ifndef IMC2_MSG
#define IMC2_MSG(x,y) foreach(tmpstr in explode(x,"\n")){ message("IMC2",tmpstr,y); }
#endif
#ifndef GET_GENDER
#define GET_GENDER(x) x->GetGender()
#endif
#ifndef ADMIN
#define ADMIN(x) archp(x)
#endif
#ifndef VERSION
#define VERSION "Tim's LPC IMC2 client 30-Jan-05 / Dead Souls integrated"
#endif

#define HTML_LOCATION "http://dead-souls.net/"

// Other things that could be #define'd...
// INVIS(x) !visible(x)
// TELL_BOT "/u/t/timbot/imc2_invalidtells.c"
// TELL_BOTS ([ "timbot" : "/u/t/timbot/imc2_tellbot.c" ])
// CHAN_BOT "/u/t/timbot/imc2_chans.c"
// CHAN_BOTS ([ "ichat" : "/u/t/timbot/imc2_ichat.c" ])
// USER_EXISTS(x) user_exists(x)

// Mode decides what kind of packet is expected
#define NO_UIDS
#define MODE_CONNECTED 1
#define MODE_WAITING_ACCEPT 2
#define MODE_CONNECT_ERROR 3
// Not used yet, I need to see what the hub sends.
#define MODE_HUB_DOWN 4
// Not used yet either.

// Debugging
//#define DEB_IN  1
//#define DEB_OUT 2
//#define DEB_PAK 3
//#define DEB_OTHER 0

string tmpstr;

static int socket_num;
static int heart_count = 0;
int mode;
mapping ping_requests; // Keeps track of who sent a ping request.
// Ping requests aren't labelled with names, so replies are destined to this MUD
// with no idea why, unless we keep track.
string buf=""; // Buffer for incoming packets (aren't always sent 1 at a time)

// Variables
int xmit_to_network_room = 1; //enable this to make a lot of noise
string hub_name, network_name;
string server_pass, server_version;
mapping chaninfo;
mapping localchaninfo; // (["chan": ([ "perm":1, "name":"something", "users":({ }) ]) ])
mapping mudinfo;
mapping genders;
mapping tells;
int sequence;

// Prototypes :)
void create();
void Setup();
void remove();
string pinkfish_to_imc2(string str);
string imc2_to_pinkfish(string str);
string escape(string str);
string unescape(string str);
mapping string_to_mapping(string str);
string main_help();

private void send_packet(string sender, string packet_type, string target, string destination, string data);
private void send_text(string text);
private void got_packet(string info);
private void start_logon();
private varargs void send_is_alive(string origin);
private void channel_in(string fromname, string frommud, mapping data);
private void tell_in(string sender, string origin, string target, mapping data);
private void beep_in(string sender, string origin, string target, mapping data);
private void who_reply_in(string origin, string target, mapping data);
private void whois_in(string fromname, string frommud, string targ, mapping data);
private void whois_reply_in(string targ,string fromname,string frommud,mapping data);
private void ping_reply_in(string sender,string origin,string target,mapping data);
private void chanwho_reply_in(string origin, string target, mapping data);
private void send_keepalive_request();
private int chan_perm_allowed(object user, string chan);
private string localize_channel(string str);
private void chan_who_in(string fromname, string frommud, mapping data);
private void send_ice_refresh();
private void resolve_callback(string address, string resolved, int key);

//private varargs void debug(mixed msg, int x){
// Add stuff in here if you want to see messages.
//}

// Sanity check: a null socket write tends to be a crasher
varargs static void validate(int i){
    /*if(i){
        //yenta("Hit validate on fd "+i);
        if(!socket_status(i) || !socket_status(i)[5]){
            tn("%^RED%^BAD SOCKET ALERT. fd "+i+":  "+
              identify(socket_status(i)),"red");
            error("Bad socket, fd "+i);
        }
    }*/
}

varargs void write_to_log(mixed args...){
    if(xmit_to_network_room){
        tn(args[1]);
    }
    log_file(args...);
}

// Functions for users to change.
int can_use(object user){ return 1; } // Is this person allowed to use IMC2 at all?  This function determines if tells can be sent to the person and such.

int level(object ob){
    // Outgoing packets are marked with the user's level.
    // This function figures it out.
    // If you have different ways of ranking, make this function convert them to what IMC2 uses.
    // IMC2 uses: Admin=5, Imp=4, Imm=3, Mort=2, or None=1
    if(ADMIN(ob)) return 5; // Admin
    //TMI-2: if(wizardp(ob)) return 3;
    //TMI-2: if(userp(ob)) return 2;
    //Discworld: if(ob->query_creator()) return 3;
    if(this_player()) return 2;
    return 1; // None
}

string chan_perm_desc(int i){
    // Given the permission level assigned locally to a channel, return a short
    // string describing what the number means.  The number means nothing
    // outside of this MUD.  Also, they are independant of each other, and
    // so you can do groups without having to always do subgroups of higher
    // ones or anything like 'levels'.  BACKLOG_WEB_LEVEL is the only one of
    // significance, as it's the only one that the web backlog thing works on.
    switch(i){
    case 2: return "arch";
    case 1: return "creator";
    case 0: return "public";
    }
    return "invalid";
}

int chan_perm_allowed(object user, string chan){
    // Using the permission level assigned locally to a channel,
    // return 1 if user is allowed to use the channel, 0 if not.
    switch(localchaninfo[chan]["perm"]){
    case 2: if(archp(user)) return 1; return 0;
    case 1: if(creatorp(user)) return 1; return 0;
    case 0: return 1;
    }
}


// Shouldn't have to change anything beyond this point.

void write_callback(int fd){
#ifdef IMC2_LOGGING
    write_to_log(DATA_LOG,"Write_Callback. \n");
#endif
    start_logon();
}


void read_callback(int socket, mixed info){
    string a,b;
    int done=0;

#ifdef IMC2_LOGGING
    write_to_log(DATA_LOG,"SERVER: "+info+"\n");
#endif

    if(!sizeof(info)) return 0;
    //debug(save_variable(info),DEB_IN);
    buf += info;

    // The hub groups packets, unfortunately.
    switch(mode){
    case MODE_WAITING_ACCEPT: // waiting for Hub to send autosetup
        if(sscanf(info, "autosetup %s accept %s\n\r",
            hub_name, network_name)==2){
            //debug("Connected, hub is "+hub_name+", network is "+network_name);
            mode = MODE_CONNECTED;
            send_is_alive("*");
            send_keepalive_request();
            send_ice_refresh();
        } else if(sscanf(info, "PW %s %s version=%d %s\n",
            hub_name, server_pass, server_version, network_name)==4){
            mode = MODE_CONNECTED;
            send_is_alive("*");
            send_keepalive_request();
            send_ice_refresh();
        } else{ // Failed login sends plaintext error message.
            //debug("Failed to connect... "+info);
        }
        buf=""; // clear buffer
        break;
    case MODE_CONNECTED:
        while(!done){
            if(sscanf(buf,"%s\n\r%s",a,b)==2){ // found a break...
                got_packet(a);
                buf=b;
            } else { // no break...
                done = 1;
            }
        }
        break;
    }
    return;
}

private void got_packet(string info){
    string str;
    string a,b;
    int i;
    string sender, origin, route, packet_type, target, destination, strdata;
    int sequence;
    mapping data;
    object who;
    if(!sizeof(info)) return;
    //debug(save_variable(info),DEB_PAK);;
#ifdef IMC2_LOGGING
    write_to_log(DATA_LOG,"GOT PACKET: "+info+"\n");
#endif

    str = info;
    // messages end with " \n\r" or "\n" or sometimes just a space
    sscanf(str, "%s\n^", str);
    sscanf(str, "%s\r^", str);
    sscanf(str, "%s ^", str);
    sscanf(str, "%s ^", str);
    if(sscanf(str, "%s %d %s %s %s %s",
        a, sequence, route, packet_type,
        b, strdata)==6){ // matches
        if(sscanf(b,"%s@%s",target,destination)!=2){
            // Could be MUD instead of Name@MUD or *@MUD
            target="*"; destination=b;
        }
        if(sscanf(a,"%s@%s",sender,origin)!=2){
            sender="*"; origin=a;
        }
        data = string_to_mapping(strdata);
        //			debug("sender="+sender);
        //			debug("origin="+origin);
        //			debug("sequence="+sequence);
        //			debug("route="+route);
        //			debug("packet_type="+packet_type);
        //			debug("data="+save_variable(data));
        if(!mudinfo[origin]) mudinfo[origin] = ([ ]);

        switch(packet_type){
        case "is-alive": // For making a MUD list.
            if(!mudinfo[origin]) mudinfo[origin] = ([ ]);
            // example of info:
            // versionid=\"IMC2 AntiFreeze CL-2 SWR 1.0\" url=none md5=1
            //mudinfo[origin]["version"]="blah";
            if(!mudinfo[origin]["online"]){
                CHAT_D->eventSendChannel(origin+"@IMC2","muds","%^GREEN%^online%^RESET%^",0);
            }
            mudinfo[origin]+=data;
            mudinfo[origin]["online"]=1;
            //debug("handled is-alive for mud "+origin);
            break;
        case "close-notify": // Someone disconnected.
            if(!mudinfo[data["host"]]) mudinfo[data["host"]] = ([]);
            if(mudinfo[data["host"]]["online"]){
                CHAT_D->eventSendChannel(data["host"]+"@IMC2","muds","%^RED%^offline%^RESET%^",0);
            }
            mudinfo[data["host"]]["online"]=0;
            break;
        case "keepalive-request": // Request for is-alive.
            send_is_alive(origin);
            break;
        case "ice-msg-b": // Broadcast channel message.
            channel_in(sender, origin, data);
            break;
        case "tell": // Tells or emotes.
            tell_in(sender, origin, target, data);
            break;
        case "who-reply":
            who_reply_in(origin,target,data);
            break;
        case "whois": // Like I3's locate
            whois_in(sender,origin,target,data);
            break;
        case "whois-reply":
            whois_reply_in(target,sender,origin,data);
            break;
        case "beep":
            beep_in(sender, origin, target, data);
            break;
        case "ice-update": // Reply from ice-refresh.
            chaninfo[data["channel"]]=data;
            break;
        case "wHo": // Drop-through
        case "who":
            send_packet("*","who-reply",sender,origin,
              "text="+escape(pinkfish_to_imc2(WHO_STR)));
            CHAT_D->eventSendChannel("SYSTEM","intermud","[" + capitalize(sender)+"@"+origin+
              " requests the IMC2 who list]",0);
            break;
        case "ice-destroy": // Deleting channel.
            map_delete(chaninfo,data["channel"]);
            break;
        case "user-cache": // User info, like I3's ucache service.
            if(!genders[origin]) genders[origin]=([ ]);
            genders[origin][sender]=data["gender"];
            break;
        case "user-cache-reply": // Reply with user info
            if(!genders[origin]) genders[origin]=([ ]);
            genders[origin][data["user"]]=data["gender"];
            break;
        case "user-cache-request": // Request for user info
            sscanf(data["user"],"%s@%*s",str);
            who = FIND_PLAYER(lower_case(str));
            if(who
#ifdef INVIS
              && !INVIS(who)
#endif
            ){
                switch(GET_GENDER(who)){
                case "male" : i=0; break;
                case "female" : i=1; break;
                default : i=2; break;
                }
                send_packet("*","user-cache-reply",sender,
                  origin,sprintf("gender=%d",i));
            }
            break;
        case "ping":
            send_packet("*","ping-reply",sender,origin,
              sprintf("path=\"%s\"",route));
            break;
        case "ping-reply":
            ping_reply_in(sender,origin,target,data);
            break;
        case "ice-msg-r": // Relayed channel message.
            channel_in(sender, origin, data);
            CHAT_D->eventSendChannel("IMC2Test:"+sender,"admin","ice-msg-r sender:"+sender+"; origin:" +origin+"; data:"+(string)data);
            break;
        case "ice-chan-whoreply": // Tell who's listening to a channel.
            chanwho_reply_in(origin,target,data);
            break;
        case "emote": // Channel creation/destruction message... anything else?
            CHAT_D->eventSendChannel("IMC2:"+NETWORK_ID,"admin",data["text"]);
#ifdef ANNOUNCE_LOG
            write_to_log(ANNOUNCE_LOG,ctime(time())+": "+data["text"]+"\n");
#endif
            break;
        case "ice-chan-who": // Check who's listening to a channel.
            chan_who_in(sender,origin,data);
            break;
        case "channel-notify":
            // Don't care about this.  Useful only if you care when
            // people on other MUDs start/stop listening to a channel.
            // example: Someone@SomeMUD 1087076772 SomeMUD channel-notify *@*  channel=Hub01:ichat status=0
            break;
            // The following packets shouldn't be incoming.
        case "ice-cmd": // Remote channel administration
        case "remote-admin": // For controlling the hub
        case "ice-refresh": // Request data about channels
        case "ice-msg-p": // Private channel message
            //debug("This packet isn't supposed to be incoming: "+packet_type);
            break;
        default:
#ifdef UNKNOWN_DATA_LOG
            write_to_log(UNKNOWN_DATA_LOG,"Unknown packet: "+escape(info)+"\n\n");
#endif
            //debug("Unlisted packet type: "+packet_type);
            break;
        }
    }
    else{
        buf += info;
        //debug("Doesn't match incoming pattern, so putting on buffer: "+str);
#ifdef BAD_PACKET
        write_to_log(BAD_PACKET,"Doesn't match incoming pattern: "+str+"\n");
#endif
    }
}

private int close_callback(object socket){
    // Connection was closed.
#ifdef IMC2_LOGGING
    write_to_log(DATA_LOG,"DISCONNECTED\n");
#endif
    socket_close(socket_num);
    //	socket->remove();
    create();
    return 1;
}

private void send_text(string text){
    // Send a literal string.
    // Almost everything should use the send_packet function instead of this.
#ifdef IMC2_LOGGING
    write_to_log(DATA_LOG,"CLIENT: "+save_variable(text)+"\n");
#endif
    //debug(save_variable(text), DEB_OUT);
    //	debug("writing to socket: "+socket_num);
    validate(socket_num);
    socket_write(socket_num,text);
    //	imc2_socket->send(text);
    return;
}

void create(){
    set_heart_beat(1);
    tn("IMC2: created "+ctime(time()));
    call_out( (: Setup :), 1);
}

void Setup(){
    int temp;
#ifdef DISABLE_IMC2
    if(DISABLE_IMC2){
        call_out( (: remove() :), 1);
        return;
    }
#endif

    if(DISABLE_INTERMUD == 1){
        call_out( (: remove() :), 1);
        return;
    }

#ifndef NO_UIDS
    seteuid(getuid());
#endif
    tn("Creating IMC2 object at "+ctime(time())+".\n");
#ifdef IMC2_LOGGING
    write_to_log(DATA_LOG,"Creating IMC2 object at "+ctime(time())+".\n");
#endif
    if(sizeof(get_dir(SAVE_FILE+".o"))) restore_object(SAVE_FILE);
    if(!mudinfo) mudinfo = ([ ]);
    if(!chaninfo) chaninfo = ([ ]);
    if(!localchaninfo) localchaninfo = ([ ]);
    if(!genders) genders = ([ ]);
    if(!tells) tells=([ ]);
    ping_requests=([ ]);
    mode = MODE_WAITING_ACCEPT;
#ifdef HOSTIP
    // We already know the IP, go straight to the connecting, just do callback as if it found the IP.
    resolve_callback(HOSTIP,HOSTIP,1);
#else
    temp = resolve(HOSTNAME, "resolve_callback");
    if(temp == 0){
        //debug("Addr_server is not running, resolve failed.");
#ifdef IMC2_LOGGING
        write_to_log(DATA_LOG,"Addr_server is not running, resolve failed.\n");
#endif
        remove();
        return;
    }
#endif
}

void heart_beat(){
    heart_count++;
    if(heart_count > 30){
        mixed sstat = socket_status(socket_num);
        heart_count = 0;
        if(!sstat || sstat[1] != "DATA_XFER"){
            socket_close(socket_num);
            //tn("reloading");
            RELOAD_D->eventReload(this_object(), 2, 1);
        }
    }
}

void remove(){
    // This object is getting destructed.
    socket_close(socket_num);
    mode=2;
    //	if(imc2_socket) imc2_socket->remove();
#ifdef IMC2_LOGGING
    write_to_log(DATA_LOG,"IMC2 OBJECT REMOVED\n");
#endif
    save_object(SAVE_FILE);
    destruct(this_object());
}

void resolve_callback( string address, string resolved, int key ) {
    // Figured out what the IP is for the address.
    int error;
    //debug("Resolved to: "+resolved);

    write_file("/tmp/imc2.log","address: "+address+"\n");
    write_file("/tmp/imc2.log","resolved: "+resolved+"\n");
    write_file("/tmp/imc2.log","key: "+key+"\n");
    //tn("IMC2: About to try to create a socket.");

    //socket_num = socket_create(STREAM, "close_callback");
    socket_num = socket_create(STREAM, "read_callback", "close_callback");
    if (socket_num < 0) {
#ifdef IMC2_LOGGING
        write_to_log(DATA_LOG,"socket_create: " + socket_error(socket_num) + "\n");
#endif
        //tn("IMC2: socket_create: " + socket_error(socket_num) + "\n");
        //tn("IMC2: stack: " + get_stack());
        return;
    }
#ifdef IMC2_LOGGING
    write_to_log(DATA_LOG,"socket_create: Created Socket " + socket_num + "\n");
#endif

    error = socket_connect(socket_num, resolved+" "+HOSTPORT, "read_callback", "write_callback");
    if (error != EESUCCESS) {
#ifdef IMC2_LOGGING
        write_to_log(DATA_LOG,"socket_connect: " + socket_error(error) + "\n");
#endif
        //debug("socket_connect, error="+error+": " + socket_error(error) + "\n");
        socket_close(socket_num);
        return;
    }
#ifdef IMC2_LOGGING
    write_to_log(DATA_LOG,"socket_connect: connected socket " + socket_num + " to " + resolved+" "+HOSTPORT + "\n");
#endif

    //write_callback(socket_num);
}

void start_logon(){
    //debug("Gonna try logging in, sending the PW thing...");
#if NOT_AUTO
    send_text(sprintf("PW %s %s version=%d\n",
#else
        send_text(sprintf("PW %s %s version=%d autosetup %s\n",
#endif
            MUDNAME,
            IMC2_CLIENT_PW, 2
#ifndef NOT_AUTO
            , IMC2_SERVER_PW
#endif
          ));
        buf="";
        /* For invitation-only networks.
                send_text(sprintf("PW %s %s version=%d %s\n",
                        MUDNAME,
                        IMC2_CLIENT_PW, 2, "hub03"
                ));
        */
        sequence=time();
    }

      string escape(string str){
          str=replace_string(str,"\\","\\\\");
          str=replace_string(str,"\"","\\\"");
          str=replace_string(str,"\n","\\n");
          str=replace_string(str,"\r","\\r");
          if(sizeof(explode(str," "))!=1) str = "\""+str+"\"";
          return str;
      }

      string unescape(string str){
          string a,b=str,output="";
          while(sscanf(b,"%s\\%s",a,b)==2){
              output += a;
              if(sizeof(b)){
                  switch(b[0]){
                  case 34 : output += "\""; break; // '\"' makes warnings.
                  case '\\' : output += "\\"; break;
                  case 'n' : output += "\n"; break;
                  case 'r' : output += "\r"; break;
                  }
              }
              b=b[1..];
          }
          output += b;
          output = replace_string(output,"\n\r","\n");
          if((sizeof(explode(output," "))==1) && sscanf(output,"\\\"%*s\\\"") ) output = "\""+output+"\"";
          return output;
      }

      mapping string_to_mapping(string str){
          // Picks first element off of string and then repeats?
          mapping out=([]);
          int i;
          string what,data,rest;

          rest = str;

          while(sizeof(rest)>0){
              sscanf(rest, "%s=%s", what, rest);
              /*
                              write("what="+what+", rest="+rest+"\n");
              */
              // At this point, what is the key, rest is value plus rest.
              if(rest[0]==34){ // value is in quotes, tons of fun!
                  // find first quote without a backslash in front?
                  /*
                                          write("rest begings with a quote\n");
                  */
                  i = 1;
                  while(((rest[i]!=34) || (rest[i-1]==92)) && (i<sizeof(rest))){ // 34 = ", 92 = \
                      // While this is not a quote, or if this is an escaped quote, keep looking.
                      i++;
                  }
                  // now are 1 space past quote
                  //			write("i="+i+"\n");
                  data=rest[1..(i-1)]; // skip opening and closing quotes
                  rest=rest[(i+2)..]; // skip past space
                  //			write("new data="+data+"\n");
                  //			write("new rest="+rest+"\n");
                  // Data is now what was in the quotes... now to un-escape the data...
                  out[what]=unescape(data);
              }
              else{ // value is not in quotes, tons of actual non-sarcastic fun!
                  // just split it at the first space
                  if(sscanf(rest,"%s %s",data,rest)!=2){ // break at first space
                      data = rest;
                      rest = "";
                  }
                  if((sscanf(data,"%d",i)==1) && (sprintf("%d",i)==data)) // is just number
                      out[what]=i;
                  else // not just a number
                      out[what]=data;
              }
          }
          return out;
      }

      void send_packet(string sender, string packet_type, string target, string destination, string data){
          // Sends a packet in the format that IMC2 uses.
          sequence++;
          send_text(sprintf("%s@%s %d %s %s %s@%s %s\n",
              sender,	MUDNAME, sequence, MUDNAME, packet_type,
              target, destination, data
            ));
      }

      void send_keepalive_request(){
          // Ask all the muds to tell us that they're alive.
          string mud;
          // Mark all the muds as offline until they respond.
          foreach(mud in keys(mudinfo)){
              mudinfo[mud]["online"]=0;
          }
          send_packet("*","keepalive-request","*","*","");
      }

      varargs void send_is_alive(string origin){
          // Sends an is-alive packet to whoever requested it, or else broadcasts it.
          send_packet("*","is-alive","*",(origin ? origin : "*"),
            sprintf("versionid=\"%s\" networkname=%s url=%s",
              VERSION, network_name, URL));
      }

      void channel_in(string fromname, string frommud, mapping data){
          string sender;
          string localchan;

          int emote=0;

          sender=fromname+"@"+frommud;
          if(data["sender"]) sender = data["sender"];
          if(data["realfrom"]) sender = data["realfrom"];
          if(intp(data["text"])) data["text"]=sprintf("%d",data["text"]);

          if(data["emote"]) emote = data["emote"];
          //Following fix courtesy of Tricky
          if (emote == 1 && strsrch(data["text"], "$N") == -1) 
              data["text"] = "$N " + data["text"];

          localchan = CHANNEL_BOT->GetLocalChannel(data["channel"]);
          CHANNEL_BOT->eventSendChannel(sender, localchan, data["text"], emote, "", "");
      }

      void channel_out(string user,string chan,string msg,int emote){
          // Send outgoing channel message.
          send_packet(user,"ice-msg-b","*","*", sprintf("channel=%s text=%s emote=%d echo=0", chan,escape(pinkfish_to_imc2(msg)),emote));
      }

      varargs static void tell_out(object from, string targname, string targmud, string msg, int reply, int emote){
          // Send outgoing tell.
          if(!reply) reply=0;
          send_packet(capitalize(from->GetKeyName()),"tell",targname,targmud,
            sprintf("level=%d text=%s reply=%d emote=%d", level(from),escape(msg),reply,emote));
          from->eventPrint("%^BOLD%^RED%^You tell " + capitalize(targname) + "@" + targmud + ":%^RESET%^ " + msg, MSG_CONV);
      }

      void tell_in(string sender, string origin, string target, mapping data){
          // Incoming tell. Parse and display to user
          object who;
          int sz;
          string blmsg;
          who=FIND_PLAYER(lower_case(target));
          target=GET_CAP_NAME(who);
          data["text"]=imc2_to_pinkfish(data["text"]);
          who->SetProperty("reply",sender+"@"+origin);
          if(who
#ifdef INVIS
            && VISIBLE(who)
#endif
            && can_use(who)
          ){
              switch(data["isreply"]){
              case 2: // emote
                  who->eventPrint("%^BOLD%^RED%^" + sender + "@" + origin + " %^RESET%^ " + data["text"], MSG_CONV);
                  //IMC2_MSG(sprintf("%%^CYAN%%^%s@%s%%^RESET%%^ %s\n", NETWORK_ID,sender,origin,data["text"]), who);
                  blmsg=sprintf("%s@%s %s\n",sender,origin,data["text"]);
                  break;
              case 1: // reply
                  who->eventPrint("%^BOLD%^RED%^" + sender + "@" + origin + " tells you:%^RESET%^ " + data["text"], MSG_CONV);
                  //IMC2_MSG(sprintf("%s %%^CYAN%%^%s@%s%%^RESET%%^ replies to you: %s\n", NETWORK_ID,sender,origin,data["text"]), who);
                  blmsg=sprintf("%s@%s replied to you: %s\n", NETWORK_ID,sender,origin,data["text"]);
                  break;
              default:
                  who->eventPrint("%^BOLD%^RED%^" + sender + "@" + origin + " tells you:%^RESET%^ " + data["text"], MSG_CONV);
                  //IMC2_MSG(sprintf("%s %%^CYAN%%^%s@%s%%^RESET%%^ tells you: %s\n", NETWORK_ID,sender,origin,data["text"]), who);
                  blmsg=sprintf("%s@%s told you: %s\n", sender,origin,data["text"]);
                  break;
              }
              if(!tells[target])
                  tells[target]=([ ]);
              tells[target]["reply"] = sprintf("%s@%s",sender,origin);
              if(!tells[target]["backlog"] || !arrayp(tells[target]["backlog"]))
                  tells[target]["backlog"]=({ });
              tells[target]["backlog"] += ({ blmsg });
              sz = sizeof(tells[target]["backlog"]);
              if(sz>BACKLOG_SIZE)
                  tells[target]["backlog"]=tells[target]["backlog"][(sz-BACKLOG_SIZE)..sz];
          } else {
#ifdef TELL_BOTS
              // Can have a mapping of bots which can get tells.
              if(TELL_BOTS[target]){
                  call_other(TELL_BOTS[target],"got_tell",sender, origin, target, data["text"]);
                  return;
              }
#endif
#ifdef TELL_BOT
              // They should have got_tell(fromname, frommud, target, text)
              // I'll assume bots don't care about emote/reply... tell me if you do?
              // TELL_BOT will get told of all tells which don't have a valid target, and
              // then the sender will be notified that it didn't get to anyone.
              // This would be useful if for example you wanted all tells sent to
              // offline people to be mudmailed to them or something.
              call_other(TELL_BOT,"got_tell",sender, origin, target, data["text"]);
#endif
              send_packet("*","tell",sender,origin,
                sprintf("level=-1 text=\"%s is not online on this mud.\" isreply=1",target));
          }
      }

      int tell(mixed arg, object who){
          string targmud,targplayer,msg;
          int i;
          if(!who || !this_player() || who != this_player()) return 0;
          if(who->GetForced()){
              return 0;
          }
          i = sscanf(arg,"%s@%s %s",targplayer, targmud, msg);
          if(i != 3 ){
              write("There was an error in your message. See: \"help imc2\"");
              return 1;
          }
          tell_out(who, targplayer, targmud, msg, 0, 0);
          return 1;
      } 

      string pinkfish_to_imc2(string str){
          // Foreground
          str=replace_string(str,"%^BLACK%^","~x"); // Black
          str=replace_string(str,"%^RED%^","~R"); // Red
          str=replace_string(str,"%^GREEN%^","~G"); // Green
          str=replace_string(str,"%^BLUE%^","~B"); // Blue
          str=replace_string(str,"%^WHITE%^","~W"); // White
          str=replace_string(str,"%^ORANGE%^","~y"); // Orange
          str=replace_string(str,"%^CYAN%^","~c"); // Cyan
          str=replace_string(str,"%^YELLOW%^","~Y"); // Yellow
          str=replace_string(str,"%^MAGENTA%^","~p"); // Magenta -> purple sounds closest
          str=replace_string(str,"%^GRAY%^","~w"); // Gray doesn't display on my MUD, bah :(
          // Background
          str=replace_string(str,"%^B_BLACK%^","^x");
          str=replace_string(str,"%^B_RED%^","^R"); // Red
          str=replace_string(str,"%^B_GREEN%^","^G"); // Green
          str=replace_string(str,"%^B_BLUE%^","^b"); // Blue
          str=replace_string(str,"%^B_WHITE%^","^W"); // White
          str=replace_string(str,"%^B_ORANGE%^","^O"); // Orange
          str=replace_string(str,"%^B_CYAN%^","^c"); // Cyan
          str=replace_string(str,"%^B_YELLOW%^","^Y"); // Yellow
          str=replace_string(str,"%^B_MAGENTA%^","^p"); // Magenta -> purple sounds closest
          // Misc.
          str=replace_string(str,"%^FLASH%^","~$"); // Flash -> Blink
          str=replace_string(str,"%^BOLD%^","~L"); // Bold
          str=replace_string(str,"%^RESET%^","~!");
          //Replace anything that was done in %^BOLD%^RED%^ format and wasn't already caught
          str=replace_string(str,"%^FLASH","~$"); // Flash -> Blink
          str=replace_string(str,"%^BOLD","~L"); // Bold
          str=replace_string(str,"%^RESET","~!");
          str=replace_string(str,"FLASH%^","~$"); // Flash -> Blink
          str=replace_string(str,"BOLD%^","~L"); // Bold
          str=replace_string(str,"RESET%^","~!");
          return str;
      }

      string imc2_to_pinkfish(string str){
          string output="";
          int sz;
          /*
          For colors explanation, refer to IMC Packet Documentation by Xorith.
          Thanks very much for putting that out, by the way. :)
          Found at http://hub00.muddomain.com/imc2_protocol_doc.txt
          */
          sz=sizeof(str)-1;
          while(sizeof(str)>1){
              switch(str[0]){
              case '~': // Foreground
                  switch(str[1]){
                  case 'Z': break; // Random
                  case 'x': output += "%^BLACK%^"; break; // Black
                  case 'r': output += "%^RED%^"; break; // Dark Red
                  case 'g': output += "%^GREEN%^"; break; // Dark Green
                  case 'y': output += "%^ORANGE%^"; break; // Orange
                  case 'b': output += "%^BLUE%^"; break; // Dark Blue
                  case 'p': output += "%^MAGENTA%^"; break; // Purple
                  case 'c': output += "%^CYAN%^"; break; // Cyan
                  case 'w': output += "%^WHITE%^"; break; // Grey
                  case 'D': output += "%^BLACK%^"; break; // Dark Grey
                  case 'z': output += "%^BLACK%^"; break; // Same as ~D
                  case 'R': output += "%^RED%^"; break; // Red
                  case 'G': output += "%^GREEN%^"; break; // Green
                  case 'Y': output += "%^YELLOW%^"; break; // Yellow
                  case 'B': output += "%^BLUE%^"; break; // Blue
                  case 'P': output += "%^MAGENTA%^"; break; // Pink
                  case 'C': output += "%^BLUE%^"; break; // Light Blue
                  case 'W': output += "%^WHITE%^"; break; // White

                  case 'm': output += "%^MAGENTA%^"; break; // same as p
                  case 'd': output += "%^WHITE%^"; break; // same as w
                  case 'M': output += "%^MAGENTA%^"; break; // same as P
                      // Misc.
                  case '!': output += "%^RESET%^"; break; // Reset
                  case 'L': output += "%^BOLD%^"; break; // Bold
                  case 'u': break; // Underline
                  case '$': output += "%^FLASH%^"; break; // Blink
                  case 'i': break; // Italic
                  case 'v': break; // Reverse
                  case 's': break; // Strike-thru

                  case '~': output += "~"; break; // ~~ prints as ~
                  default : output += "~"; // Don't skip over this
                      // (cheap hack is to add a character in front so the [2..] thing still works)
                      str = " "+str;
                      break;
                  }
                  str=str[2..];
                  break;
              case '^':  // Background
                  switch(str[1]){
                  case 'Z': break; // Random
                  case 'x': output += "%^B_BLACK%^"; break; // Black
                  case 'r': output += "%^B_RED%^"; break; // Dark Red
                  case 'g': output += "%^B_GREEN%^"; break; // Dark Green
                  case 'O': output += "%^B_ORANGE%^"; break; // Orange
                  case 'B': output += "%^B_BLUE%^"; break; // Dark Blue
                  case 'p': output += "%^B_MAGENTA%^"; break; // Purple
                  case 'c': output += "%^B_CYAN%^"; break; // Cyan
                  case 'w': output += "%^B_WHITE%^"; break; // Grey
                  case 'z': output += "%^B_BLACK%^"; break; // Dark Grey
                  case 'R': output += "%^B_RED%^"; break; // Red
                  case 'G': output += "%^B_GREEN%^"; break; // Green
                  case 'Y': output += "%^B_YELLOW%^"; break; // Yellow
                  case 'b': output += "%^B_BLUE%^"; break; // Blue
                  case 'P': output += "%^B_MAGENTA%^"; break; // Pink
                  case 'C': output += "%^B_BLUE%^"; break; // Light Blue
                  case 'W': output += "%^B_WHITE%^"; break; // White
                  case '^': output += "^"; break; // ^^ prints as ^
                  default : output += "^"; // Don't skip over this
                      // (cheap hack is to add a character in front so the [2..] thing still works)
                      str = " "+str;
                      break;
                  }
                  str=str[2..];
                  break;
              case '`': // Blinking Foreground
                  switch(str[1]){
                  case 'Z': output += "%^FLASH%^"; break; // Random
                  case 'x': output += "%^FLASH%^%^BLACK%^"; break; // Black
                  case 'r': output += "%^FLASH%^%^RED%^"; break; // Dark Red
                  case 'g': output += "%^FLASH%^%^GREEN%^"; break; // Dark Green
                  case 'O': output += "%^FLASH%^%^ORANGE%^"; break; // Orange
                  case 'b': output += "%^FLASH%^%^BLUE%^"; break; // Dark Blue
                  case 'p': output += "%^FLASH%^%^MAGENTA%^"; break; // Purple
                  case 'c': output += "%^FLASH%^%^CYAN%^"; break; // Cyan
                  case 'w': output += "%^FLASH%^%^WHITE%^"; break; // Grey
                  case 'z': output += "%^FLASH%^%^BLACK%^"; break; // Dark Grey
                  case 'R': output += "%^FLASH%^%^RED%^"; break; // Red
                  case 'G': output += "%^FLASH%^%^GREEN%^"; break; // Green
                  case 'Y': output += "%^FLASH%^%^YELLOW%^"; break; // Yellow
                  case 'B': output += "%^FLASH%^%^BLUE%^"; break; // Blue
                  case 'P': output += "%^FLASH%^%^MAGENTA%^"; break; // Pink
                  case 'C': output += "%^FLASH%^%^BLUE%^"; break; // Light Blue
                  case 'W': output += "%^FLASH%^%^WHITE%^"; break; // White
                  case '`': output += "`"; break; // `` prints as `
                  default : output += "`"; // Don't skip over this
                      // (cheap hack is to add a character in front so the [2..] thing still works)
                      str = " "+str;
                      break;
                  }
                  str=str[2..];
                  break;
              default:
                  output += str[0..0];
                  str=str[1..];
                  break;
              }
          }
          output += str;
          return output;
      }

      private void who_reply_in(string origin, string target, mapping data){
          string output;
          object targuser;
          targuser = FIND_PLAYER(lower_case(target));
          if(targuser){
              output = NETWORK_ID+" who reply from: %^CYAN%^"+origin+"%^RESET%^\n";
              output += imc2_to_pinkfish(data["text"])+"\n";
              IMC2_MSG(output,targuser);
          }
      }

      private void whois_in(string fromname, string frommud, string targ, mapping data){
          if(FIND_PLAYER(lower_case(targ))
#ifdef INVIS
            && !INVIS(FIND_PLAYER(lower_case(targ)))
#endif
            && can_use(FIND_PLAYER(lower_case(targ)))
          ){
              send_packet(targ,"whois-reply",fromname,frommud,"text=Online");
          }
#ifdef USER_EXISTS
          else if(USER_EXISTS(lower_case(targ))){
              send_packet(targ,"whois-reply",fromname,frommud,
                "text=\"Exists but is offline\"");
          }
#endif
      }

      private void whois_reply_in(string targ,string fromname,string frommud,mapping data){
          object who;
          who=FIND_PLAYER(lower_case(targ));
          if(who){
              IMC2_MSG(sprintf("%s whois reply: %s@%s: %s\n",
                  NETWORK_ID,fromname,frommud,data["text"]),who);
          }
      }

      private void beep_in(string sender, string origin, string target, mapping data){
          object who;
          who=FIND_PLAYER(lower_case(target));
          if(who && can_use(who)
#ifdef INVIS
            && VISIBLE(who)
#endif
          ){
              IMC2_MSG(sprintf("%s- %%^CYAN%%^%s@%s%%^RESET%%^ \abeeps you.\n",
                  NETWORK_ID,sender,origin,data["text"]), who);
          }
          else{
              send_packet("*","tell",sender,origin,
                sprintf("level=-1 text=\"%s is not online.\" isreply=1",target));
          }
      }

      void beep_out(object from, string targname, string targmud){
          send_packet(GET_CAP_NAME(from),"beep",targname,targmud,
            sprintf("level=%d ", level(from)));
      }

      void send_ice_refresh(){ send_packet("*","ice-refresh","*","*",""); }

      void ping_reply_in(string sender,string origin,string target,mapping data){
          object who;
          if((target=="*") ){ 
		      target=ping_requests[sender];
			  if(target){
                  target=ping_requests[sender];
                  map_delete(ping_requests,sender);
			  }
          }
          who=FIND_PLAYER(lower_case(target));
          if(who){
              IMC2_MSG(sprintf("%s route to %%^CYAN%%^%s%%^RESET%%^ is: %s\n",
                  NETWORK_ID,origin,data["path"]), who);
          }
      }

      void ping_out(string from,string targmud){ send_packet(from,"ping","*",targmud,""); }
      varargs void who_out(string from,string targmud,string type){ send_packet(from,"who","*",targmud,(type ? sprintf("type=\"%s\"",type) : "type=who")); }

      void chanwho_out(object from,string chan,string mud){
          send_packet(GET_CAP_NAME(from),"ice-chan-who","*",mud,
            sprintf("level=%d channel=%s lname=%s",level(from),
              localchaninfo[chan]["name"],chan));
      }

      void chanwho_reply_in(string origin, string target, mapping data){
          string output;
          object targuser;
          targuser = FIND_PLAYER(lower_case(target));
          if(targuser){
              output = NETWORK_ID+" chanwho reply from "+origin+" for "+data["channel"]+"\n";
              output += imc2_to_pinkfish(data["list"]);
              IMC2_MSG(output,targuser);
          }
      }

      void chan_who_in(string fromname, string frommud, mapping data){
          /*	  // Handles an incoming channel who request.
                    object user, *usrs=({ });
                    string local, lname;
                    string output;
                    local=localize_channel(data["channel"]);
                    if(data["lname"]) lname = data["lname"];
                    else lname = local;
                    if(!local){ // Channel not used locally.
                        output = sprintf("channel=%s list=\"%s (%s) is not configured on this MUD.\n\"",
                          data["channel"],lname,data["channel"]);
                    }
                    else{ // Is used locally
                        output = "The following users are listening to "+lname+" ("+local+"):\n  ";
                        foreach(user in users()){
                            if(chan_listening(user,local)
#ifdef INVIS
                              && !INVIS(user)
#endif
                            ){
                                usrs += ({ GET_CAP_NAME(user) });
                            }
                        } // foreach
                        output += implode(usrs,", ");
                        output += "\n";
                        if(!sizeof(usrs)){
                            output=sprintf("channel=%s list=\"Nobody is listening to %s (%s) on this MUD.\n\"",
                              lname,local);
                        }
                    }
                    send_packet("*","ice-chan-whoreply",fromname,frommud,output);*/
      }

      string find_mud(string str){
          // Makes case-insensitive mud name into the actual mud name, or else 0.
          string mud;
          if (!str) return 0;

          str = lower_case(str);
          foreach(mud in keys(mudinfo)){
              if ( lower_case(mud)==str ) return mud;
          }
          return 0;
      }

      string localize_channel(string str){
          // Tells what the local name for a channel is, given the network name for it, or else 0.
          string a;
          foreach(a in keys(localchaninfo)){
              if(lower_case(localchaninfo[a]["name"])==lower_case(str)) return a;
          }
          return 0;
      }

      //Returns mud list to user object 'towho'
      void mudlist(object towho) {
          int x,y;
          string output;
          string mud,*muds;

          muds = sort_array(keys(mudinfo),1);
          if (!mode==MODE_CONNECTED) {
              message("system",MUDNAME+" is not connected to the "+NETWORK_ID+" network!\n",towho);
              return;
          } else if (!sizeof(mudinfo)){
              message("system","There are no muds on the "+NETWORK_ID+" network!\n",towho);
              return;
          } else {
              x=0; y=0;
              output=sprintf("[%s] %-20s %-20s %-20s\n","U/D?","Name","Network","IMC2 Version");
              foreach (mud in muds){
                  if(!mudinfo[mud]) output += "Error on mud: "+mud+"\n";
                  else {
                      if(mudinfo[mud]["online"]) x++; else y++;
                      output += sprintf("[%s] %-20s %-20s %-20s\n",
                        (mudinfo[mud]["online"] ? "%^GREEN%^ UP %^RESET%^" : "%^RED%^DOWN%^RESET%^"),
                        mud, mudinfo[mud]["networkname"], mudinfo[mud]["versionid"]);
                  }
              }
              output += sprintf("%d of %d MUDs are online.\n",x,x+y);
              message("system",output,towho);
              return;
          }
      }

      mapping getmudinfo() {
          return mudinfo;
      }

      //pings mud 'mudname', returns it to user object 'towho'
      void mudinfo(string args,object towho) {
          string str;
          string output;

          if(!args) {
              message("system","See info for which MUD?",towho);
              return;
          }

          str=find_mud(args);
          if(!str) {
              message("system","MUD isn't known on "+NETWORK_ID+".",towho);
              return;
          }

          output=("Mud info for: "+str+"\nStatus: ");

          if(mudinfo[str]["online"]) output+=("%^GREEN%^Online%^RESET%^\n");
          else output+=("%^RED%^Offline%^RESET%^\n");

          if(mudinfo[str]["versionid"])   output+=("Version ID: "+mudinfo[str]["versionid"]+"\n");
          if(mudinfo[str]["url"]) 	    output+=("URL: "+mudinfo[str]["url"]+"\n");
          if(mudinfo[str]["networkname"]) output+=("Network name: "+mudinfo[str]["networkname"]+"\n");

          message("system",output,towho);
          return;
      }

      //Gets WHO list from mud 'mudname', returns it to user object 'towho'
      void mudwho(string mudname,object towho) {
          string str;
          if(!mudname) {
              message("system","Send who request to which MUD?",towho);
              return;
          }
          str=find_mud(mudname);
          if(!str) {
              message("system","MUD isn't known on "+NETWORK_ID+".",towho);
              return;
          }
          if(!mudinfo[str]["online"]) {
              message("system",str+" is offline right now.",towho);
              return;
          }
          who_out(capitalize(this_player()->GetKeyName()),str);
          message("system",NETWORK_ID+" sent a who request to "+str+"\n",towho);
          return;
      }

      //pings mud 'mudname', returns it to user object 'towho'
      mixed pingmud(string mudname,object towho) {
          string str;
          if(!mudname) {
              message("system","Send ping to which MUD?",towho);
              return;
          }
          str=find_mud(mudname);
          if(!str) {
              message("system","MUD isn't known on "+NETWORK_ID+".",towho);
              return;
          }
          if(!mudinfo[str]["online"]) {
              message("system",str+" is offline right now.",towho);
              return;
          }
          ping_out(capitalize(towho->GetKeyName()),str);
          message("system","Sent a ping to "+str+".",towho);
          return;
      }

      //Displays status to user object 'towho'
      void getstatus(object towho) {
          string output;

          output=sprintf(@EndText
IMC2 NETWORK INFORMATION
------------------------
Status: %s
Hub address: %s
Hub port: %d
The hub calls itself: %s
The network calls itself: %s
Command to use this network: %s
The MUD calls this connection: %s

Packet logging: %s

The network calls the MUD: %s
The MUD's Version ID: %s
The MUD's URL: %s
EndText,
            ((mode==MODE_CONNECTED) ? "Connected" : "Not connected"),
#ifdef HOSTNAME
            HOSTNAME,
#else
            HOSTIP,
#endif
            HOSTPORT,hub_name,network_name,COMMAND_NAME,NETWORK_ID,
#ifdef IMC2_LOGGING
            "on",
#else
            "off",
#endif
            MUDNAME,VERSION,URL);

          message("system",output,towho);
      }

      //return mode
      int getonline() {
          return mode;
      }

      //Beeps user@mud stored in 'args', returns it to user object 'towho'
      void interbeep(string args, object towho) {
          string a,b;
          if(!args || sscanf(args,"%s@%s",a,b)!=2) {
              message("system","Invalid syntax.",towho);
              return;
          }
          b=find_mud(b);
          if(!b) {
              message("system","MUD isn't known on "+NETWORK_ID+".",towho);
              return;
          }
          if(!mudinfo[b]["online"]) {
              message("system",b+" is offline right now.",towho);
              return;
          }
          beep_out(towho,a,b);
          message("system",sprintf("You beep %s@%s.",capitalize(a),b),towho);
          return;
      }

      //Fingers user@mud stored in 'args', returns it to user object 'towho'
      void finger(string args, object towho) {
          string a,b;

          if(!args) return notify_fail("Send finger request to who@where?\n");;
          if(sscanf(args,"%s@%s",a,b)!=2){
              return notify_fail("Send finger request to who@where?\n");;
          }
          b=find_mud(b);
          who_out(GET_CAP_NAME(towho),b,"finger "+a);
          IMC2_MSG(NETWORK_ID"- Sent a finger request to "+a+"@"+b+"\n",THIS_PLAYER);
      }

      //Gets whois for user@mud stored in 'args', returns it to user object 'towho'
      void whois(string args, object towho) {
          if(!args || !sizeof(args)) return notify_fail("Locate who?\n");

          send_packet(GET_CAP_NAME(THIS_PLAYER),"whois",args,"*", sprintf("level=%d ",level(THIS_PLAYER)));

          IMC2_MSG("Sent a request on "+NETWORK_ID+" looking for "+args+"\n",THIS_PLAYER);
      }

      //Gets MUD Info for a mud 'args', returns it to user object 'towho'
      void getremotemudinfo(string args, object towho) {
          string str;

          if(!args) return notify_fail("Send info request to which MUD?\n");

          str=find_mud(args);

          if(!str) return notify_fail("MUD isn't known on "+NETWORK_ID+".\n");
          if(!mudinfo[str]["online"]) return notify_fail(NETWORK_ID+"- "+str+" is offline right now.\n");

          who_out(GET_CAP_NAME(THIS_PLAYER),str,"info");
          IMC2_MSG(NETWORK_ID"- Sent an info request to "+str+"\n",THIS_PLAYER);
      }

      //Write list of all channels to screen
      void allchans(string args, object towho) {
          string output;
          string a;

          output=NETWORK_ID+" channels:\n";
          output += sprintf("%-23s %-17s %-7s %-6s %-10s %-10s\n",
            "Name","Owner","Policy","Level","Suggested","Local Name");

          foreach(a in sort_array(keys(chaninfo),1)){
              output += sprintf("%-23s %-17s %-7s %-6s %-10s %-10s\n",
                a,chaninfo[a]["owner"],
                chaninfo[a]["policy"],
                chaninfo[a]["level"],chaninfo[a]["localname"],
                (localize_channel(a) ? localize_channel(a) : "<none>"));
          }
          IMC2_MSG(output,THIS_PLAYER);

      }

      //Send a channel command to the server
      void chancmd(string args, object towho) {
          string a, b, c;

          if(!ADMIN(THIS_PLAYER)) return notify_fail("You aren't allowed to use chancmd.\n");
          if(!args || (sscanf(args,"%s:%s %s",a,b,c)!=3))
              return notify_fail("Syntax: "+COMMAND_NAME+" chancmd hub:channel command\n");
          if(!chaninfo[a+":"+b])
              write(a+" is not listed as a channel, sending command anyway...\n");
          send_packet(GET_CAP_NAME(THIS_PLAYER),"ice-cmd","IMC",b, sprintf("channel=%s command=\"%s\"", a+":"+b,escape(c)));

          IMC2_MSG(sprintf("%s- Sent a command for the %s channel to %s\n",NETWORK_ID,a,GET_CAP_NAME(THIS_PLAYER),b),THIS_PLAYER);
      }

      int command(string str){
          // Takes the arguments given to the command which does IMC2 stuff.
          string cmd, args;
          string output;
          string a,b,c;
          int x,y;
          int emote,reply;
          object usr, *usrs=({ });

          if(IMC2_D->getonline() != 1) return 0;

          if(!str) str = "help";
          sscanf(str,"%s %s",cmd,args);
          if(!cmd) cmd=str;
          if(!args) args = "";

          switch(cmd){
          case "list":
              mudlist(this_player());
              return 1;
              break;
          case "setup":
              getstatus(this_player());
              return 1;
              break;
          case "info":
              mudinfo(args,this_player());
              return 1;
              break;
          case "ping":
              pingmud(args,this_player());
              return 1;
              break;
          case "who":
              mudwho(args,this_player());
              return 1;
              break;
          case "tell":
              tell(args,this_player());
              return 1;
              break;
          case "finger":
              finger(args,this_player());
              return 1;
              break;
          case "mudinfo":
              getremotemudinfo(args,this_player());
              return 1;
              break;
              /*	  case "chanwho":
                            if(!args) return notify_fail("What channel?\n");
                            if(sscanf(args,"%s %s",a,b)!=2){
                                a=args;
                                b="";
                            }
                            else {
                                if(b!="*") b=find_mud(b);
                            }
                            if(!b) return notify_fail("MUD isn't known on "+NETWORK_ID+".\n");
                            if(!localchaninfo[a]) return notify_fail("Invalid channel.\n");
                            if(b==""){ // check who's on locally
                                c = "The following users are listening to "+a+":\n  ";
                                foreach(usr in users())
                                if(chan_listening(usr,a)){
                                    usrs += ({ usr });
                                    c += " "+GET_CAP_NAME(usr);
                                }
                                c += "\n";
                                if(!usrs) c="Nobody on this mud is listening to that channel.\n";
                                IMC2_MSG(c,THIS_PLAYER);
                                return 1;
                            }
                            if((b!="*") && (!mudinfo[b]["online"]))
                                return notify_fail(NETWORK_ID+"- "+b+" is offline right now.\n");
                            chanwho_out(THIS_PLAYER,a,b);
                            IMC2_MSG(sprintf("%s- Sent a channel who request to %s for the %s channel.\n",
                                NETWORK_ID,b,a),THIS_PLAYER);
                            return 1;
                            break;*/
          case "allchans":
              allchans(args,this_player());
              return 1;
              break;
          case "beep":
              interbeep(args,this_player());
              return 1;
              break;
          case "whois":
              whois(args,this_player());
              return 1;
              break;
          case "chancmd":
              chancmd(args,this_player());
              return 1;
              break;
          case "help": // drop through to default
          default:
              IMC2_MSG(main_help(),THIS_PLAYER);
              return 1;
              break;
          }
      }

      string main_help(){
          return sprintf(@EndText
IMC2 system by Tim, set up for %s.
To use this, type the command '%s' followed by one of the following:
	chans - lists the channels that this MUD uses
	allchans - lists all the channels on this network
	chan (channel) (message) - talks on a channel
	chanemote (channel) (message) - emotes on a channel
	chansocial (channel) (message) - does a social on a channel
	chanwho (channel) (mud) - checks who is listening to a channel
	backlog (channel) - reads last %d messages on the channel
	listen (channel) - listen to a channel
	unlisten (channel) - stop listening to a channel
	info (name) - lists information about a MUD
	list - lists the MUDs on this network
	beep (name)@(mud) - send a beep through the network
	tell (name)@(mud) (message) - send a tell through the network
	tellemote (name)@(mud) (message) - send an emote through the network
	tells - shows your last %d tells
	finger (name)@(mud) - send a finger request for information about name@mud
	reply (message) - reply to the last incoming tell you received
	replyemote (message) - reply with an emote to the last incoming tell you received
	ping (mud) - pings a mud
	setup - shows information about this IMC2 network
	help - see this help message
Admin commands:
	configchan (local_name) (remote_name) (level_number) - configures a channel locally
	unconfigchan (local_name) - removes a locally configured channel
	chancmd (channel) (command) - for remote channel administration
	chanperms - lists the permission levels that are possible for configchan
EndText, NETWORK_ID,COMMAND_NAME,BACKLOG_SIZE,BACKLOG_SIZE);
      }

      string html(string str){
          string mud, *muds;
          string a,b;
          int x=0,y=0;
          string output="";
          if(!str) str="";
          sscanf(str,"%s_%s",a,b);
          if(!a) a=str;
          switch(a){
          case "list":
              muds = sort_array(keys(mudinfo),1);
              if (!sizeof(mudinfo)){
                  return ("There are no muds on the "+NETWORK_ID+" network!\n");
              }
              else{
                  output="<tr><td><b>Status</b></td><td><b>Name</b></td><td><b>Network</b></td><td><b>IMC2 Version</b></td></tr>\n";
                  foreach (mud in muds){
                      if(mudinfo[mud]["online"]) x++; else y++;
                      output += sprintf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
                        (mudinfo[mud]["online"] ? "UP" : "DOWN"),
                        mudinfo[mud]["url"] ? "<a href=\""+
                        ((mudinfo[mud]["url"][0..6]!="http://") ? "http://"+mudinfo[mud]["url"] : mudinfo[mud]["url"])
                        +"\">"+mud+"</a>" : mud,
                        mudinfo[mud]["networkname"],
                        mudinfo[mud]["versionid"]);
                  }
              }
              return sprintf("<B>%d of %d MUDs are on %s.</B.\n<BR><table>%s</table>",
                x,x+y,NETWORK_ID,output);
              break;
          case "backlog":
              if(b && localchaninfo[b] && (localchaninfo[b]["perm"]==BACKLOG_WEB_LEVEL)){
                  // Show backlog for channel.
                  return b+" channel backlog:\n"+implode(localchaninfo[b]["backlog"],"\n");
              }
              // List the channels
              output = NETWORK_ID+" channels on "+MUDNAME+":\n";
              foreach(b in sort_array(keys(localchaninfo),1)){
                  if(localchaninfo[b]["perm"]==BACKLOG_WEB_LEVEL) // is public
                      output += "<a href=\""+HTML_LOCATION+"backlog_"+b+"\">";
                  output += b;
                  output += " - "+chan_perm_desc(localchaninfo[b]["perm"]);
                  if(localchaninfo[b]["perm"]==BACKLOG_WEB_LEVEL) // is public
                      output += " - on web</a>";
                  else
                      output += " - not on web";
                  output += "\n";
              }
              return output;
              break;
          default:
              return MUDNAME+" uses Tim's LPC IMC2 system to connect to the "+
              NETWORK_ID+" network.\n"+
              "(version:"+VERSION+")\n"+
              "From here, you can look at the <a href=\""+HTML_LOCATION+"list\">list of muds</a>,\n"+
              "view the <a href=\""+HTML_LOCATION+"backlog\">public channel backlogs</a>,\n"+
              "or go to the main <a href=\""+URL+"\">"+MUDNAME+"</a> web site.\n";
              break;
          }
      }

      int clean_up(){ return 0; }


      void forget_user(string str){ map_delete(tells,str); }

      void eventChangeIMC2Passwords(){
          string cpass = alpha_crypt(10);
          string spass = alpha_crypt(10);
          string filename = "/secure/include/config.h";
          string file = read_file(filename);

          if(!file || !sizeof(file)) return;

          file = replace_string(file,"clientpass",cpass);
          file = replace_string(file,"serverpass",spass);

          write_file(filename, file, 1);

          return;
      }
