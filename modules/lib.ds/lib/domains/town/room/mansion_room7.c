#include <lib.h>
inherit LIB_ROOM;

private int Rats;
int Rats = random(2);

static void create() {
    room::create();
    SetClimate("indoors");
    SetAmbientLight(30);
    SetShort("kitchen");
    SetLong("This is a very large kitchen designed to "
      "accommodate dozens of cooks. It looks like nobody "
      "has prepared a meal here in quite some time.");
    SetExits( ([
        "west" : "/domains/town/room/mansion_dhall3"
      ]) );
    SetInventory( ([
        "/domains/town/obj/rack" : 1,
        "/domains/town/obj/stove" : 1,
        "/domains/town/npc/rat" : Rats,
      ]) );
}

void init(){
    room::init();
}
