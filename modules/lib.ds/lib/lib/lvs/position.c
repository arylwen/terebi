/*    /lib/position.c
 *    From the Dead Souls Object Library
 *    Handles the positioning of living things
 *    Created by Descartes of Borg 961221
 *    Version: @(#) position.c 1.1@(#)
 *    Last modified: 96/12/21
 */

#include <position.h>
#include <medium.h>
#include <message_class.h>
#include <daemons.h>

inherit LIB_FALL;
inherit LIB_SINK;

private int           Position = POSITION_STANDING;
private static object Chair    = 0;

// abstract methods
varargs mixed eventPrint(string str, mixed *args...);
// end abstract methods

int GetPosition(){
    return Position;
}

int SetPosition(int x){
    Position = x;
}

mixed eventFall(){
    return fall::eventFall();
}

#if 0
mixed eventFall(){
    object env = environment();
    mixed rumbo;
    if(!env || !(rumbo = env->GetExit("down"))) return 0;
    if(env->GetMedium() != MEDIUM_AIR){
        send_messages("fall", "$agent_name $agent_verb to the ground.",
          this_object(), 0, environment(), 0);
        Position = POSITION_LYING;
        return 1;
    }
    else {
        rumbo = load_object(rumbo);
        if(!rumbo) return 0; 
        say(this_object()->GetName()+" plummets in from above.");
        write("You plummet downward!");
        if(this_object()->eventMove(rumbo)){
            env->eventPrint(this_object()->GetName()+" continues "+
              possessive(this_player())+" fall downward.", MSG_ENV);
            call_out( "eventFall", 1);
        }
        return 1;
    }
    return 0;
}
#endif

varargs mixed eventLay(object target){
    mixed tmp;

    if( Position != POSITION_STANDING && Position != POSITION_SITTING){
        eventPrint("You must be standing or sitting in order to lie.");
        return 1;
    }
    if( !target ){
        send_messages("lie", "$agent_name $agent_verb down.", this_object(),
          0, environment(), 0);
        Position = POSITION_LYING;
        return 1;
    }
    tmp = target->eventReceiveLay(this_object());
    if( tmp != 1 ){
        if( !tmp ){
            eventPrint("You cannot lie there!");
        }
        else {
            eventPrint(tmp);
        }
        return 1;
    }
    send_messages("lie", "$agent_name $agent_verb down on " +
      target->GetShort() + ".", this_object(), 0, environment(), 0);
    Position = POSITION_LYING;
    Chair = target;
    return 1;

}

varargs mixed eventKneel(object target){
    mixed tmp;

    if( Position != POSITION_STANDING && Position != POSITION_SITTING){
        eventPrint("You must be standing or sitting in order to kneel.");
        return 1;
    }
    if( !target ){
        send_messages("kneel", "$agent_name $agent_verb down.", this_object(),
          0, environment(), 0);
        Position = POSITION_KNEELING;
        return 1;
    }
#if 0
    tmp = target->eventReceiveLay(this_object());
    if( tmp != 1 ){
        if( !tmp ){
            eventPrint("You cannot lie there!");
        }
        else {
            eventPrint(tmp);
        }
        return 1;
    }
    send_messages("lie", "$agent_name $agent_verb down on " +
      target->GetShort() + ".", this_object(), 0, environment(), 0);
    Position = POSITION_LYING;
    Chair = target;
#endif
    return 1;
}

