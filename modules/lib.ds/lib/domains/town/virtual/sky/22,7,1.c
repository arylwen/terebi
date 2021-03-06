#include <medium.h>
#include <terrain_types.h>
#include <lib.h>
#include <rooms.h>

inherit LIB_ROOM;

void create() {
    room::create();
    SetAmbientLight(30);
    SetDayLight(30);
    SetNightLight(30);
    SetShort("The sky");
    SetLong("You are in the air above a vast sea that stretches from horizon to horizon.");
    SetClimate("temperate");
    SetTown("wilderness");
    SetTerrainType(T_MIDAIR);
    SetMedium(MEDIUM_AIR);
    SetItems( ([
        ({ "sea", "ocean" }) : "A seemingly endless body of water.",
      ]) );
    SetExits( ([
        "north" : "/domains/town/virtual/sky/22,8,1",
        "south" : "/domains/town/virtual/sky/22,6,1",
        "east" : "/domains/town/virtual/sky/23,7,1",
        "southeast" : "/domains/town/virtual/sky/23,6,1",
        "northeast" : "/domains/town/virtual/sky/23,8,1",
        "west" : "/domains/town/virtual/sky/21,7,1",
        "southwest" : "/domains/town/virtual/sky/21,6,1",
        "northwest" : "/domains/town/virtual/sky/21,8,1",
        "up" : "/domains/town/virtual/sky/22,7,2",
        "down" : "/domains/fort/room/alley.c",
      ]) );
}
void init(){
    ::init();
}
