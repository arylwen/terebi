#include <lib.h>
#include <rooms.h>

inherit LIB_ROOM;

void create() {
    room::create();
    SetClimate("indoors");
    SetAmbientLight(30);
    SetShort("The start room");
    SetLong("The default start room. To enter "+
      "a sample set of rooms, go down.");
    SetExits( ([ 
        "down" : "/domains/campus/room/start",
      ]) );
    SetNoModify(1);
	write_file("log_gab", "domain/default/room/start.c created \n");
}
void init(){
    ::init();
}