varargs mixed eventSit(object target){
    mixed tmp;

    if( Position != POSITION_STANDING && Position != POSITION_LYING ){
        eventPrint("You can't sit from that position.");
        return 1;
    }
    if( !target ){
        if( Position == POSITION_STANDING){
            send_messages("sit", "$agent_name $agent_verb down.", 
              this_object(),0, environment(), 0);
        }
        else send_messages("sit", "$agent_name $agent_verb up.",
              this_object(),0, environment(),0);

        Position = POSITION_SITTING;
        return 1;
    }
    tmp = target->eventReceiveSit(this_object());
    if( tmp != 1 ){
        if( !tmp ){
            eventPrint("You cannot sit there!");
        }
        else {
            eventPrint(tmp);
        }
        return 1;
    }
    send_messages("sit", "$agent_name $agent_verb down on " +
      target->GetShort() + ".", this_object(), 0, environment(), 0);
    Position = POSITION_SITTING;
    Chair = target;
    return 1;
}

mixed eventFly(){
    if( Chair ){
        mixed tmp = Chair->eventReleaseStand(this_object());

        if( tmp != 1 ){
            if( !tmp ){
                eventPrint("You cannot get up!");
            }
            else {
                eventPrint(tmp);
            }
            return 1;
        }
        Chair = 0;
    }
    if(this_object()->CanFly() && Position != POSITION_FLYING){
        tell_object(this_object(),"You begin flying.");
        say(this_player()->GetName()+" begins flying and hovers in the air.");
        Position = POSITION_FLYING;
    }
    return 1;
}

mixed eventSwim(){
    if( Chair ){
        mixed tmp = Chair->eventReleaseStand(this_object());

        if( tmp != 1 ){
            if( !tmp ){
                eventPrint("You cannot!");
            }
            else {
                eventPrint(tmp);
            }
            return 1;
        }
        Chair = 0;
    }
    if(this_object()->CanSwim() && Position != POSITION_SWIMMING){
        tell_object(this_object(),"You begin swimming.");
        say(this_player()->GetName()+" begins swimming.");
        Position = POSITION_SWIMMING;
    }
    return 1;
}

mixed eventFloat(){
    if( Chair ){
        mixed tmp = Chair->eventReleaseStand(this_object());

        if( tmp != 1 ){
            if( !tmp ){
                eventPrint("You can't!");
            }
            else {
                eventPrint(tmp);
            }
            return 1;
        }
        Chair = 0;
    }
    if(this_object()->CanFloat() && Position != POSITION_FLOATING){
        tell_object(this_object(),"You begin floating.");
        say(this_player()->GetName()+" begins floating.");
        Position = POSITION_FLOATING;
    }
    return 1;
}

mixed eventLand(){
    object env = environment();
    if(!env) return 0;
    if(! Position == POSITION_FLYING ) return 0;
    if( env->GetMedium() == MEDIUM_AIR || env->GetMedium() == MEDIUM_WATER ||
      env->GetMedium() == MEDIUM_SPACE ) return 0;  
    write("You stop flying.");
    say(this_player()->GetName()+" stops flying.");
    if(stringp(hobbled(this_player()))) Position = POSITION_STANDING;
    else Position = POSITION_LYING;
    return 1;
}

mixed eventStand(){
    if(!stringp(hobbled(this_player()))){
        eventPrint("Your injuries prevent you from standing.");
        return 1;
    }
    if(RACES_D->GetLimblessRace(this_player()->GetRace()) ){
        eventPrint("You aren't endowed with limbs with which to stand.");
        return 1;
    }
    if( Position == POSITION_STANDING ){
        eventPrint("You are already standing!");
        return 1;
    }
    if( Position == POSITION_FLYING){
        return eventLand();
    }

    if( Chair ){
        mixed tmp = Chair->eventReleaseStand(this_object());

        if( tmp != 1 ){
            if( !tmp ){
                eventPrint("You cannot get up!");
            }
            else {
                eventPrint(tmp);
            }
            return 1;
        }
        Chair = 0;
    }
    Position = POSITION_STANDING;
    send_messages("stand", "$agent_name $agent_verb up.", this_object(), 0,
      environment(), 0);
    return 1;
}

object GetFurniture(){
    if(Chair) return Chair;
}
string GetFurnitureName(){
    if(Chair) return Chair->GetShort();
    else return "none";
}
